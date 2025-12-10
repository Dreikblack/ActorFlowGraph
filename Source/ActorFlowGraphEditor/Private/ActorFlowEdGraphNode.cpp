#include "ActorFlowEdGraphNode.h"
#include "SActorFlowGraphNode.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "ActorFlowGraphNode"

TSharedPtr<SGraphNode> UActorFlowEdGraphNode::CreateVisualWidget()
{
	return SNew(SActorFlowGraphNode, this);
}

void UActorFlowEdGraphNode::AllocateDefaultPins()
{
	//no default pins
}

FText UActorFlowEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(ActorName);
}

void UActorFlowEdGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	FToolMenuSection& Section = Menu->AddSection("FlowGraphNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
	Section.AddMenuEntry(GenericCommands.Delete);
}

bool UActorFlowEdGraphNode::CanUserDeleteNode() const
{
	return true;
}

bool UActorFlowEdGraphNode::CanDuplicateNode() const
{
	return false;
}

bool UActorFlowEdGraphNode::IncludeParentNodeContextMenu() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
