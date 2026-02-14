// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "ITestBroadcaster.h"

#include "TestBroadcasterImpl.generated.h"

UCLASS()
class UTestBroadcasterImpl : public UObject, public ITestBroadcaster
{
    GENERATED_BODY()

public:
    FTestIntChangedSignature& OnIntValueChanged() override { return ValueChangedDelegate;}
    FTestIntChangedSignature& OnIntValueChangedConst() const override { return const_cast<FTestIntChangedSignature&>(ValueChangedConstDelegate); }

    FTestIntChangedSignature ValueChangedDelegate;
    FTestIntChangedSignature ValueChangedConstDelegate;
};
