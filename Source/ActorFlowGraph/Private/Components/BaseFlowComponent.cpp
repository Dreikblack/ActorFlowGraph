#include "Components/BaseFlowComponent.h"


UBaseFlowComponent::UBaseFlowComponent()
{
	bIsEnabled = true;
	bDoTriggerOnlyOnce = false;
}

void UBaseFlowComponent::SetEnable(bool bInEnable)
{
	bIsEnabled = bInEnable;
}

