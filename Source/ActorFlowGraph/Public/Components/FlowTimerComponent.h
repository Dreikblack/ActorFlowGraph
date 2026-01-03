#pragma once

#include "CoreMinimal.h"
#include "Components/BaseFlowComponent.h"
#include "FlowTimerComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "OnTimer", FlowComponent))
class ACTORFLOWGRAPH_API UFlowTimerComponent : public UBaseFlowComponent
{
	GENERATED_BODY()

public:	
	UFlowTimerComponent();

protected:
	FTimerHandle TimerHandle;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Timer")
	float TimerRate;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/*Set or Reset Timer*/
	UFUNCTION(BlueprintCallable, Category = "Flow | Timer", meta = (FlowInput))
	virtual void SetTimer();
public:	
	UFUNCTION(BlueprintCallable, Category = "Flow | Timer", meta = (FlowInput))
	void OnTimer();

	/*Pause or UnPause Timer*/
	virtual void SetEnable(bool bInEnable = true) override;

	UFUNCTION(BlueprintCallable)
	bool IsPaused();

	UFUNCTION(BlueprintCallable)
	bool IsTimerActive();

	UFUNCTION(BlueprintCallable)
	float GetTimerRemaining();
};
