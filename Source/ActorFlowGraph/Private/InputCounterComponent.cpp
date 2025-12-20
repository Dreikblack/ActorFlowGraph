#include "InputCounterComponent.h"
#include "ActorFlowUtils.h" 

UInputCounterComponent::UInputCounterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentCount = 0;
	MaxCount = 1;
}


void UInputCounterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInputCounterComponent::Input(int IncreaseBy)
{
	if (!IsActive())
	{
		return;
	}
	CurrentCount = CurrentCount + IncreaseBy;
	if (CurrentCount >= MaxCount)
	{
		SetActive(false);
		FLOW_EMIT("Output");
	}
}

void UInputCounterComponent::SingleInput()
{
	Input(1);
}


