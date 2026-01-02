#include "Components/FlowAndComponent.h"
#include "ActorFlowUtils.h" 

UFlowAndComponent::UFlowAndComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InputsMap.Add(false);
	InputsMap.Add(false);
	bEnable = true;
	bDoTriggerOnlyOnce = true;
}

void UFlowAndComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFlowAndComponent::SetEnable(bool bInEnable)
{
	bEnable = bInEnable;
}

void UFlowAndComponent::InputTrue(int Index)
{
	Input(Index, true);
}

void UFlowAndComponent::InputFalse(int Index)
{
	Input(Index, false);
}
void UFlowAndComponent::Input(int Index, bool InValue)
{
	if (!bEnable || Index >= InputsMap.Num())
	{
		return;
	}
	InputsMap[Index] = InValue;
	for (bool Value : InputsMap)
	{
		if (!Value)
		{
			return;
		}
	}
	if (bDoTriggerOnlyOnce)
	{
		SetEnable(false);
	}
	FLOW_EMIT("Output");
}

