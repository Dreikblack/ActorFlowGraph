#include "Components/FlowSequenceComponent.h"
#include "ActorFlowUtils.h" 


UFlowSequenceComponent::UFlowSequenceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxOutputsCount = 2;
	bDoTriggerOnlyOnce = false;
}

void UFlowSequenceComponent::Input()
{
	if (!bIsEnabled)
	{
		return;
	}
	if (bDoTriggerOnlyOnce)
	{
		SetEnable(false);
	}
	for (int i = 0; i < MaxOutputsCount; i++)
	{
		FLOW_EMIT(FString::FromInt(i));
	}
}