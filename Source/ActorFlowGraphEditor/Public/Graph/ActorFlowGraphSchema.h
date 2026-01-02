#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"

#include "ActorFlowGraphSchema.generated.h"

class UActorFlowEdGraphNode;
class UFlowConnectionVariables;

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowGraphSchema : public UEdGraphSchema {
    GENERATED_BODY()
protected:
   
public:
    virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

    virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

    virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
    virtual UFlowConnectionVariables* GetVariablesByFunction(UActorFlowEdGraphNode* ActorFlowGraphNode, UFunction* InFunction, UEdGraphPin* PinA, UEdGraphPin* PinB) const;
    virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
    virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

    void CreatePins(UClass* InCls, FName InOwnerName, UActorFlowEdGraphNode* Node, bool bDoCheckIfExist = false);
    void CreatePin(UActorFlowEdGraphNode* Node, FName ClassName, FName PinName, FName OwnerName, bool bIsInput);

    virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
   
    virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
    
    virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

    virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
};
