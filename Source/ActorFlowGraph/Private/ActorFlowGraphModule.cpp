#include "ActorFlowGraphModule.h"

#define LOCTEXT_NAMESPACE "FActorFlowGraphModule"

void FActorFlowGraphModule::StartupModule()
{
	//Empty
}

void FActorFlowGraphModule::ShutdownModule()
{
	//Empty
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorFlowGraphModule, ActorFlowGraph)