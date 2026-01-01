#include "Components/FlowTimerComponent.h"
#include "ActorFlowUtils.h" 

UFlowTimerComponent::UFlowTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsEnable = false;
	bIsLooped = true;
	TimerRate = 60.0f;
}


// Called when the game starts
void UFlowTimerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (bIsEnable)
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
		World->GetTimerManager().SetTimer(TimerHandle, this, &UFlowTimerComponent::OnTimer, TimerRate, bIsLooped);
	}
}

void UFlowTimerComponent::OnTimer()
{
	if (!bIsLooped)
	{
		bIsEnable = false;
	}
	FLOW_EMIT("OnTimer");
}

void UFlowTimerComponent::SetEnable(bool bInEnable)
{
	if (bIsEnable != bInEnable)
	{
		bIsEnable = bInEnable;
		if (UWorld* World = GetWorld())
		{
			if (bIsEnable)
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
	return !bIsEnable;
}

bool UFlowTimerComponent::IsTimerActive()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().IsTimerActive(TimerHandle);
	}
	return bIsEnable;
}

float UFlowTimerComponent::GetTimerRemaining()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(TimerHandle);
	}
	return 0.0f;
}
