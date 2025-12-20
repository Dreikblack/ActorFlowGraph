#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"

class IAssetTypeActions;

class ACTORFLOWGRAPHEDITOR_API FActorFlowGraphEditorModule : public FDefaultModuleImpl
{
private:
	TArray<UClass*> CachedFlowComponents;

	virtual void OnEditorSelectionChanged(UObject* NewSelection);
	virtual void OnActorDeleted(AActor* DeletedActor);
public:
	static EAssetTypeCategories::Type ActorFlowGraphAssetCategory;
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	const TArray<UClass*>& GetFlowComponentClasses() const
	{
		return CachedFlowComponents;
	}
	virtual void RebuildFlowComponentsCache();
};
