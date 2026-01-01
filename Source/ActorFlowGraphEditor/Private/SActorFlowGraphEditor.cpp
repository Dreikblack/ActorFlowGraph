#include "SActorFlowGraphEditor.h"
#include "ActorFlowGraphAsset.h"
#include "ActorFlowGraphAssetEditor.h"
#include "Framework/Commands/GenericCommands.h"
#include "ActorFlowEdGraphNode.h"
#include "ActorFlowEdGraph.h"
#include "ActorFlowGraphSchema.h"
#include "SGraphPanel.h"
#include "EdGraphNode_Comment.h"

#define LOCTEXT_NAMESPACE "SActorFlowGraphEditor"

void SActorFlowGraphEditor::Construct(const FArguments& InArgs, const TSharedPtr<FActorFlowGraphAssetEditor> InAssetEditor)
{
	FlowAssetEditor = InAssetEditor;
	FlowAsset = FlowAssetEditor.Pin()->GraphAsset;
	DetailsView = InArgs._DetailsView;

	BindGraphCommands();

	SGraphEditor::FArguments Arguments;
	Arguments._AdditionalCommands = CommandList;
	FString CornerText = "Actor Flow Graph";
	if (FlowAsset->LevelName != NAME_None)
	{
		CornerText = CornerText + " (" + FlowAsset->LevelName.ToString() + ")";
	}
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString(CornerText);
	Arguments._Appearance = AppearanceInfo;
	Arguments._GraphToEdit = FlowAsset->EdGraph;
	Arguments._GraphEvents = InArgs._GraphEvents;
	Arguments._AutoExpandActionMenu = true;
	Arguments._GraphEvents.OnSelectionChanged = FOnSelectionChanged::CreateSP(this, &SActorFlowGraphEditor::OnSelectedNodesChanged);
	Arguments._GraphEvents.OnMouseButtonDown = FOnMouseButtonDown::CreateSP(this, &SActorFlowGraphEditor::OnMouseMouseButtonDown);
	Arguments._GraphEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &SActorFlowGraphEditor::OnNodeTitleCommitted);

	SGraphEditor::Construct(Arguments);
}

bool SActorFlowGraphEditor::CanEdit()
{
	return GEditor->PlayWorld == nullptr;
}

bool SActorFlowGraphEditor::IsTabFocused() const
{
	return FlowAssetEditor.Pin()->IsTabFocused(FActorFlowGraphAssetEditor::GraphTabID);
}

void SActorFlowGraphEditor::BindGraphCommands()
{
	const FGenericCommands& GenericCommands = FGenericCommands::Get();

	CommandList = MakeShareable(new FUICommandList);
	CommandList->MapAction(GenericCommands.Delete,
		FExecuteAction::CreateSP(this, &SActorFlowGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &SActorFlowGraphEditor::CanDeleteNodes));
	CommandList->MapAction(GenericCommands.Undo,
		FExecuteAction::CreateStatic(&SActorFlowGraphEditor::UndoGraphAction),
		FCanExecuteAction::CreateStatic(&SActorFlowGraphEditor::CanEdit));

	CommandList->MapAction(GenericCommands.Redo,
		FExecuteAction::CreateStatic(&SActorFlowGraphEditor::RedoGraphAction),
		FCanExecuteAction::CreateStatic(&SActorFlowGraphEditor::CanEdit));
}

void SActorFlowGraphEditor::UndoGraphAction()
{
	GEditor->UndoTransaction();
}

void SActorFlowGraphEditor::RedoGraphAction()
{
	GEditor->RedoTransaction();
}

void SActorFlowGraphEditor::DeleteSelectedNodes()
{
	const FScopedTransaction Transaction(LOCTEXT("DeleteSelectedNode", "Delete Selected Node"));
	GetCurrentGraph()->Modify();
	FlowAsset->Modify();

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = CastChecked<UEdGraphNode>(*NodeIt);
		if (Node->CanUserDeleteNode())
		{
			Node->Modify();
			UEdGraph* Graph = GetCurrentGraph();
			GetCurrentGraph()->GetSchema()->BreakNodeLinks(*Node);
			Node->DestroyNode();
		}
	}
}

bool SActorFlowGraphEditor::CanDeleteNodes() const
{
	if (CanEdit() && IsTabFocused())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			if (const UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
			{
				if (Node->CanUserDeleteNode())
				{
					return true;
				}
			}
		}
	}

	return false;
}

void SActorFlowGraphEditor::OnSelectedNodesChanged(const TSet<UObject*>& Nodes)
{
	if (bSelectionSyncInProgress)
	{
		return;
	}
	TArray<UObject*> ObjectsToSelect;

	TGuardValue<bool> Guard(bSelectionSyncInProgress, true);
	for (UObject* Object : Nodes)
	{
		if (const UActorFlowEdGraphNode* Node = Cast<UActorFlowEdGraphNode>(Object))
		{
			if (AActor* Actor = Cast<AActor>(Node->Actor.ResolveObject()))
			{
				ObjectsToSelect.Add(Actor);
				if (GEditor && !Actor->IsSelectedInEditor())
				{
					GEditor->SelectNone(false, true);
					GEditor->SelectActor(Actor, true, true, true);
				}
			}
		}
		else if (UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Object))
		{
			ObjectsToSelect.Add(CommentNode);
		}
	}
	if (!ObjectsToSelect.IsEmpty())
	{
		DetailsView->SetObjects(ObjectsToSelect);
	}
}

FReply SActorFlowGraphEditor::OnMouseMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	UActorFlowEdGraph* ActorGraph = Cast<UActorFlowEdGraph>(FlowAssetEditor.Pin()->Graph);
	bool bWasConnectionClicked = false;
	if (ActorGraph->SplineOverlapResult.GetCloseToSpline())
	{
		UEdGraphPin* OutPin1;
		UEdGraphPin* OutPin2;
		if (ActorGraph->SplineOverlapResult.GetPins(*GetGraphPanel(), OutPin1, OutPin2))
		{
			bWasConnectionClicked = true;
			UActorFlowEdGraphNode* ActorFlowGraphNodeA = Cast<UActorFlowEdGraphNode>(OutPin1->GetOwningNode());
			FGuidPair Pair(OutPin1->PinId, OutPin2->PinId);
			ActorGraph->SelectedConnection = Pair;
			DetailsView->SetObject(ActorFlowGraphNodeA->Connections.FindRef(Pair));
		}
	}
	if (!bWasConnectionClicked)
	{
		DetailsView->SetObject(nullptr);
		ActorGraph->SelectedConnection = FGuidPair();
	}
	return FReply::Unhandled();
}

void SActorFlowGraphEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void SActorFlowGraphEditor::SelectNodeByActor(AActor* Actor)
{
	if (bSelectionSyncInProgress)
	{
		return;
	}
	TGuardValue<bool> Guard(bSelectionSyncInProgress, true);
	for (UEdGraphNode* Node : GetCurrentGraph()->Nodes)
	{
		auto ActorFlowNode = Cast<UActorFlowEdGraphNode>(Node);
		if (!ActorFlowNode)
		{
			continue;
		}

		if (ActorFlowNode->Actor == FSoftObjectPath(Actor) && !GetSelectedNodes().Contains(Node))
		{
			ClearSelectionSet();
			SetNodeSelection(Node, true);
			ZoomToFit(true);
			return;
		}
	}
}

#undef LOCTEXT_NAMESPACE
