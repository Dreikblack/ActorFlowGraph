#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputCounterComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "Output", FlowComponent))
class ACTORFLOWGRAPH_API UInputCounterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputCounterComponent();

protected:
	virtual void BeginPlay() override;

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
