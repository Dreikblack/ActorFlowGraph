#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlowAndComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "Output", FlowComponent))
class ACTORFLOWGRAPH_API UFlowAndComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Flow | And")
	TArray<bool> InputsMap;
public:	
	UFlowAndComponent();

protected:
	UPROPERTY(EditAnywhere, Category = "Flow | And")
	bool bEnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | And")
	bool bDoTriggerOnlyOnce;
public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void InputTrue(int Index = 0);

	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void InputFalse(int Index = 0);

	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void Input(int Index = 0, bool InValue = true);

public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void SetEnable(bool bInEnable = true);
};
