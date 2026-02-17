// Copyright Andrei Sudarikov. All Rights Reserved.

#include "InterfaceEventsStatics.h"
#include "EventRegistry.h"

#include "Blueprint/BlueprintExceptionInfo.h"

void UInterfaceEventsStatics::AddEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName)
{
    if (FMulticastScriptDelegate* FoundDelegateInstance = GetDelegateInstance(InEventOwner.GetObject(), InEventId))
    {
        FScriptDelegate NewDelegate;
        NewDelegate.BindUFunction(InObject, InFunctionName);

        FoundDelegateInstance->Add(NewDelegate);
    }
}

void UInterfaceEventsStatics::RemoveEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName)
{
    if (FMulticastScriptDelegate* FoundDelegateInstance = GetDelegateInstance(InEventOwner.GetObject(), InEventId))
    {
        FoundDelegateInstance->Remove(InObject, InFunctionName);
    }
}

void UInterfaceEventsStatics::RemoveEventListenerObject(FScriptInterface InEventOwner, FName InEventId, UObject* InObject)
{
    if (FMulticastScriptDelegate* FoundDelegateInstance = GetDelegateInstance(InEventOwner.GetObject(), InEventId))
    {
        FoundDelegateInstance->RemoveAll(InObject);
    }
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

FMulticastScriptDelegate* UInterfaceEventsStatics::GetDelegateInstance(UObject* InEventOwner, FName InEventId)
{
    if (InEventOwner == nullptr)
    {
        RaiseScriptError(TEXT("Accessed None trying to get Object from interface"));
        return nullptr;
    }

    FMulticastScriptDelegate* FoundDelegateInstance = FEventRegistry::FindDelegateInstance(InEventOwner, InEventId);
    if (FoundDelegateInstance == nullptr)
    {
        RaiseScriptError(FString::Printf(TEXT("Delegate instance with name '%s' not found"), *InEventId.ToString()));
        return nullptr;
    }

    return FoundDelegateInstance;
}

void UInterfaceEventsStatics::RaiseScriptError(const FString& InError)
{
#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
    FFrame* TopFrame = FFrame::GetThreadLocalTopStackFrame();

#if WITH_EDITOR
    if (TopFrame)
    {
        const FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, FText::FromString(InError));
        FBlueprintCoreDelegates::ThrowScriptException(TopFrame->Object, *TopFrame, ExceptionInfo);
    }
    else
#endif
    {
        UE_LOG(LogBlueprintUserMessages, Error, TEXT("%s:\n%s"), *InError, *TopFrame->GetStackTrace());
    }
#endif
}
