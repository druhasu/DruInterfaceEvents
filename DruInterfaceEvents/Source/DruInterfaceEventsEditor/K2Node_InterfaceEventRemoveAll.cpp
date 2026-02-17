// Copyright Andrei Sudarikov. All Rights Reserved.

#include "K2Node_InterfaceEventRemoveAll.h"
#include "DruInterfaceEvents/EventRegistry.h"
#include "DruInterfaceEvents/InterfaceEventsStatics.h"

void UK2Node_InterfaceEventRemoveAll::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    CommonExpandNode(CompilerContext, SourceGraph, GET_MEMBER_NAME_CHECKED(UInterfaceEventsStatics, RemoveEventListenerObject));
}

void UK2Node_InterfaceEventRemoveAll::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    CommonGetMenuActions(ActionRegistrar);
}

void UK2Node_InterfaceEventRemoveAll::AllocateDefaultPins()
{
    CommonAllocateDefaultPins(false /* bCreateEventPin */, true /* bCreateListenerPin */);

    Super::AllocateDefaultPins();
}

FText UK2Node_InterfaceEventRemoveAll::GetNodeTitleForCache(ENodeTitleType::Type TitleType) const
{
    const FEventEntry* FoundEntry = GetEventEntry();
    FText EventDisplayName = FoundEntry != nullptr ? FoundEntry->EventDisplayName : INVTEXT("<Unknown>");

    return FText::Format(INVTEXT("Unbind All Events from {0}"), EventDisplayName);
}
