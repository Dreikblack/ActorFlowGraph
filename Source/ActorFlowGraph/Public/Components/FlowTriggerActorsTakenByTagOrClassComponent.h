#pragma once

#include "CoreMinimal.h"
#include "Components/BaseFlowComponent.h"
#include "FlowTriggerActorsTakenByTagOrClassComponent.generated.h"

class UFlowVariableBase;

UCLASS(ClassGroup=(Flow), meta=(BlueprintSpawnableComponent, FlowComponent))
class ACTORFLOWGRAPH_API UFlowTriggerActorsTakenByTagOrClassComponent : public UBaseFlowComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlowTriggerActorsTakenByTagOrClassComponent();

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow|TriggerActorsTakenByTagOrClass")
	FName ActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow|TriggerActorsTakenByTagOrClass")
	TSubclassOf<AActor> ActorClass;
public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void CallActorsFunctionByName(FName InFuncName, TMap<FName, UFlowVariableBase*> InVariablesMap);
};
