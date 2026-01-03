#include "Components/FlowInputCounterComponent.h"
#include "ActorFlowUtils.h"

UFlowInputCounterComponent::UFlowInputCounterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentCount = 0;
	MaxCount = 1;
	bDoTriggerOnlyOnce = true;
}


void UFlowInputCounterComponent::Input(int IncreaseBy)
{
	if (!bIsEnabled)
	{
		return;
	}
	CurrentCount = CurrentCount + IncreaseBy;
	if (CurrentCount >= MaxCount)
	{
		FLOW_EMIT("Output");
		if (bDoTriggerOnlyOnce)
		{
			SetEnable(false);
		}
	}
}

void UFlowInputCounterComponent::SingleInput()
{
	Input(1);
}


