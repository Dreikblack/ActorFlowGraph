#include "ActorFlowSchemaActions.h"
#include "ActorFlowEdGraph.h"
#include "ActorFlowGraphAssetEditor.h"
#include "Toolkits/ToolkitManager.h"

UEdGraphNode* FActorFlowSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(ParentGraph);
	if (FoundAssetEditor.IsValid())
	{
		TSharedPtr<FActorFlowGraphAssetEditor> AssetEditor = StaticCastSharedPtr<FActorFlowGraphAssetEditor>(FoundAssetEditor);
		UWorld* World = GEditor->GetEditorWorldContext().World();
		check(World);

		const FScopedTransaction Transaction(FText::FromString("Add Actor with Flow Component"));
		World->Modify();

		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transactional;
		AActor* Actor = World->SpawnActor<AActor>(
			AActor::StaticClass(),
			FTransform::Identity,
			Params
		);
		Actor->Modify();

		FString Name = ComponentClass->GetName();
		Name.RemoveFromStart(TEXT("U"));
		Name.RemoveFromStart(TEXT("BP_"));
		Name.RemoveFromEnd(TEXT("Component"));

		FName UniqueName = MakeUniqueObjectName(
			World,
			Actor->GetClass(),
			FName(*Name)
		);
		Actor->SetActorLabel(UniqueName.ToString());
		Actor->SetFolderPath(FName("Nodes"));

		UActorComponent* Component = NewObject<UActorComponent>(
			Actor,                  
			ComponentClass,
			UniqueName,
			RF_Transactional | RF_Public
		);

		Component->Modify();
		Component->bAutoActivate = true;        
		Component->RegisterComponent();         
		Actor->AddInstanceComponent(Component); 	

		return AssetEditor->CreateNodeFromActor(ParentGraph, Actor, FVector2f(Location.X, Location.Y));
	}
	return NULL;
}
