#include "Components/FlowAndComponent.h"
#include "ActorFlowUtils.h" 

UFlowAndComponent::UFlowAndComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InputsMap.Add(false);
	InputsMap.Add(false);
	bDoTriggerOnlyOnce = true;
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
	if (!bIsEnabled || Index >= InputsMap.Num())
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

