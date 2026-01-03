#pragma once

#include "CoreMinimal.h"
#include "Components/BaseFlowComponent.h"
#include "FlowSequenceComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "0,1", FlowComponent))
class ACTORFLOWGRAPH_API UFlowSequenceComponent : public UBaseFlowComponent
{
	GENERATED_BODY()

public:
	UFlowSequenceComponent();
protected:
	UPROPERTY(EditAnywhere, Category = "Flow | Sequence")
	int MaxOutputsCount;

public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void Input();
};
