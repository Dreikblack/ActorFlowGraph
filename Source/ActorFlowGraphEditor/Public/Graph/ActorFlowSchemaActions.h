#pragma once

#include "CoreMinimal.h"
#include "ActorFlowSchemaActions.generated.h"

USTRUCT()
struct ACTORFLOWGRAPHEDITOR_API FActorFlowSchemaAction_NewNode : public FEdGraphSchemaAction
{
    GENERATED_USTRUCT_BODY()

    FActorFlowSchemaAction_NewNode()
    {
    }

    UClass* ComponentClass = nullptr;

    FActorFlowSchemaAction_NewNode(
        const FText& InCategory,
        const FText& InMenuDesc,
        const FText& InTooltip,
        int32 InGrouping,
        UClass* InComponentClass
    )
        : FEdGraphSchemaAction(
            InCategory,
            InMenuDesc,
            InTooltip,
            InGrouping
        ), 
        ComponentClass(InComponentClass)
    {
    }

    virtual UEdGraphNode* PerformAction(
        UEdGraph* ParentGraph,
        UEdGraphPin* FromPin,
        const FVector2D Location,
        bool bSelectNewNode = true
    ) override;  
};

USTRUCT()
struct ACTORFLOWGRAPHEDITOR_API FActorFlowSchemaAction_NewComment : public FEdGraphSchemaAction
{
    GENERATED_USTRUCT_BODY()

    FActorFlowSchemaAction_NewComment()
    {
    }


    FActorFlowSchemaAction_NewComment(
        const FText& InCategory,
        const FText& InMenuDesc,
        const FText& InTooltip,
        int32 InGrouping
    )
        : FEdGraphSchemaAction(
            InCategory,
            InMenuDesc,
            InTooltip,
            InGrouping
        )
    {
    }

    virtual UEdGraphNode* PerformAction(
        UEdGraph* ParentGraph,
        UEdGraphPin* FromPin,
        const FVector2D Location,
        bool bSelectNewNode = true
    ) override;
};