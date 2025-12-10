#pragma once

#include "AssetTypeActions_Base.h"

class ACTORFLOWGRAPHEDITOR_API FAssetTypeActions_ActorFlowGraph : public FAssetTypeActions_Base {
public:
    virtual FText GetName() const override;

    virtual FColor GetTypeColor() const override;

    virtual UClass* GetSupportedClass() const override;

    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;

	virtual uint32 GetCategories() override;
};
