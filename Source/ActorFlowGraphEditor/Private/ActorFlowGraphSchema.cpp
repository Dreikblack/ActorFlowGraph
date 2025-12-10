#include "ActorFlowGraphSchema.h"
#include "ActorFlowEdGraphNode.h"
#include "ActorFlowConnectionDrawingPolicy.h"


#define LOCTEXT_NAMESPACE "ActorFlowGraph"

void UActorFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
}

const FPinConnectionResponse UActorFlowGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins should be in and out"));
	}
	if (A == B)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same pin"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UActorFlowGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	bool bModified = Super::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		UActorFlowEdGraphNode* ActorFlowGraphNodeA = Cast<UActorFlowEdGraphNode>(PinA->GetOwningNode());
		UActorFlowEdGraphNode* ActorFlowGraphNodeB = Cast<UActorFlowEdGraphNode>(PinB->GetOwningNode());

		UEdGraph* EdGraph = ActorFlowGraphNodeA->GetGraph();

		EdGraph->NotifyNodeChanged(ActorFlowGraphNodeA);
		EdGraph->NotifyNodeChanged(ActorFlowGraphNodeB);
	}

	return bModified;
}

void UActorFlowGraphSchema::CreatePins(UClass* InCls, FName InName, UActorFlowEdGraphNode* Node)
{
	FString OutputsString = InCls->GetMetaData(TEXT("FlowOutputs"));

	TArray<FString> Outputs;
	OutputsString.ParseIntoArray(Outputs, TEXT(","), true);

	for (FString& Out : Outputs)
	{
		Out = Out.TrimStartAndEnd();
		auto pin = Node->CreatePin(EGPD_Output, TEXT("FlowPin"), InName, FName(*Out));
		pin->bNotConnectable = false;
	}

	for (TFieldIterator<UFunction> It(InCls, EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		UFunction* Func = *It;
		if (Func->HasMetaData(TEXT("FlowInput")))
		{
			auto pin = Node->CreatePin(EGPD_Input, TEXT("FlowPin"), InName, Func->GetFName());
			pin->bNotConnectable = false;
		}
	}
}

FConnectionDrawingPolicy* UActorFlowGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FActorFlowConnectionDrawingPolicy(
		InBackLayerID,
		InFrontLayerID,
		InZoomFactor,
		InClippingRect,
		InDrawElements,
		InGraphObj
	);
}

void UActorFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
}

void UActorFlowGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{

}

FLinearColor UActorFlowGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Green;
}

#undef LOCTEXT_NAMESPACE
