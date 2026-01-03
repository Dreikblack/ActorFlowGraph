#include "Components/FlowSequenceComponent.h"
#include "ActorFlowUtils.h" 


UFlowSequenceComponent::UFlowSequenceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxOutputsCount = 2;
	bEnable = true;
	bDoTriggerOnlyOnce = false;
}

void UFlowSequenceComponent::Input()
{
	if (!bEnable)
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

void UFlowSequenceComponent::SetEnable(bool bInEnable)
{
	bEnable = bInEnable;
}
