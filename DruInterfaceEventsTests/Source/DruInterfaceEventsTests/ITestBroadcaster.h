// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "ITestBroadcaster.generated.h"

UINTERFACE(MinimalApi, NotBlueprintable)
class UTestBroadcaster : public UInterface { GENERATED_BODY() };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTestIntChangedSignature, int32, NewValue);

class ITestBroadcaster
{
    GENERATED_BODY()

public:
    // Slashed Comment
    virtual FTestIntChangedSignature& OnIntValueChanged() = 0;

    /*
     Starred Comment
     May contain several lines and even quotes! " \"
     */
    virtual FTestIntChangedSignature& OnIntValueChangedConst() const = 0;
};
