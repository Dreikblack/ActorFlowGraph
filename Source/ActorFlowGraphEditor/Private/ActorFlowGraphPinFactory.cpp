#include "ActorFlowGraphPinFactory.h"
#include "ActorFlowGraphSchema.h"
#include "KismetPins/SGraphPinExec.h"


TSharedPtr<class SGraphPin> FActorFlowGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (!InPin->GetSchema()->IsA<UActorFlowGraphSchema>())
	{
		// Limit pin widget creation to ActorFlowGraph schemas 
		return nullptr;
	}

	return SNew(SGraphPinExec, InPin);
}
