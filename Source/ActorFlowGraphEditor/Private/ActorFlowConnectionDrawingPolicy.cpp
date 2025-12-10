#include "ActorFlowConnectionDrawingPolicy.h"

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

void FActorFlowConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2f& Start, const FVector2f& End, const FConnectionParams& Params)
{
	FConnectionDrawingPolicy::DrawConnection(LayerId, Start, End, Params);
}

void FActorFlowConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;

	// Get the schema and grab the default color from it
	check(OutputPin);
	check(GraphObj);
	const UEdGraphSchema* Schema = GraphObj->GetSchema();

	if (OutputPin->bOrphanedPin || (InputPin && InputPin->bOrphanedPin))
	{
		Params.WireColor = FLinearColor::Red;
	}

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;

	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}
}
