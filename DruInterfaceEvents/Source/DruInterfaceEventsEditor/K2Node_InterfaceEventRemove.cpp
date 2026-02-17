// Copyright Andrei Sudarikov. All Rights Reserved.

#include "K2Node_InterfaceEventRemove.h"
#include "DruInterfaceEvents/EventRegistry.h"
#include "DruInterfaceEvents/InterfaceEventsStatics.h"

void UK2Node_InterfaceEventRemove::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    CommonExpandNode(CompilerContext, SourceGraph, GET_MEMBER_NAME_CHECKED(UInterfaceEventsStatics, RemoveEventListenerFunction));
}

void UK2Node_InterfaceEventRemove::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    CommonGetMenuActions(ActionRegistrar);
}

void UK2Node_InterfaceEventRemove::AllocateDefaultPins()
{
    CommonAllocateDefaultPins(true /* bCreateEventPin */, false /* bCreateListenerPin */);

    Super::AllocateDefaultPins();
}

FText UK2Node_InterfaceEventRemove::GetNodeTitleForCache(ENodeTitleType::Type TitleType) const
{
    const FEventEntry* FoundEntry = GetEventEntry();
    FText EventDisplayName = FoundEntry != nullptr ? FoundEntry->EventDisplayName : INVTEXT("<Unknown>");

    return FText::Format(INVTEXT("Unbind Event from {0}"), EventDisplayName);
}
