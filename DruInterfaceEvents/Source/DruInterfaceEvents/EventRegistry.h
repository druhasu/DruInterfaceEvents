// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/ScriptDelegateFwd.h"
#include "Containers/Set.h"

struct FEventEntry
{
    using FDelegateGetterFunc = FMulticastScriptDelegate & (*) (UObject*);

    FName EventId;
    FDelegateGetterFunc DelegateGetter;

#if WITH_EDITORONLY_DATA
    FTopLevelAssetPath InterfaceClass;
    FTopLevelAssetPath DelegateSignature;
    FText EventDisplayName;
    FText EventTooltip;
#endif

    friend uint32 GetTypeHash(const FEventEntry& InEntry)
    {
        return GetTypeHash(InEntry.EventId);
    }

    friend bool operator== (const FEventEntry& A, const FEventEntry& B)
    {
        return A.EventId == B.EventId;
    }

    friend bool operator== (const FEventEntry& A, FName EventId)
    {
        return A.EventId == EventId;
    }
};

class FEventRegistry
{
public:
    using FDelegateGetterFunc = FMulticastScriptDelegate& (*) (UObject*);

    static FMulticastScriptDelegate* FindDelegateInstance(UObject* InOwner, FName InEventId);

    static DRUINTERFACEEVENTS_API void RegisterEvent(FEventEntry&& InEntry);
    static DRUINTERFACEEVENTS_API const TSet<FEventEntry>& GetAllEvents() { return Events; }

private:
    static TSet<FEventEntry> Events;
};

#define DRU_IEVENTS_GETTER_LAMBDA(ClassName, MethodName) [](UObject* Obj) -> FMulticastScriptDelegate& { return Cast<I ## ClassName>(Obj)->MethodName(); }

#if WITH_EDITORONLY_DATA
    #define DRU_IEVENTS_REGISTER_EVENT(ClassName, MethodName, ClassPath, DelegateSignature, Tooltip) \
        FEventRegistry::RegisterEvent(FEventEntry{ UE_STRINGIZE(ClassName ## _ ## MethodName), DRU_IEVENTS_GETTER_LAMBDA(ClassName, MethodName), FTopLevelAssetPath ClassPath, FTopLevelAssetPath DelegateSignature, INVTEXT(#MethodName), INVTEXT(Tooltip) })
#else
    #define DRU_IEVENTS_REGISTER_EVENT(ClassName, MethodName, ClassPath, DelegateSignature, Tooltip) \
        FEventRegistry::RegisterEvent(FEventEntry{ UE_STRINGIZE(ClassName ## _ ## MethodName), DRU_IEVENTS_GETTER_LAMBDA(ClassName, MethodName) })
#endif
