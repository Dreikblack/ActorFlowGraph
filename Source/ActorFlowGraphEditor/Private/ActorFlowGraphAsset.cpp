#include "ActorFlowGraphAsset.h"
#include "ActorFlowEdGraphNode.h"
#include "UObject/ObjectSaveContext.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "ActorFlowEdGraph.h"

void UActorFlowGraphAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	UpdateAssetFromGraph();
}

void UActorFlowGraphAsset::UpdateAssetFromGraph()
{
	if (EdGraph == nullptr)
	{
		return;
	}
	//Create runtime asset
	const FString EditorPackage = GetOutermost()->GetName();
	const FString RuntimePackageName = EditorPackage + TEXT("_Runtime");

	UPackage* RuntimePackage = FindPackage(nullptr, *RuntimePackageName);
	if (!RuntimePackage)
		RuntimePackage = CreatePackage(*RuntimePackageName);

	const FString RuntimeObjectName = FPackageName::GetLongPackageAssetName(RuntimePackageName);

	if (!ActorFlowGraphRuntime)
	{
		ActorFlowGraphRuntime = FindObject<UActorFlowGraphRuntime>(RuntimePackage, *RuntimeObjectName);

		if (!ActorFlowGraphRuntime)
		{
			ActorFlowGraphRuntime = NewObject<UActorFlowGraphRuntime>(
				RuntimePackage,
				UActorFlowGraphRuntime::StaticClass(),
				*RuntimeObjectName,
				RF_Public | RF_Standalone
			);
		}

		FAssetRegistryModule::AssetCreated(ActorFlowGraphRuntime);
	}

	ActorFlowGraphRuntime->Nodes.Empty();
	//Convert Graph into Runtime data
	for (UEdGraphNode* EdNode : EdGraph->Nodes)
	{
		UActorFlowEdGraphNode* FlowNode = Cast<UActorFlowEdGraphNode>(EdNode);
		if (!FlowNode)
		{
			continue;
		}

		FActorFlowNode AssetNode;
		AssetNode.NodeGuid = FlowNode->NodeGuid;
		AssetNode.NodeName = FlowNode->ActorName;
		AssetNode.NodePosX = FlowNode->NodePosX;
		AssetNode.NodePosY = FlowNode->NodePosY;
		AssetNode.Actor = FlowNode->Actor;

		for (UEdGraphPin* Pin : FlowNode->Pins)
		{
			FFlowGraphPin PinToSave;
			PinToSave.OwnerName = Pin->GetPrimaryTerminalType().TerminalSubCategory;
			PinToSave.PinName = Pin->PinName;

			if (Pin->Direction == EGPD_Output)
			{
				for (UEdGraphPin* TargetPin : Pin->LinkedTo)
				{
					if (TargetPin->GetOwningNode())
					{
						FConnectionFlowNode Connection;
						Connection.OwnerPinName = Pin->GetFName();
						Connection.OwnerName = Pin->GetPrimaryTerminalType().TerminalSubCategory;
						Connection.OwnerNodeId = Pin->GetOwningNode()->NodeGuid;

						Connection.TargetPinName = TargetPin->GetFName();
						Connection.TargetOwnerName = TargetPin->GetPrimaryTerminalType().TerminalSubCategory;
						Connection.TargetNodeId = TargetPin->GetOwningNode()->NodeGuid;

						FGuidPair Pair(Pin->PinId, TargetPin->PinId);
						UFlowConnectionVariables* NodeVariables = FlowNode->Connections.FindRef(Pair);
						if (NodeVariables)
						{
							UFlowConnectionVariables* AssetVariables = NewObject<UFlowConnectionVariables>(ActorFlowGraphRuntime, UFlowConnectionVariables::StaticClass());
							for (auto& MapPair : NodeVariables->VariablesMap)
							{
								UFlowVariableBase* NewVar = DuplicateObject(MapPair.Value, AssetVariables);
								AssetVariables->VariablesMap.Add(MapPair.Key, NewVar);
							}
							AssetVariables->VariablesMap;
							Connection.Variables = AssetVariables;
						}

						PinToSave.OutputConnections.Add(Connection);
					}
				}
				AssetNode.Outputs.Add(PinToSave);
			}
			else if (Pin->Direction == EGPD_Input)
			{
				AssetNode.Inputs.Add(PinToSave);
			}
		}

		ActorFlowGraphRuntime->Nodes.Add(AssetNode);
		ActorFlowGraphRuntime->LevelName = LevelName;
	}

	//Save runtime asset
	const FString Filename = FPackageName::LongPackageNameToFilename(RuntimePackageName, FPackageName::GetAssetPackageExtension());

	RuntimePackage->MarkPackageDirty();
	ActorFlowGraphRuntime->MarkPackageDirty();

	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Public | RF_Standalone;
	Args.SaveFlags = SAVE_NoError;

	UPackage::SavePackage(RuntimePackage, ActorFlowGraphRuntime, *Filename, Args);

}
