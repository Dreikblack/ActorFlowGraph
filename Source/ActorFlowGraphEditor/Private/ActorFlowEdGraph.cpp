#include "ActorFlowEdGraph.h"

UActorFlowGraphAsset* UActorFlowEdGraph::GetAsset() const
{
	return GetTypedOuter<UActorFlowGraphAsset>();
}
