// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"

#include "K2Node_InterfaceEventBase.generated.h"

struct FEventEntry;

UCLASS()
class DRUINTERFACEEVENTSEDITOR_API UK2Node_InterfaceEventBase : public UK2Node
{
    GENERATED_BODY()

public:
    //~ Begin UEdGraphNode Interface
    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    FText GetTooltipText() const override;
    void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;
    //~ End UEdGraphNode Interface

    //~ Begin UK2Node Interface
    //bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) override;
    FText GetMenuCategory() const override;
    //~ End UK2Node Interface

    //~ Begin UObject Interface
    bool Modify(bool bAlwaysMarkDirty = true) override;
    //~ End UObject Interface

    /* Name of Event getter method */
    UPROPERTY()
    FName EventId;

protected:
    virtual FText GetNodeTitleForCache(ENodeTitleType::Type TitleType) const { return FText::GetEmpty(); };
    void CommonGetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const;
    void CommonAllocateDefaultPins(bool bCreateEventPin, bool bCreateListenerPin);
    void CommonExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, FName FunctionName);
    const FEventEntry* GetEventEntry() const;

private:
    /* Constructing FText strings can be costly, so we cache the node's title */
    FNodeTextTable CachedTexts;
};
