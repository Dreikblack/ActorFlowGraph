#include "ActorFlowSubsystem.h"
#include "Engine/World.h"
#include "ActorFlowGraphRuntime.h"
#include "ActorFlowUtils.h"

void UActorFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// nothing
}


void UActorFlowSubsystem::Deinitialize()
{
	// nothing
}

void UActorFlowSubsystem::InitFlowGraphs(TArray<UActorFlowGraphRuntime*> InGraphAssets)
{
	GraphAssets = InGraphAssets;
	ActorNodeMap.Empty();
	//fill map to be able quickly get all related to actor nodes
	for (UActorFlowGraphRuntime* Asset : GraphAssets)
	{
		if (!Asset)
		{
			continue;
		}

		for (FActorFlowNode& Node : Asset->Nodes)
		{
			IdNodeMap.Add(Node.NodeGuid, &Node);
			const FSoftObjectPath ActorPath = Node.Actor;

			if (!ActorPath.IsValid())
			{
				continue;
			}
			ActorNodeMap.FindOrAdd(ActorPath.GetSubPathString()).Add(&Node);
		}
	}
}

void UActorFlowSubsystem::Emit(UObject* Sender, FName PinName)
{
	AActor* Actor = Cast<AActor>(Sender);
	UActorComponent* Component = Cast<UActorComponent>(Sender);

	if (Actor || Component && Component->GetOwner())
	{
		FSoftObjectPath Path;
		FName SenderName;
		if (Actor) 
		{
			Path = FSoftObjectPath(Actor);
			SenderName = Actor->GetFName();
		}
		else
		{
			Path = FSoftObjectPath(Component->GetOwner());
			SenderName = Component->GetFName();
		}
		TArray<FActorFlowNode*>* Nodes = ActorNodeMap.Find(Path.GetSubPathString());
		if (!Nodes)
		{
			return;
		}

		for (const FActorFlowNode* Node : *Nodes)
		{
			for (const FFlowGraphPin OutputPin : Node->Outputs)
			{
				if (OutputPin.PinName == PinName && OutputPin.OwnerName == SenderName)
				{
					for (const FConnectionFlowNode Connection : OutputPin.OutputConnections)
					{
						auto FoundTargetNode = IdNodeMap.Find(Connection.TargetNodeId);
						if (!FoundTargetNode)
						{
							continue;
						}
						AActor* InActor = Cast<AActor>((*FoundTargetNode)->Actor.TryLoad());
						if (InActor)
						{
							for (const FFlowGraphPin InputPin : (*FoundTargetNode)->Inputs)
							{
								if (InputPin.PinName == Connection.TargetPinName && InputPin.OwnerName == Connection.TargetOwnerName)
								{
									TMap<FName, UFlowVariableBase*> VariablesMap;
									if (Connection.Variables)
									{
										VariablesMap = Connection.Variables->VariablesMap;
									}
									TMap<FName, void*> Args;
									if (InputPin.OwnerName == InActor->GetFName())
									{
										CallFunctionByName(InActor, InputPin.PinName, VariablesMap);
										Emit(InActor, InputPin.PinName);
									}
									else
									{
										for (UActorComponent* CurrentComponent : InActor->GetComponents())
										{
											if (InputPin.OwnerName == CurrentComponent->GetFName())
											{
												CallFunctionByName(CurrentComponent, InputPin.PinName, VariablesMap);
												Emit(CurrentComponent, InputPin.PinName);
												break;
											}
										}
									}
								}
							}
						}
					}
					break;
				}
			}
		}
	}
}
