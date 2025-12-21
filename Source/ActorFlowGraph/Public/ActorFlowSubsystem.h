#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorFlowGraphRuntime.h"

#include "ActorFlowSubsystem.generated.h"

UCLASS()
class ACTORFLOWGRAPH_API UActorFlowSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<UActorFlowGraphRuntime*> GraphAssets;

	/*Actors SubPath - key, Nodes - value*/
	TMap<FString, TArray<FActorFlowNode*>> ActorNodeMap;
	/*Node guid - key, Nodes - value*/
	TMap<FGuid, FActorFlowNode*> IdNodeMap;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void InitFlowGraphs(TArray<UActorFlowGraphRuntime*> InGraphAssets);

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void Emit(UObject* Sender, FName PinName);
};
