#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlowTriggerComponent.generated.h"

class UShapeComponent;

struct FCollisionProperties
{
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;
	TEnumAsByte<ECollisionChannel> ObjectType;
	FCollisionResponseContainer Responses;
};

UCLASS(ClassGroup = (Flow), meta = (BlueprintSpawnableComponent, FlowOutputs = "OnEnter, OnExit"))
class ACTORFLOWGRAPH_API UFlowTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlowTriggerComponent();

protected:
	//To filter which actor will trigger output by tag
	UPROPERTY(EditAnywhere, Category = "Flow | Trigger")
	TSet<FName> FilterTags;

	//To filter which actor will trigger output by class
	UPROPERTY(EditAnywhere, Category = "Flow | Trigger")
	TSet<TSubclassOf<AActor>> FilterClasses;

	UShapeComponent* CollisionShape;

	FCollisionProperties SavedCollision;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Trigger")
	bool bDoTriggerOnExitOnlyIfEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow | Trigger")
	bool bDoTriggerOnlyOnce;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Can be overrided to set more complex filter
	bool IsFilteredActor(AActor* Actor);

	UPROPERTY(EditAnywhere, Category = "Flow | Trigger")
	bool bEnable;

public:
	UFUNCTION(BlueprintCallable, meta = (FlowInput))
	void SetEnable(bool bInEnable = true);
};
