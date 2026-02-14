// Copyright Andrei Sudarikov. All Rights Reserved.

#include "EventRegistry.h"

TSet<FEventEntry> FEventRegistry::Events;

FMulticastScriptDelegate* FEventRegistry::FindDelegateInstance(UObject* InOwner, FName InEventId)
{
    const FEventEntry* FoundEntry = Events.FindByHash(GetTypeHash(InEventId), InEventId);
    if (FoundEntry == nullptr)
        return nullptr;

    return FoundEntry != nullptr ? &(*FoundEntry->DelegateGetter)(InOwner) : nullptr;
}

void FEventRegistry::RegisterEvent(FEventEntry&& InEntry)
{
    check(!InEntry.EventId.IsNone());
    check(InEntry.DelegateGetter != nullptr);
    checkf(!Events.Contains(InEntry), TEXT("Delegate getter already registered: '%s'"), *InEntry.EventId.ToString());

    Events.Emplace(InEntry);
}
