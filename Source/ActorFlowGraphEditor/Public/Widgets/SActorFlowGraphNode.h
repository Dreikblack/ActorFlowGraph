#pragma once

#include "SGraphNode.h"

class UActorFlowEdGraphNode;

class ACTORFLOWGRAPHEDITOR_API SActorFlowGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SActorFlowGraphNode)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UActorFlowEdGraphNode* InNode);

	virtual ~SActorFlowGraphNode() override;

protected:
	virtual void AddPinGroup(TSharedPtr<SVerticalBox> Parent, const FString& Title, const TArray<UEdGraphPin*>& Pins);
	virtual TSharedRef<SWidget> BuildPinGroupWidget(const TArray<UEdGraphPin*>& Pins);
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd);
public:
	virtual void UpdateGraphNode() override;
};
