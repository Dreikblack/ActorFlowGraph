#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class ACTORFLOWGRAPHEDITOR_API FActorFlowConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
private:
    UEdGraph* GraphObj;
public:
    FActorFlowConnectionDrawingPolicy(
        int32 InBackLayerID,
        int32 InFrontLayerID,
        float InZoomFactor,
        const FSlateRect& InClippingRect,
        FSlateWindowElementList& InDrawElements,
        UEdGraph* InGraph);

    virtual void DrawConnection(
        int32 LayerId,
        const FVector2f& Start,
        const FVector2f& End,
        const FConnectionParams& Params) override;

    virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
};
