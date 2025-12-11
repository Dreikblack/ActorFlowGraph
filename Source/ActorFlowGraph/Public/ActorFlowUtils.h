#pragma once

#include "ActorFlowSubsystem.h"

inline void FlowEmit(UObject* Sender, FName Pin)
{
	if (!Sender)
	{
		return;
	}

	UWorld* World = Sender->GetWorld();
	if (!World)
	{
		return;
	}

	if (UActorFlowSubsystem* FlowSubsystem = World->GetSubsystem<UActorFlowSubsystem>())
	{
		FlowSubsystem->Emit(Sender, Pin);
	}
}

#define FLOW_EMIT(Pin) FlowEmit(this, FName(Pin))

static bool CallFunctionByName(UObject* Object, const FName FuncName, TMap<FName, UFlowVariableBase*> VariablesMap);
