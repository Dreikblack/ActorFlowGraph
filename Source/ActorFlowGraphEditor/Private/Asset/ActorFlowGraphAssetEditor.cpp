#include "Asset/ActorFlowGraphAssetEditor.h"
#include "GraphEditorActions.h"
#include "Graph/ActorFlowGraphSchema.h"
#include "GraphEditor.h"
#include "Asset/ActorFlowGraphAsset.h"
#include "Graph/ActorFlowEdGraphNode.h"
#include "Widgets/SActorFlowGraphEditor.h"
#include "ActorFlowGraphRuntime.h"
#include "Graph/ActorFlowEdGraph.h"
#include "Widgets/SActorFlowPalette.h"


#define LOCTEXT_NAMESPACE "ActorFlowGraphAssetEditor"


const FName FActorFlowGraphAssetEditor::GraphTabID(TEXT("ActorFlowNode_GraphTab"));
const FName FActorFlowGraphAssetEditor::DetailsTabID(TEXT("ActorFlowNode_DetailsTab"));
const FName FActorFlowGraphAssetEditor::PaletteTabID(TEXT("ActorFlowNode_PaletteTab"));

FActorFlowGraphAssetEditor::FActorFlowGraphAssetEditor()
{
}

FActorFlowGraphAssetEditor::~FActorFlowGraphAssetEditor()
{
	GEditor->UnregisterForUndo(this);
}

FName FActorFlowGraphAssetEditor::GetToolkitFName() const
{
	return FName("ActorFlowGraphEditor");
}

FText FActorFlowGraphAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ActorFlowAsset Editor");
}

FString FActorFlowGraphAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "ActorFlowAsset").ToString();
}

FLinearColor FActorFlowGraphAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FActorFlowGraphAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	TObjectPtr<UEdGraph> ref(Graph);
	Collector.AddReferencedObject(ref);
}

FString FActorFlowGraphAssetEditor::GetReferencerName() const
{
	return TEXT("FActorFlowGraphAssetEditor");
}

bool FActorFlowGraphAssetEditor::IsTabFocused(const FTabId& TabId) const
{
	if (const TSharedPtr<SDockTab> CurrentGraphTab = GetToolkitHost()->GetTabManager()->FindExistingLiveTab(TabId))
	{
		return CurrentGraphTab->IsActive();
	}

	return false;
}

void FActorFlowGraphAssetEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		HandleUndoTransaction();
	}
}

void FActorFlowGraphAssetEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		HandleUndoTransaction();
	}
}

void FActorFlowGraphAssetEditor::HandleUndoTransaction()
{
	GraphEditorWidget->ClearSelectionSet();
	GraphEditorWidget->NotifyGraphChanged();
	FSlateApplication::Get().DismissAllMenus();
}

void FActorFlowGraphAssetEditor::InitActorFlowAssetEditor(
	const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UEdGraph* InGraph,
	UActorFlowGraphAsset* InGraphAsset
)
{
	Graph = InGraph;

	GraphAsset = InGraphAsset;
	GraphAsset->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	UActorFlowEdGraph* ActorGraph = Cast<UActorFlowEdGraph>(Graph);
	ActorGraph->SelectedConnection = FGuidPair();

	FPropertyEditorModule& PropModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch = true;
	Args.bShowSectionSelector = true;
	Args.bShowLooseProperties = true;
	Args.bAllowMultipleTopLevelObjects = true;
	Args.bLockable = true;
	Args.bShowObjectLabel = true;
	
	DetailsView = PropModule.CreateDetailView(Args);

	Palette = SNew(SActorFlowPalette).Graph(Graph);

	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ActorFlowEditor_Layout_v3")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)	
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.12f)
				->AddTab(PaletteTabID, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(1.0f)
				->AddTab(GraphTabID, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->AddTab(DetailsTabID, ETabState::OpenedTab)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		FName("ActorFlowEditor"),
		Layout,
		true,
		true,
		Graph
	);
}

void FActorFlowGraphAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(
		GraphTabID,
		FOnSpawnTab::CreateSP(this, &FActorFlowGraphAssetEditor::SpawnGraphTab)
	)
		.SetDisplayName(FText::FromString("Actor Flow Graph"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(DetailsTabID, FOnSpawnTab::CreateSP(this, &FActorFlowGraphAssetEditor::SpawnDetailsTab))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(PaletteTabID, FOnSpawnTab::CreateSP(this, &FActorFlowGraphAssetEditor::SpawnPaletteTab))
		.SetDisplayName(LOCTEXT("PaletteTab", "Palette"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Palette"));
	
}

void FActorFlowGraphAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(GraphTabID);
	InTabManager->UnregisterTabSpawner(DetailsTabID);
}

TSharedRef<SDockTab> FActorFlowGraphAssetEditor::SpawnGraphTab(const FSpawnTabArgs& Args)
{
	FGraphEditorCommands::Register();
	check(Graph);

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnDropActors = SGraphEditor::FOnDropActors::CreateSP(this, &FActorFlowGraphAssetEditor::OnDropActors);

	SAssignNew(GraphEditorWidget, SActorFlowGraphEditor, SharedThis(this))
		.GraphEvents(GraphEvents)
		.DetailsView(DetailsView);

	return SNew(SDockTab)
		.Label(FText::FromString("Actor Flow Graph"))
		[
			GraphEditorWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FActorFlowGraphAssetEditor::SpawnDetailsTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabID);

	return SNew(SDockTab)
		.Label(LOCTEXT("ActorFlowDetailsTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FActorFlowGraphAssetEditor::SpawnPaletteTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PaletteTabID);

	return SNew(SDockTab)
		.Label(LOCTEXT("ActorFlowPaletteTitle", "Palette"))
		[
			Palette.ToSharedRef()
		];
}

void FActorFlowGraphAssetEditor::OnDropActors(const TArray< TWeakObjectPtr<AActor> >& Actors, UEdGraph* InGraph, const FVector2f& DropLocation)
{
	if (UEdGraph* EdGraph = InGraph)
	{
		const FScopedTransaction Transaction(FText::FromString("Drop Actor on Actor Flow Graph"));

		InGraph->Modify();

		FVector2f CurrentLocation = DropLocation;
		for (int32 i = 0; i < Actors.Num(); i++)
		{
			AActor* DroppedActor = Actors[i].Get();
			if (GraphAsset->LevelName == NAME_None)
			{
				GraphAsset->LevelName = DroppedActor->GetLevel()->GetOuter()->GetFName();
			}
			UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(InGraph->Schema));
			if (Schema)
			{
				CreateNodeFromActor(EdGraph, DroppedActor, CurrentLocation);
				CurrentLocation.Y += 50.f;
			}
		}

	}
}

UActorFlowEdGraphNode* FActorFlowGraphAssetEditor::CreateNodeFromActor(UEdGraph* InGraph, AActor* Actor, const FVector2f& Position)
{
	if (!InGraph || !Actor)
	{
		return nullptr;
	}
	//don't add node, if already have one for this actor in this graph
	for (UEdGraphNode* EdNode : Graph->Nodes)
	{
		UActorFlowEdGraphNode* FoundNode = Cast<UActorFlowEdGraphNode>(EdNode);
		if (FoundNode && FoundNode->Actor == FSoftObjectPath(Actor))
		{
			return nullptr;
		}
	}

	const FScopedTransaction Transaction(FText::FromString("Add Actor Flow Node"));
	InGraph->Modify();
	GraphAsset->Modify();

	UActorFlowEdGraphNode* NewNode = NewObject<UActorFlowEdGraphNode>(InGraph, NAME_None, RF_Transactional);

	NewNode->CreateNewGuid();
	NewNode->NodePosX = Position.X;
	NewNode->NodePosY = Position.Y;

	NewNode->ActorName = *Actor->GetActorLabel();
	NewNode->Actor = FSoftObjectPath(Actor);

	UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(InGraph->Schema));

	Schema->CreatePins(Actor->GetClass(), Actor->GetFName(), NewNode);

	for (UActorComponent* Component : Actor->GetComponents())
	{
		Schema->CreatePins(Component->GetClass(), Component->GetFName(), NewNode);
	}

	InGraph->AddNode(NewNode, true);
	return NewNode;
}

void FActorFlowGraphAssetEditor::SelectNode(AActor* Actor)
{
	if (!GraphEditorWidget.IsValid() || !Graph)
	{
		return;
	}
	GraphEditorWidget->SelectNodeByActor(Actor);
	
}

void FActorFlowGraphAssetEditor::DeleteNodesByActor(AActor* InDeletedActor)
{
	if (!Graph)
	{
		return;
	}
	TArray<UEdGraphNode*> NodesToRemove;
	for (UEdGraphNode* Node : Graph->Nodes)
	{
		if (UActorFlowEdGraphNode* FlowNode = Cast<UActorFlowEdGraphNode>(Node))
		{
			if (!FlowNode->Actor.IsValid())
			{
				NodesToRemove.Add(FlowNode);
				continue;
			}
			UObject* Resolved = FlowNode->Actor.ResolveObject();
			if (!Resolved)
			{
				NodesToRemove.Add(FlowNode);
				continue;
			}
			if (Resolved == InDeletedActor)
			{
				NodesToRemove.Add(FlowNode);
				break;
			}
		}
	}
	if (!NodesToRemove.IsEmpty())
	{
		const FScopedTransaction Transaction(FText::FromString("Delete Node from Graph by Deleted Actor"));

		Graph->Modify();
		for (UEdGraphNode* Node : NodesToRemove)
		{
			Node->Modify();
			Graph->RemoveNode(Node);
		}
		Graph->NotifyGraphChanged();
	}
}

#undef LOCTEXT_NAMESPACE
