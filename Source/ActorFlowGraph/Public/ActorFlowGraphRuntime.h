#pragma once

#include "CoreMinimal.h"
#include "FlowVariableBase.h"

#include "ActorFlowGraphRuntime.generated.h"

/*Actor Flow Connection*/
UCLASS(EditInlineNew)
class UFlowConnectionVariables : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	FString ConnectionName;
#endif 

	UPROPERTY(EditAnywhere, Instanced)
	TMap<FName, UFlowVariableBase*> VariablesMap;
};

USTRUCT()
struct FConnectionFlowNode
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid OwnerNodeId;

	/*Component/Actor FName*/
	UPROPERTY()
	FName OwnerName;

	UPROPERTY()
	FName OwnerPinName;

	UPROPERTY()
	FGuid TargetNodeId;

	/*Component/Actor FName*/
	UPROPERTY()
	FName TargetOwnerName;

	UPROPERTY()
	FName TargetPinName;

	UPROPERTY(EditAnywhere, Instanced, Category = "Input Variables")
	UFlowConnectionVariables* Variables;
};

USTRUCT()
struct FFlowGraphPin
{
	GENERATED_BODY()


	/*Actor or Component*/
	UPROPERTY()
	FName OwnerName;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	TArray<FConnectionFlowNode> OutputConnections;
};

USTRUCT()
struct FActorFlowNode
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid NodeGuid;

	/** X position of node in the editor */
	UPROPERTY()
	int32 NodePosX;

	/** Y position of node in the editor */
	UPROPERTY()
	int32 NodePosY;

	/** Actor name */
	UPROPERTY()
	FName NodeName;

	UPROPERTY()
	FSoftObjectPath Actor;

	UPROPERTY()
	TArray<FComponentReference> Components;

	UPROPERTY()
	TArray<FFlowGraphPin> Inputs;

	UPROPERTY()
	TArray<FFlowGraphPin> Outputs;
};

/**
 *
 */
UCLASS(BlueprintType)
class ACTORFLOWGRAPH_API UActorFlowGraphRuntime : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FActorFlowNode> Nodes;
};
