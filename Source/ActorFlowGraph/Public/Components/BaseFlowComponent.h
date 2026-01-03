#pragma once

#include "CoreMinimal.h"
#include "BaseFlowComponent.generated.h"

UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent), Abstract)
class ACTORFLOWGRAPH_API UBaseFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseFlowComponent();
protected:
	UPROPERTY(EditAnywhere, Category = "Flow")
	bool bIsEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	bool bDoTriggerOnlyOnce;
public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	virtual void SetEnable(bool bInEnable = true);
};
