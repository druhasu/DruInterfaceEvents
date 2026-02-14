// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "K2Node_InterfaceEventBase.h"

#include "K2Node_InterfaceEventRemoveAll.generated.h"

UCLASS()
class DRUINTERFACEEVENTSEDITOR_API UK2Node_InterfaceEventRemoveAll : public UK2Node_InterfaceEventBase
{
    GENERATED_BODY()

public:
    //~ Begin UK2Node Interface
    void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    //~ End UK2Node Interface

    //~ Begin UEdGraphNode Interface
    void AllocateDefaultPins() override;
    //~ End UEdGraphNode Interface

protected:
    FText GetNodeTitleForCache(ENodeTitleType::Type TitleType) const override;
};
