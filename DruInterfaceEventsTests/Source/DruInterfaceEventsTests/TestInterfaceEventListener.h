// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "ITestBroadcaster.h"

#include "TestInterfaceEventListener.generated.h"

class ITestBroadcaster;

UCLASS(Blueprintable)
class UTestInterfaceEventListener : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void BindToEvent(const TScriptInterface<ITestBroadcaster>& Broadcaster);

    UFUNCTION(BlueprintImplementableEvent)
    void BindToEventConst(const TScriptInterface<ITestBroadcaster>& Broadcaster);

    UFUNCTION(BlueprintImplementableEvent)
    void UnbindFromEvent(const TScriptInterface<ITestBroadcaster>& Broadcaster);

    UFUNCTION(BlueprintImplementableEvent)
    void UnbindAllFromEvent(const TScriptInterface<ITestBroadcaster>& Broadcaster);

    UPROPERTY(BlueprintReadWrite)
    int32 LastEventValue = 0;

    UPROPERTY(BlueprintAssignable)
    FTestIntChangedSignature OnASDFG;
};
