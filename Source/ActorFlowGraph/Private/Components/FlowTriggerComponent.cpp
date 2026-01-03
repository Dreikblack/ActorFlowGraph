#include "Components/FlowTriggerComponent.h"
#include "Components/ShapeComponent.h"
#include "ActorFlowUtils.h" 

UFlowTriggerComponent::UFlowTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bDoTriggerOnExitOnlyIfEmpty = true;
	bDoTriggerOnlyOnce = false;
	bIsEnabled = true;
}


void UFlowTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
	CollisionShape = GetOwner()->GetComponentByClass<UShapeComponent>();
	if (CollisionShape)
	{
		CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &UFlowTriggerComponent::OnOverlapBegin);
		CollisionShape->OnComponentEndOverlap.AddDynamic(this, &UFlowTriggerComponent::OnOverlapEnd);
	}
	SetEnable(bIsEnabled);
}

void UFlowTriggerComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsEnabled || !IsFilteredActor(OtherActor))
	{
		return;
	}
	FLOW_EMIT("OnEnter");
}

void UFlowTriggerComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsEnabled || !IsFilteredActor(OtherActor))
	{
		return;
	}
	if (bDoTriggerOnExitOnlyIfEmpty)
	{
		TArray<AActor*> CurrentOverlappingActors;
		CollisionShape->GetOverlappingActors(CurrentOverlappingActors);
		for (AActor* Actor : CurrentOverlappingActors)
		{
			if (IsFilteredActor(Actor))
			{
				return;
			}
		}
	}	

	FLOW_EMIT("OnExit");
	if (bDoTriggerOnlyOnce)
	{
		SetEnable(false);
	}
}

bool UFlowTriggerComponent::IsFilteredActor(AActor* Actor)
{
	bool bIsFilteredActor = Actor != nullptr && Actor != GetOwner();
	if (bIsFilteredActor && !FilterTags.IsEmpty())
	{
		bIsFilteredActor = false;
		for (const FName& FilterTag : FilterTags)
		{
			if (Actor->ActorHasTag(FilterTag))
			{
				bIsFilteredActor = true;
				break;
			}
		}
	}
	if (bIsFilteredActor && !FilterClasses.IsEmpty())
	{
		bIsFilteredActor = false;
		for (const TSubclassOf<AActor>& FilterClass : FilterClasses)
		{
			if (Actor->IsA(FilterClass))
			{
				bIsFilteredActor = true;
				break;
			}
		}
	}
	return bIsFilteredActor;
}

void UFlowTriggerComponent::SetEnable(bool bInEnable)
{
	bIsEnabled = bInEnable;
	if (!CollisionShape)
	{
		return;
	}
	if (!bInEnable)
	{
		SavedCollision.CollisionEnabled = CollisionShape->GetCollisionEnabled();
		SavedCollision.ObjectType = CollisionShape->GetCollisionObjectType();
		SavedCollision.Responses = CollisionShape->GetCollisionResponseToChannels();
		CollisionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (!CollisionShape->IsCollisionEnabled())
	{
		CollisionShape->SetCollisionEnabled(SavedCollision.CollisionEnabled);
		CollisionShape->SetCollisionObjectType(SavedCollision.ObjectType);
		CollisionShape->SetCollisionResponseToChannels(SavedCollision.Responses);
	}
}
