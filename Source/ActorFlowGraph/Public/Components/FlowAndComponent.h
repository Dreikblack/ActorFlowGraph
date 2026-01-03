#pragma once

#include "CoreMinimal.h"
#include "Components/BaseFlowComponent.h"
#include "FlowAndComponent.generated.h"


UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "Output", FlowComponent, DisplayName = "Flow Logical AND"))
class ACTORFLOWGRAPH_API UFlowAndComponent : public UBaseFlowComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Flow | And")
	TArray<bool> InputsMap;
public:	
	UFlowAndComponent();
public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void InputTrue(int Index = 0);

	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void InputFalse(int Index = 0);

	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void Input(int Index = 0, bool InValue = true);
};
