// Copyright Andrei Sudarikov. All Rights Reserved.

#include "InterfaceEventsStatics.h"
#include "EventRegistry.h"

void UInterfaceEventsStatics::AddEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName)
{
    if (InEventOwner.GetObject() == nullptr)
        return; // TODO: Report error

    FMulticastScriptDelegate* FoundDelegateInstance = FEventRegistry::FindDelegateInstance(InEventOwner.GetObject(), InEventId);
    if (FoundDelegateInstance == nullptr)
        return; // TODO: Report error

    FScriptDelegate NewDelegate;
    NewDelegate.BindUFunction(InObject, InFunctionName);

    FoundDelegateInstance->Add(NewDelegate);
}

void UInterfaceEventsStatics::RemoveEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName)
{
    if (InEventOwner.GetObject() == nullptr)
        return; // TODO: Report error

    FMulticastScriptDelegate* FoundDelegateInstance = FEventRegistry::FindDelegateInstance(InEventOwner.GetObject(), InEventId);
    if (FoundDelegateInstance == nullptr)
        return; // TODO: Report error

    FoundDelegateInstance->Remove(InObject, InFunctionName);
}

void UInterfaceEventsStatics::RemoveEventListenerObject(FScriptInterface InEventOwner, FName InEventId, UObject* InObject)
{
    if (InEventOwner.GetObject() == nullptr)
        return; // TODO: Report error

    FMulticastScriptDelegate* FoundDelegateInstance = FEventRegistry::FindDelegateInstance(InEventOwner.GetObject(), InEventId);
    if (FoundDelegateInstance == nullptr)
        return; // TODO: Report error

    FoundDelegateInstance->RemoveAll(InObject);
}

DEFINE_FUNCTION(UInterfaceEventsStatics::execUnpackDelegate)
{
    P_GET_PROPERTY_REF(FDelegateProperty, DelegateRef);
    P_GET_OBJECT_REF(UObject, OutObjectRef);
    P_GET_PROPERTY_REF(FNameProperty, OutFunctionNameRef);
    P_FINISH;

    P_NATIVE_BEGIN;

    UObject*& OutObject = P_ARG_GC_BARRIER(OutObjectRef);
    OutObject = DelegateRef.GetUObject();
    OutFunctionNameRef = DelegateRef.GetFunctionName();

    P_NATIVE_END;
}
