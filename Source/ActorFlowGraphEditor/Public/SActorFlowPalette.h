#pragma once

#include "SGraphPalette.h"


/**
 * 
 */
class ACTORFLOWGRAPHEDITOR_API SActorFlowPalette : public SGraphPalette
{
public:
	SLATE_BEGIN_ARGS(SActorFlowPalette) {}
		SLATE_ARGUMENT(UEdGraph*, Graph)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SActorFlowPalette() override;
protected:
	UEdGraph* GraphObj;
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;
};
