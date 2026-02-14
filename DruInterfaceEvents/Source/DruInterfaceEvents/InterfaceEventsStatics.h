// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "InterfaceEventsStatics.generated.h"

UCLASS()
class DRUINTERFACEEVENTS_API UInterfaceEventsStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static void AddEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName);

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static void RemoveEventListenerFunction(FScriptInterface InEventOwner, FName InEventId, UObject* InObject, FName InFunctionName);

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static void RemoveEventListenerObject(FScriptInterface InEventOwner, FName InEventId, UObject* InObject);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "InEvent", BlueprintInternalUseOnly = "true"))
    static void UnpackDelegate(int32 InEvent, UObject*& OutObject, FName& OutFunctionName) { checkNoEntry(); }
    DECLARE_FUNCTION(execUnpackDelegate);
};
