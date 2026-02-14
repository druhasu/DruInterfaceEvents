// Copyright Andrei Sudarikov. All Rights Reserved.

#include "K2Node_InterfaceEventAdd.h"
#include "DruInterfaceEvents/EventRegistry.h"
#include "DruInterfaceEvents/InterfaceEventsStatics.h"

void UK2Node_InterfaceEventAdd::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    CommonExpandNode(CompilerContext, SourceGraph, GET_MEMBER_NAME_CHECKED(UInterfaceEventsStatics, AddEventListenerFunction));
}

void UK2Node_InterfaceEventAdd::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    CommonGetMenuActions(ActionRegistrar);
}

void UK2Node_InterfaceEventAdd::AllocateDefaultPins()
{
    CommonAllocateDefaultPins(true /* bCreateEventPin */, false /* bCreateListenerPin */);

    Super::AllocateDefaultPins();
}

FText UK2Node_InterfaceEventAdd::GetNodeTitleForCache(ENodeTitleType::Type TitleType) const
{
    const FEventEntry* FoundEntry = GetEventEntry();
    if (FoundEntry == nullptr)
        return FText::GetEmpty();

    return FText::Format(INVTEXT("Bind Event to {0}"), FoundEntry->EventDisplayName);
}
