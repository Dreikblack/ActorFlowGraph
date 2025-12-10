#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ActorFlowEdGraphNode.generated.h"

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

protected:
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;

public:
	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FSoftObjectPath Actor;

	UPROPERTY()
	TArray<FComponentReference> Components;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;
	virtual bool IncludeParentNodeContextMenu() const override;
};
