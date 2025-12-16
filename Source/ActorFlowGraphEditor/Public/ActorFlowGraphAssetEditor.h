#pragma once
#include "Toolkits/AssetEditorToolkit.h"
#include "ActorFlowGraphAsset.h"

class SActorFlowGraphEditor;
class IDetailsView;


class ACTORFLOWGRAPHEDITOR_API FActorFlowGraphAssetEditor : public FAssetEditorToolkit, public FEditorUndoClient, public FGCObject
{
public:
	FActorFlowGraphAssetEditor();
	virtual ~FActorFlowGraphAssetEditor() override;
	void InitActorFlowAssetEditor(
		const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UEdGraph* Graph,
		UActorFlowGraphAsset* GraphAsset
	);

	// Tabs
	static const FName GraphTabID;
	static const FName DetailsTabID;

	// IToolkit

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	// FGCObject

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	bool IsTabFocused(const FTabId& TabId) const;

	// FEditorUndoClient
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	virtual void HandleUndoTransaction();
protected:
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<class IDetailsView> DetailsView;

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

	TSharedRef<SDockTab> SpawnGraphTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);
private:
	FDelegateHandle GraphChangeListenerHandle;
	TSharedPtr<SActorFlowGraphEditor> GraphEditorWidget;
	void OnDropActors(const TArray< TWeakObjectPtr<AActor> >& Actors, UEdGraph* Graph, const FVector2f& DropLocation);
	void CreateNodeFromActor(UEdGraph* Graph, AActor* Actor, const FVector2f& Position);
public:
	UEdGraph* Graph = nullptr;
	UActorFlowGraphAsset* GraphAsset = nullptr;
	void SelectNode(AActor* Actor);
	void DeleteNodesByActor(AActor* InDeletedActor);
};
