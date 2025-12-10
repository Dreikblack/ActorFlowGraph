#include "ActorFlowGraphManagerActor.h"
#include "ActorFlowSubsystem.h"


AActorFlowGraphManagerActor::AActorFlowGraphManagerActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AActorFlowGraphManagerActor::BeginPlay()
{
	Super::BeginPlay();
	if (UActorFlowSubsystem* FlowSubsystem = GetWorld()->GetSubsystem<UActorFlowSubsystem>())
	{
		FlowSubsystem->InitFlowGraphs(GraphAssets);
	}
}

