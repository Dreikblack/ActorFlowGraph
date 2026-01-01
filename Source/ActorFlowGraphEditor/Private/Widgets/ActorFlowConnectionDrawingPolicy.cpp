#include "Widgets/ActorFlowConnectionDrawingPolicy.h"
#include "Graph/ActorFlowGraphSchema.h"
#include "Graph/ActorFlowEdGraph.h"
#include "Graph/ActorFlowEdGraphNode.h"

FActorFlowConnectionDrawingPolicy::FActorFlowConnectionDrawingPolicy(
	int32 InBackLayerID,
	int32 InFrontLayerID,
	float InZoomFactor,
	const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraph)
	: FConnectionDrawingPolicy(
		InBackLayerID,
		InFrontLayerID,
		InZoomFactor,
		InClippingRect,
		InDrawElements)	
{
	GraphObj = InGraph;
}

void FActorFlowConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;

	// Get the schema and grab the default color from it
	check(OutputPin);
	check(GraphObj);

	if (OutputPin->bOrphanedPin || (InputPin && InputPin->bOrphanedPin))
	{
		Params.WireColor = FLinearColor::Red;
	}

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;

	UActorFlowEdGraph* ActorGraph = Cast<UActorFlowEdGraph>(GraphObj);
	if (OutputPin && InputPin && ActorGraph->SelectedConnection == FGuidPair(OutputPin->PinId, InputPin->PinId))
	{
		Params.WireColor = FLinearColor::Yellow;
		Params.WireThickness = 3.0f;
	}

	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}
}

void FActorFlowConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2f& Start, const FVector2f& End, const FConnectionParams& Params)
{
	UActorFlowEdGraph* ActorGraph = Cast<UActorFlowEdGraph>(GraphObj);
	FConnectionDrawingPolicy::DrawConnection(LayerId, Start, End, Params);
	ActorGraph->SplineOverlapResult = SplineOverlapResult;
}
