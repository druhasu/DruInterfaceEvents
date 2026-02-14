using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using EpicGames.Core;
using EpicGames.UHT.Parsers;
using EpicGames.UHT.Tables;
using EpicGames.UHT.Tokenizer;
using EpicGames.UHT.Types;
using EpicGames.UHT.Utils;

namespace DruInterfaceEvents;

[UnrealHeaderTool]
public static class InterfaceEventParser
{
    struct EventInfo
    {
        public UhtClass InterfaceClass;
        public string EventName;
        public UhtFunction DelegateSignature;
        public string Tooltip;
    };

    private const string ModuleName = "DruInterfaceEvents";
    private const string ExporterName = "DruInterfaceEventsGenerator";
    private const string ExporterNameFixup = "DruInterfaceEventsGeneratorFixup";

    private static readonly List<EventInfo> Events = [];
    private static readonly List<string> GeneratedFiles = [];

    private static readonly string FormatCommentForToolTipName = "FormatCommentForToolTip";
    private static readonly string AppendUTF8LiteralStringName = "AppendUTF8LiteralString";

    private static readonly MethodInfo? FormatCommentMethod = typeof(UhtParsingScope).GetMethod(FormatCommentForToolTipName, BindingFlags.Static | BindingFlags.NonPublic);
    private static readonly MethodInfo? AppendUTF8LiteralStringMethod = typeof(StringBuilderCache).Assembly
        .GetType("EpicGames.UHT.Utils.UhtStringBuilderExtensions")
        ?.GetMethod(AppendUTF8LiteralStringName, BindingFlags.Static | BindingFlags.Public, [typeof(StringBuilder), typeof(StringView)]);

    [UhtExporter(Name = ExporterName, ModuleName = ModuleName, Options = UhtExporterOptions.Default)]
    public static void GenerateCode(IUhtExportFactory factory)
    {
        if (FormatCommentMethod == null)
            throw new UhtException($"Method {FormatCommentForToolTipName} not found");

        if (AppendUTF8LiteralStringMethod == null)
            throw new UhtException($"Method {AppendUTF8LiteralStringName} not found");

        CollectEvents(factory);
        WriteGeneratedCode(factory);
    }

    [UhtExporter(Name = ExporterNameFixup, ModuleName = ModuleName, Options = UhtExporterOptions.Default)]
    public static void FixupGeneratedFileNames(IUhtExportFactory factory)
    {
        foreach (var originalFilepath in GeneratedFiles)
        {
            var originalWriteTime = File.GetLastWriteTime(originalFilepath);
            var newFilepath = originalFilepath[..^".keep".Length] + ".cpp";

            File.Copy(originalFilepath, newFilepath, true);
            File.SetLastWriteTime(newFilepath, originalWriteTime);
        }
    }

    private static List<EventInfo> CollectEvents(IUhtExportFactory factory)
    {
        List<EventInfo> result = [];
        UhtTokenBufferReader? commentReader = null;

        foreach (UhtHeaderFile header in factory.Session.HeaderFiles)
        {
#if UE_5_5_OR_LATER
            if (header.Module.IsPartOfEngine)
#else
            if (header.Package.IsPartOfEngine)
#endif
            {
                // skip Engine packages, no need to generate anything for them
                continue;
            }

            // create new reader for each header file
            commentReader = null;

            foreach (UhtClass uhtClass in header.Children.OfType<UhtClass>())
            {
                if (uhtClass.ClassType != UhtClassType.NativeInterface)
                    continue;

                if (uhtClass.Declarations == null)
                    continue;

                foreach (var declaration in uhtClass.Declarations)
                {
#if UE_5_6_OR_LATER
                    using UhtTokenReplayReaderBorrower borrowedReader = new(uhtClass.HeaderFile, uhtClass.HeaderFile.Data.Memory, declaration.Tokens, UhtTokenType.EndOfDeclaration);
                    IUhtTokenReader tokenReader = borrowedReader.Reader;
#else
                    UhtTokenReplayReader tokenReader = UhtTokenReplayReader.GetThreadInstance(uhtClass, uhtClass.HeaderFile.Data.Memory, declaration.Tokens, UhtTokenType.EndOfDeclaration);
#endif

                    if (!tokenReader.TryOptional("virtual"))
                        continue;

                    if (!tokenReader.TryOptionalIdentifier(out var delegateNameToken))
                        continue;

                    var delegateType = factory.Session.FindType(null, UhtFindOptions.DelegateFunction | UhtFindOptions.SourceName, delegateNameToken.ToString()) as UhtFunction;
                    if (delegateType == null)
                        continue;

                    if (!tokenReader.TryOptional("&"))
                    {
                        tokenReader.LogError($"'{delegateNameToken}' must be returned by reference");
                        continue;
                    }

                    if (!tokenReader.TryOptionalIdentifier(out var getterNameToken))
                    {
                        tokenReader.LogError("Cannot parse delegate getter method name");
                        continue;
                    }

                    try
                    {
                        tokenReader.Require('(').Require(')').Optional("const").Require('=').RequireConstInt().Require(';');
                    }
                    catch (UhtTokenException e)
                    {
                        tokenReader.LogError(e.Message);
                        continue;
                    }

                    string tooltip = string.Empty;
                    if (FormatCommentMethod != null)
                    {
                        commentReader ??= new UhtTokenBufferReader(header, header.Data.Memory);
                        var comment = GetRawComment(commentReader, declaration.Tokens[0].UngetLine);

                        if(comment.Length > 0)
                            tooltip = (string)FormatCommentMethod.Invoke(null, [comment])!;
                    }

                    Events.Add(new EventInfo
                    {
                        InterfaceClass = uhtClass,
                        EventName = getterNameToken.Value.ToString(),
                        DelegateSignature = delegateType,
                        Tooltip = tooltip,
                    });
                }
            }
        }

        return result;
    }

    private static string GetRawComment(UhtTokenBufferReader reader, int endLine)
    {
        while (!reader.IsEOF)
        {
            reader.SkipWhitespaceAndComments();
            reader.CommitPendingComments();

            if (reader.InputLine >= endLine)
            {
                return reader.Comments.Length switch
                {
                    0 => string.Empty,
                    1 => reader.Comments[0].ToString(),
                    _ => JoinComments(reader.Comments)
                };
            }
            else
            {
                reader.ClearComments();
                reader.GetLine();
            }
        }

        return string.Empty;

        static string JoinComments(ReadOnlySpan<StringView> comments)
        {
            using BorrowStringBuilder borrower = new(StringBuilderCache.Small);
            StringBuilder builder = borrower.StringBuilder;
            foreach (StringView comment in comments)
            {
                builder.Append(comment);
            }
            return builder.ToString();
        }
    }

    private static void WriteGeneratedCode(IUhtExportFactory factory)
    {
        GeneratedFiles.Clear();

        foreach (var eventsPerModule in Events.GroupBy(e => e.InterfaceClass.HeaderFile.Module))
        {
            var sortedIncludeFiles = eventsPerModule
                .Select(e => GetProperIncludePath(e.InterfaceClass.HeaderFile))
                .Distinct()
                .OrderBy(s => s)
                .ToArray();

            var sortedEvents = eventsPerModule
                .OrderBy(e => e.InterfaceClass.SourceName)
                .ThenBy(e => e.EventName)
                .ToArray();

            using BorrowStringBuilder borrower = new(StringBuilderCache.Big);
            var sb = borrower.StringBuilder;

            sb.AppendLine("// Copyright Andrei Sudarikov. All Rights Reserved.");
            sb.AppendLine();
            sb.AppendLine("/* -------------------------------------------------------- *");
            sb.AppendLine("     This file was generated by DruInterfaceEvents plugin");
            sb.AppendLine("     DO NOT modify it manually!");
            sb.AppendLine(" * -------------------------------------------------------- */");
            sb.AppendLine();

            sb.AppendLine("#ifdef DRUINTERFACEEVENTS_API");
            sb.AppendLine();

            sb.AppendLine("#include \"DruInterfaceEvents/EventRegistry.h\"");
            sb.AppendLine();

            foreach (string headerFile in sortedIncludeFiles)
            {
                sb.Append("#include \"");
                sb.Append(headerFile);
                sb.Append("\"");
                sb.AppendLine();
            }

            sb.AppendLine();

            sb.AppendLine("struct FInterfaceEventsRegistrator");
            sb.AppendLine("{");
            sb.AppendLine("    FInterfaceEventsRegistrator()");
            sb.AppendLine("    {");

            foreach (EventInfo entry in sortedEvents)
            {
                sb.Append("        DRU_IEVENTS_REGISTER_EVENT(");

                sb.Append(entry.InterfaceClass.EngineName).Append(", ");
                sb.Append(entry.EventName).Append(", ");

                sb.Append("(\"").Append(entry.InterfaceClass.Package.SourceName).Append("\", \"").Append(entry.InterfaceClass.EngineName).Append("\"), ");
                sb.Append("(\"").Append(entry.DelegateSignature.Package.SourceName).Append("\", \"").Append(entry.DelegateSignature.EngineName).Append("\"), ");

                if (AppendUTF8LiteralStringMethod != null)
                    AppendUTF8LiteralStringMethod?.Invoke(null, [sb, new StringView(entry.Tooltip)]);
                else
                    sb.Append("\"\"");

                sb.AppendLine(");");
            }

            sb.AppendLine("    }");

#if UE_5_5_OR_LATER
            UhtModule module = eventsPerModule.Key;
            string moduleName = module.Module.Name;
            string moduleOutputDirectory = module.Module.OutputDirectory;
#else
            UHTManifest.Module module = eventsPerModule.Key;
            string moduleName = module.Name;
            string moduleOutputDirectory = module.OutputDirectory;
#endif

            sb.Append("} GInterfaceEventsRegistrator_");
            sb.Append(moduleName);
            sb.AppendLine(";");
            sb.AppendLine();

            sb.AppendLine("#endif");

            var outputPath = Path.Combine(moduleOutputDirectory, $"{moduleName}.IEvents.gen.keep");
            GeneratedFiles.Add(outputPath);

            factory.CommitOutput(outputPath, sb);
        }
    }

    private static string GetProperIncludePath(UhtHeaderFile headerFile)
    {
        if (headerFile.ModuleRelativeFilePath.StartsWith("Public", StringComparison.OrdinalIgnoreCase))
        {
            // output relative path for files in the "Public" folder
            return headerFile.IncludeFilePath;
        }
        else if (headerFile.ModuleRelativeFilePath.StartsWith("Classes", StringComparison.OrdinalIgnoreCase))
        {
            // output relative path for files in the legacy "Classes" folder
            return headerFile.IncludeFilePath;
        }
        else
        {
            // otherwise output full path with module Name
#if UE_5_5_OR_LATER
            return headerFile.Module.Module.Name + "/" + headerFile.ModuleRelativeFilePath;
#else
            return headerFile.Package.Module.Name + "/" + headerFile.ModuleRelativeFilePath;
#endif
        }
    }
}
