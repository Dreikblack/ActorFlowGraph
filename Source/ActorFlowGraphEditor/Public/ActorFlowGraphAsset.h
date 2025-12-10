#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActorFlowGraphRuntime.h"

#include "ActorFlowGraphAsset.generated.h"

class FObjectPreSaveContext;
class UActorFlowEdGraph;

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowGraphAsset : public UPrimaryDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    UActorFlowGraphRuntime* ActorFlowGraphRuntime;
#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere)
    UActorFlowEdGraph* EdGraph = nullptr;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif
    void UpdateAssetFromGraph();
};
