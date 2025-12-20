#include "SActorFlowPalette.h"


void SActorFlowPalette::Construct(const FArguments& InArgs)
{
    GraphObj = InArgs._Graph;
    SGraphPalette::Construct(
        SGraphPalette::FArguments()
    );
}

SActorFlowPalette::~SActorFlowPalette()
{
}

void SActorFlowPalette::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
    if (!GraphObj)
    {
        return;
    }

    const UEdGraphSchema* Schema = GraphObj->GetSchema();
    if (Schema)
    {
        FGraphContextMenuBuilder ActionMenuBuilder(GraphObj);
        Schema->GetGraphContextActions(ActionMenuBuilder);
        OutAllActions.Append(ActionMenuBuilder);
    }
}
