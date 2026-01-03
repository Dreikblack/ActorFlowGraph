#include "Components/FlowTriggerActorsTakenByTagOrClassComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ActorFlowUtils.h"

UFlowTriggerActorsTakenByTagOrClassComponent::UFlowTriggerActorsTakenByTagOrClassComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlowTriggerActorsTakenByTagOrClassComponent::CallActorsFunctionByName(FName InFuncName, TMap<FName, UFlowVariableBase*> InVariablesMap)
{
	if (!bIsEnabled || InFuncName.IsNone() || ActorTag.IsNone() && ActorClass == nullptr)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	if (ActorClass == nullptr)
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), ActorTag, FoundActors);
	}
	else if (ActorTag.IsNone())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, FoundActors);
	}
	else
	{
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ActorClass, ActorTag, FoundActors);
	}
	for (AActor* Actor : FoundActors)
	{
		CallFunctionByName(Actor, InFuncName, InVariablesMap);
	}
	if (bDoTriggerOnlyOnce)
	{
		SetEnable(false);
	}
}


