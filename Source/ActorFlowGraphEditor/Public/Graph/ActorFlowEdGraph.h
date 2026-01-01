#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "GraphSplineOverlapResult.h"
#include "ActorFlowEdGraphNode.h"

#include "ActorFlowEdGraph.generated.h"

class UActorFlowGraphAsset;

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowEdGraph : public UEdGraph
{
    GENERATED_BODY()

public:
    FGraphSplineOverlapResult SplineOverlapResult;
    FGuidPair SelectedConnection;
    UActorFlowGraphAsset* GetAsset() const;
};
