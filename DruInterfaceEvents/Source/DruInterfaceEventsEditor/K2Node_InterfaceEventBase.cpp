// Copyright Andrei Sudarikov. All Rights Reserved.

#include "K2Node_InterfaceEventBase.h"
#include "DruInterfaceEvents/EventRegistry.h"
#include "DruInterfaceEvents/InterfaceEventsStatics.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"
#include "KismetCompiler.h"

FText UK2Node_InterfaceEventBase::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (!CachedTexts.IsTitleCached(TitleType, this))
    {
        CachedTexts.SetCachedTitle(TitleType, GetNodeTitleForCache(TitleType), this);
    }

    return CachedTexts.GetCachedTitle(TitleType);
}

FText UK2Node_InterfaceEventBase::GetTooltipText() const
{
    if (!CachedTexts.IsTooltipCached(this))
    {
        const FEventEntry* FoundEntry = GetEventEntry();
        if (FoundEntry != nullptr)
        {
            CachedTexts.SetCachedTooltip(FoundEntry->EventTooltip, this);
        }
    }

    return CachedTexts.GetCachedTooltip();
}

void UK2Node_InterfaceEventBase::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
    const FEventEntry* FoundEntry = GetEventEntry();
    if (FoundEntry != nullptr)
        return;

    // TODO: Report error
}

FText UK2Node_InterfaceEventBase::GetMenuCategory() const
{
    const FEventEntry* FoundEntry = GetEventEntry();
    if (FoundEntry == nullptr)
        return FText::GetEmpty();

    UClass* InterfaceClass = FindObject<UClass>(FoundEntry->InterfaceClass);
    if (InterfaceClass == nullptr)
        return FText::GetEmpty();

    return FEditorCategoryUtils::BuildCategoryString(FCommonEditorCategory::Class, InterfaceClass->GetDisplayNameText());
}

bool UK2Node_InterfaceEventBase::Modify(bool bAlwaysMarkDirty)
{
    CachedTexts.MarkDirty();

    return Super::Modify(bAlwaysMarkDirty);
}

void UK2Node_InterfaceEventBase::CommonGetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    if (ActionRegistrar.IsOpenForRegistration(GetClass()))
    {
        for (const FEventEntry& EventEntry : FEventRegistry::GetAllEvents())
        {
            UBlueprintNodeSpawner* Spawner = NewObject<UBlueprintNodeSpawner>();
            Spawner->NodeClass = GetClass();
            Spawner->DefaultMenuSignature.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.VariableIcon");
            Spawner->DefaultMenuSignature.IconTint = GetDefault<UGraphEditorSettings>()->DefaultPinTypeColor;

            Spawner->CustomizeNodeDelegate.BindLambda([=](UEdGraphNode* Node, bool bIsTemplateNode)
            {
                auto CastedNode = CastChecked<UK2Node_InterfaceEventBase>(Node);
                CastedNode->EventId = EventEntry.EventId;
            });

            ActionRegistrar.AddBlueprintAction(GetClass(), Spawner);
        }
    }
}

void UK2Node_InterfaceEventBase::CommonAllocateDefaultPins(bool bCreateEventPin, bool bCreateListenerPin)
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

    const FEventEntry* FoundEntry = GetEventEntry();
    if (FoundEntry != nullptr)
    {
        UClass* InterfaceClass = FindObject<UClass>(FoundEntry->InterfaceClass);
        UFunction* DelegateSignature = FindObject<UFunction>(FoundEntry->DelegateSignature);

        if (InterfaceClass != nullptr)
        {
            CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Interface, InterfaceClass, TEXT("Target"));
        }

        if (bCreateEventPin && DelegateSignature != nullptr)
        {
            UEdGraphPin* EventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Delegate, TEXT("Event"));
            EventPin->PinType.PinSubCategoryMemberReference.MemberParent = DelegateSignature->GetOuter();
            EventPin->PinType.PinSubCategoryMemberReference.MemberName = DelegateSignature->GetFName();
            EventPin->PinType.bIsReference = true;
            EventPin->PinType.bIsConst = true;
        }

        if (bCreateListenerPin)
        {
            UEdGraphPin* ListenerPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), UEdGraphSchema_K2::PN_Self);
            ListenerPin->PinFriendlyName = INVTEXT("Listener");
        }
    }
}

void UK2Node_InterfaceEventBase::CommonExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, FName FunctionName)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

    // spawn call to requested static event handling function
    UK2Node_CallFunction* EventFunctionCall = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    EventFunctionCall->FunctionReference.SetExternalMember(FunctionName, UInterfaceEventsStatics::StaticClass());
    EventFunctionCall->AllocateDefaultPins();

    // check if we have Event pin
    if (UEdGraphPin* InEventPin = FindPin(TEXT("Event")))
    {
        // spawn call to UnpackDelegate static function
        UK2Node_CallFunction* UnpackDelegateCall = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
        UnpackDelegateCall->FunctionReference.SetExternalMember(GET_MEMBER_NAME_CHECKED(UInterfaceEventsStatics, UnpackDelegate), UInterfaceEventsStatics::StaticClass());
        UnpackDelegateCall->AllocateDefaultPins();

        // make sure UnpackDelegate input pin has correct type
        UEdGraphPin* UnpackEventPin = UnpackDelegateCall->FindPin(TEXT("InEvent"));
        UnpackEventPin->PinType = InEventPin->PinType;

        // connect our Event pin to UnpackDelegate function
        CompilerContext.MovePinLinksToIntermediate(*InEventPin, *UnpackDelegateCall->FindPin(TEXT("InEvent")));

        // connect outputs of UnpackDelegate to AddEventListenerFunction
        Schema->TryCreateConnection(UnpackDelegateCall->FindPin(TEXT("OutObject")), EventFunctionCall->FindPin(TEXT("InObject")));
        Schema->TryCreateConnection(UnpackDelegateCall->FindPin(TEXT("OutFunctionName")), EventFunctionCall->FindPin(TEXT("InFunctionName")));
    }
    else if (UEdGraphPin* InListenerPin = FindPin(UEdGraphSchema_K2::PN_Self))
    {
        if (UEdGraphPin* InObjectPin = EventFunctionCall->FindPin(TEXT("InObject")))
        {
            if (InListenerPin->HasAnyConnections())
            {
                // connect our input "Self" pin to "InObject" pin of event handling function
                CompilerContext.MovePinLinksToIntermediate(*InListenerPin, *InObjectPin);
            }
            else
            {
                UK2Node_Self* SelfCall = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
                SelfCall->AllocateDefaultPins();

                Schema->TryCreateConnection(SelfCall->FindPin(UEdGraphSchema_K2::PN_Self), InObjectPin);
            }
        }
    }

    // set EventId to AddEventListenerFunction
    EventFunctionCall->FindPin(TEXT("InEventId"))->DefaultValue = EventId.ToString();

    // connect our "Target" pin to AddEventListenerFunction
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Target")), *EventFunctionCall->FindPin(TEXT("InEventOwner")));

    // connect our execute pins
    CompilerContext.MovePinLinksToIntermediate(*FindPin(UEdGraphSchema_K2::PN_Execute), *EventFunctionCall->GetExecPin());
    CompilerContext.MovePinLinksToIntermediate(*FindPin(UEdGraphSchema_K2::PN_Then), *EventFunctionCall->GetThenPin());
}

const FEventEntry* UK2Node_InterfaceEventBase::GetEventEntry() const
{
    return FEventRegistry::GetAllEvents().FindByHash(GetTypeHash(EventId), EventId);
}
