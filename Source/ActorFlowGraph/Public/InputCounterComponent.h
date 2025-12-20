#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputCounterComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent), meta = (FlowOutputs = "Output", FlowComponent))
class ACTORFLOWGRAPH_API UInputCounterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputCounterComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Counter")
	int CurrentCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Counter")
	int MaxCount;

	UFUNCTION(BlueprintCallable, Category = "Counter", meta = (FlowInput))
	void Input(int IncreaseBy = 1);

	UFUNCTION(BlueprintCallable, Category = "Counter", meta = (FlowInput))
	void SingleInput();
};
