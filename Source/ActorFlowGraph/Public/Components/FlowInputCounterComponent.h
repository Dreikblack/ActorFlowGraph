#pragma once

#include "CoreMinimal.h"
#include "Components/BaseFlowComponent.h"
#include "FlowInputCounterComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "Output", FlowComponent))
class ACTORFLOWGRAPH_API UFlowInputCounterComponent : public UBaseFlowComponent
{
	GENERATED_BODY()

public:
	UFlowInputCounterComponent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Counter")
	int CurrentCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Counter")
	int MaxCount;

	UFUNCTION(BlueprintCallable, Category = "Flow | Counter", meta = (FlowInput))
	void Input(int IncreaseBy = 1);

	UFUNCTION(BlueprintCallable, Category = "Flow | Counter", meta = (FlowInput))
	void SingleInput();
};
