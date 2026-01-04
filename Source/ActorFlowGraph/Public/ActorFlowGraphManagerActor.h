#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ActorFlowGraphManagerActor.generated.h"

class UActorFlowGraphRuntime;

UCLASS(ClassGroup = (Flow), Blueprintable, BlueprintType)
class ACTORFLOWGRAPH_API AActorFlowGraphManagerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorFlowGraphManagerActor();
	/** Actor Flow Graph Assets to use at this map */
	UPROPERTY(EditAnywhere, Category = "Flow")
	TArray<UActorFlowGraphRuntime*> GraphAssets;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
