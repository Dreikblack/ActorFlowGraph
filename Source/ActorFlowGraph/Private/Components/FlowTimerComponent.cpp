#include "Components/FlowTimerComponent.h"
#include "ActorFlowUtils.h" 

UFlowTimerComponent::UFlowTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsEnabled = false;
	bDoTriggerOnlyOnce = false;
	TimerRate = 60.0f;
}


// Called when the game starts
void UFlowTimerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (bIsEnabled)
		{
			SetTimer();
		}
		else
		{
			World->GetTimerManager().PauseTimer(TimerHandle);
		}
	}
}

void UFlowTimerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UFlowTimerComponent::SetTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle, this, &UFlowTimerComponent::OnTimer, TimerRate, !bDoTriggerOnlyOnce);
	}
}

void UFlowTimerComponent::OnTimer()
{
	if (bDoTriggerOnlyOnce)
	{
		bIsEnabled = false;
	}
	FLOW_EMIT("OnTimer");
}

void UFlowTimerComponent::SetEnable(bool bInEnable)
{
	if (bIsEnabled != bInEnable)
	{
		bIsEnabled = bInEnable;
		if (UWorld* World = GetWorld())
		{
			if (bIsEnabled)
			{
				World->GetTimerManager().UnPauseTimer(TimerHandle);
			}
			else
			{
				World->GetTimerManager().PauseTimer(TimerHandle);
			}
		}
	}
}

bool UFlowTimerComponent::IsPaused()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().IsTimerPaused(TimerHandle);
	}
	return !bIsEnabled;
}

bool UFlowTimerComponent::IsTimerActive()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().IsTimerActive(TimerHandle);
	}
	return bIsEnabled;
}

float UFlowTimerComponent::GetTimerRemaining()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(TimerHandle);
	}
	return 0.0f;
}
