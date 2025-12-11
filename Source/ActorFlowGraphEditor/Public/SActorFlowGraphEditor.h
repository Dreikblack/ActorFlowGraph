#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"


class UActorFlowGraphAsset;
class FActorFlowGraphAssetEditor;

class ACTORFLOWGRAPHEDITOR_API SActorFlowGraphEditor : public SGraphEditor
{
public:
	SLATE_BEGIN_ARGS(SActorFlowGraphEditor)
		{
		}

		SLATE_ARGUMENT(FGraphEditorEvents, GraphEvents)
		SLATE_ARGUMENT(TSharedPtr<IDetailsView>, DetailsView)
	SLATE_END_ARGS()
protected:
	TWeakObjectPtr<UActorFlowGraphAsset> FlowAsset;
	TWeakPtr<FActorFlowGraphAssetEditor> FlowAssetEditor;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<FUICommandList> CommandList;
	bool bSelectionSyncInProgress = false;
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FActorFlowGraphAssetEditor> InAssetEditor);
private:
	static void UndoGraphAction();
	static void RedoGraphAction();
protected:
	virtual void DeleteSelectedNodes();
	virtual bool CanDeleteNodes() const;
	virtual void OnSelectedNodesChanged(const TSet<UObject*>& Nodes);
	virtual FReply OnMouseMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	
public:
	static bool CanEdit();
	virtual bool IsTabFocused() const;

	virtual void BindGraphCommands();

	void SelectNodeByActor(AActor* Actor);
};
