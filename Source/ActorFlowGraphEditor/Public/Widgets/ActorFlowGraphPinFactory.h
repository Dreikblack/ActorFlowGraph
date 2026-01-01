#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class ACTORFLOWGRAPHEDITOR_API FActorFlowGraphPinFactory : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};