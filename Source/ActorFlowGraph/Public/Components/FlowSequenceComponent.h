#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlowSequenceComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "0,1", FlowComponent))
class ACTORFLOWGRAPH_API UFlowSequenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlowSequenceComponent();
protected:
	UPROPERTY(EditAnywhere, Category = "Flow | Sequence")
	bool bEnable;

	UPROPERTY(EditAnywhere, Category = "Flow | Sequence")
	int MaxOutputsCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Sequence")
	bool bDoTriggerOnlyOnce;

public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void Input();
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void SetEnable(bool bInEnable = true);
};
