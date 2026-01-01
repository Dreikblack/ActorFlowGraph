#include "Asset/AssetTypeActions_ActorFlowGraph.h"
#include "Asset/ActorFlowGraphAsset.h"
#include "Graph/ActorFlowEdGraphNode.h"
#include "ActorFlowGraphEditorModule.h"
#include "Asset/ActorFlowGraphAssetEditor.h"
#include "Graph/ActorFlowGraphSchema.h"
#include "Graph/ActorFlowEdGraph.h"

FText FAssetTypeActions_ActorFlowGraph::GetName() const
{
	return FText::FromString("Actor Flow Graph");
}

FColor FAssetTypeActions_ActorFlowGraph::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FAssetTypeActions_ActorFlowGraph::GetSupportedClass() const
{
	return UActorFlowGraphAsset::StaticClass();
}

static UActorFlowEdGraphNode* FindNodeByGUID(UEdGraph* Graph, const FGuid& Guid)
{
	if (!Graph)
	{
		return nullptr;
	}
	for (UEdGraphNode* GNode : Graph->Nodes)
	{
		if (!GNode)
		{
			continue;
		}
		UActorFlowEdGraphNode* Node = Cast<UActorFlowEdGraphNode>(GNode);
		if (!Node)
		{
			continue;
		}
		if (Node->NodeGuid == Guid)
		{
			return Node;
		}
	}
	return nullptr;
}

void FAssetTypeActions_ActorFlowGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	//Convert Asset to Graph
	for (auto Obj : InObjects)
	{
		auto GraphAsset = Cast<UActorFlowGraphAsset>(Obj);
		if (GraphAsset)
		{
			//Open existing
			if (GraphAsset->EdGraph)
			{
				IAssetEditorInstance* ExistingEditor =
					GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()
					->FindEditorForAsset(GraphAsset->EdGraph, /*bFocusIfOpen*/ true);
				if (ExistingEditor)
				{
					continue;
				}
			}

			//Make new
			if (!GraphAsset->EdGraph)
			{
				GraphAsset->EdGraph = NewObject<UActorFlowEdGraph>(GraphAsset, UActorFlowEdGraph::StaticClass(), NAME_None, RF_Transactional);
				GraphAsset->EdGraph->Schema = UActorFlowGraphSchema::StaticClass();
			}
			
			TSharedRef<FActorFlowGraphAssetEditor> NewEditor = MakeShared<FActorFlowGraphAssetEditor>();
			NewEditor->InitActorFlowAssetEditor(EToolkitMode::Standalone, EditWithinLevelEditor, GraphAsset->EdGraph, GraphAsset);
		}
	}
}

uint32 FAssetTypeActions_ActorFlowGraph::FAssetTypeActions_ActorFlowGraph::GetCategories()
{
	return FActorFlowGraphEditorModule::ActorFlowGraphAssetCategory;
}

