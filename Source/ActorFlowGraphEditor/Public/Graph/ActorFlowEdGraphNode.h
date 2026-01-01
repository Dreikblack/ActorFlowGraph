#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ActorFlowGraphRuntime.h"
#include "ActorFlowEdGraphNode.generated.h"

class SSearchBox;
class ITableRow;
template <typename ItemType> class SListView;

USTRUCT()
struct FGuidPair
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid A;

	UPROPERTY()
	FGuid B;

	bool operator==(const FGuidPair& Other) const
	{
		return A == Other.A && B == Other.B;
	}
};

FORCEINLINE uint32 GetTypeHash(const FGuidPair& Key)
{
	return HashCombine(GetTypeHash(Key.A), GetTypeHash(Key.B));
}

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

protected:
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	void CreateFunctionPinSubMenu(UToolMenu* Menu, bool bIsInput);
	void CreateFlowComponentSubMenu(UToolMenu* Menu);
	virtual void AddFlowComponent(UClass* InComponentClass);
public:
	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FSoftObjectPath Actor;

	//UPROPERTY()
	//TArray<FComponentReference> Components;

	UPROPERTY(EditAnywhere, Instanced)
	TMap<FGuidPair, UFlowConnectionVariables*> Connections;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;
	virtual bool IncludeParentNodeContextMenu() const override;
	virtual void AddFunctionPin(TStrongObjectPtr<UObject> InObject, FName FunctionName, bool bIsInput);
	virtual FLinearColor GetNodeBodyTintColor() const override;
};
