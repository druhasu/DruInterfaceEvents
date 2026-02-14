// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "TestBroadcasterImpl.h"
#include "TestInterfaceEventListener.h"

BEGIN_DEFINE_SPEC(FInterfaceEventsSpec, "DruInterfaceEvents", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::ProductFilter)
UTestInterfaceEventListener* CreateListener() const;
END_DEFINE_SPEC(FInterfaceEventsSpec)

void FInterfaceEventsSpec::Define()
{
    It("Should Bind to event", [this]
    {
        UTestInterfaceEventListener* Listener = CreateListener();
        UTestBroadcasterImpl* Broadcaster = NewObject<UTestBroadcasterImpl>();

        Listener->BindToEvent(Broadcaster);
        UTEST_TRUE_EXPR(Broadcaster->OnIntValueChanged().IsBound());

        Broadcaster->ValueChangedDelegate.Broadcast(12);
        UTEST_EQUAL_EXPR(Listener->LastEventValue, 12);

        return true;
    });

    It("Should Bind to event const", [this]
    {
        UTestInterfaceEventListener* Listener = CreateListener();
        UTestBroadcasterImpl* Broadcaster = NewObject<UTestBroadcasterImpl>();

        Listener->BindToEventConst(Broadcaster);
        UTEST_TRUE_EXPR(Broadcaster->OnIntValueChangedConst().IsBound());

        Broadcaster->ValueChangedConstDelegate.Broadcast(12);
        UTEST_EQUAL_EXPR(Listener->LastEventValue, 12);

        return true;
    });

    It("Should Unbind from event", [this]
    {
        UTestInterfaceEventListener* Listener = CreateListener();
        UTestBroadcasterImpl* Broadcaster = NewObject<UTestBroadcasterImpl>();

        Listener->BindToEvent(Broadcaster);
        Listener->UnbindFromEvent(Broadcaster);

        UTEST_FALSE_EXPR(Broadcaster->OnIntValueChanged().IsBound());

        Broadcaster->ValueChangedDelegate.Broadcast(12);
        UTEST_EQUAL_EXPR(Listener->LastEventValue, 0);

        return true;
    });

    It("Should UnbindAll from event", [this]
    {
        UTestInterfaceEventListener* Listener = CreateListener();
        UTestBroadcasterImpl* Broadcaster = NewObject<UTestBroadcasterImpl>();

        Listener->BindToEvent(Broadcaster);
        Listener->UnbindAllFromEvent(Broadcaster);

        UTEST_FALSE_EXPR(Broadcaster->OnIntValueChanged().IsBound());

        Broadcaster->ValueChangedDelegate.Broadcast(12);
        UTEST_EQUAL_EXPR(Listener->LastEventValue, 0);

        return true;
    });
}

UTestInterfaceEventListener* FInterfaceEventsSpec::CreateListener() const
{
    UClass* ListenerClass = StaticLoadClass(UTestInterfaceEventListener::StaticClass(), nullptr, TEXT("/DruInterfaceEventsTests/BP_TestInterfaceEventListener.BP_TestInterfaceEventListener_C"));

    return NewObject<UTestInterfaceEventListener>(GetTransientPackage(), ListenerClass);
}
