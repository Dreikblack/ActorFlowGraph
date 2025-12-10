#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"

class IAssetTypeActions;

class ACTORFLOWGRAPHEDITOR_API FActorFlowGraphEditorModule : public FDefaultModuleImpl
{
private:
	void OnEditorSelectionChanged(UObject* NewSelection);
public:
	static EAssetTypeCategories::Type ActorFlowGraphAssetCategory;
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
