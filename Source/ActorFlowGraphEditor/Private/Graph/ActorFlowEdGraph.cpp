#include "Graph/ActorFlowEdGraph.h"
#include "Asset/ActorFlowGraphAsset.h"

UActorFlowGraphAsset* UActorFlowEdGraph::GetAsset() const
{
	return GetTypedOuter<UActorFlowGraphAsset>();
}
