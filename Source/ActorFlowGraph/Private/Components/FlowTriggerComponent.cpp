#include "Components/FlowTriggerComponent.h"
#include "Components/ShapeComponent.h"
#include "ActorFlowUtils.h" 

UFlowTriggerComponent::UFlowTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bDoTriggerOnExitOnlyIfEmpty = true;
	bDoTriggerOnlyOnce = false;
	bEnable = true;
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
	SetEnable(bEnable);
}

void UFlowTriggerComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bEnable || !IsFilteredActor(OtherActor))
	{
		return;
	}
	FLOW_EMIT("OnEnter");
}

void UFlowTriggerComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bEnable || !IsFilteredActor(OtherActor))
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
	bEnable = bInEnable;
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
