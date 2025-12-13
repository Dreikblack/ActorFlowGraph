#include "ActorFlowGraphSchema.h"
#include "ActorFlowEdGraphNode.h"
#include "ActorFlowConnectionDrawingPolicy.h"


#define LOCTEXT_NAMESPACE "ActorFlowGraph"

void UActorFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
}

const FPinConnectionResponse UActorFlowGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins should be in and out"));
	}
	else if (A == B)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same pin"));
	}
	else if (A->GetFName() == B->GetFName() && A->GetOwningNode() == B->GetOwningNode() 
		&& A->GetPrimaryTerminalType().TerminalSubCategory == B->GetPrimaryTerminalType().TerminalSubCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("No loops (same function in same actor/component"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UActorFlowGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	bool bModified = Super::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		UActorFlowEdGraphNode* ActorFlowGraphNodeA = Cast<UActorFlowEdGraphNode>(PinA->GetOwningNode());
		UActorFlowEdGraphNode* ActorFlowGraphNodeB = Cast<UActorFlowEdGraphNode>(PinB->GetOwningNode());

		FGuidPair Pair(PinA->PinId, PinB->PinId);
		AActor* InActor = Cast<AActor>(ActorFlowGraphNodeB->Actor.TryLoad());
		if (!InActor)
		{
			return bModified;
		}
		if (PinB->GetPrimaryTerminalType().TerminalSubCategory == InActor->GetFName())
		{
			UFunction* Func = InActor->FindFunction(PinB->GetFName());
			if (Func)
			{		
				ActorFlowGraphNodeA->Connections.Add(Pair, GetVariablesByFunction(ActorFlowGraphNodeA, Func, PinA, PinB));
			}

		}
		else
		{
			for (UActorComponent* CurrentComponent : InActor->GetComponents())
			{
				if (PinB->GetPrimaryTerminalType().TerminalSubCategory == CurrentComponent->GetFName())
				{
					UFunction* Func = CurrentComponent->FindFunction(PinB->GetFName());
					if (Func)
					{
						ActorFlowGraphNodeA->Connections.Add(Pair, GetVariablesByFunction(ActorFlowGraphNodeA, Func, PinA, PinB));
					}
					break;
				}
			}
		}

		UEdGraph* EdGraph = ActorFlowGraphNodeA->GetGraph();

		EdGraph->NotifyNodeChanged(ActorFlowGraphNodeA);
		EdGraph->NotifyNodeChanged(ActorFlowGraphNodeB);
	}

	return bModified;
}

UFlowConnectionVariables* UActorFlowGraphSchema::GetVariablesByFunction(UActorFlowEdGraphNode* ActorFlowGraphNode, UFunction* InFunction, UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	UFlowConnectionVariables* Variables = NewObject<UFlowConnectionVariables>(ActorFlowGraphNode, UFlowConnectionVariables::StaticClass(), NAME_None, RF_Transactional);

	for (TFieldIterator<FProperty> It(InFunction); It; ++It)
	{
		FProperty* Prop = *It;
		UFlowVariableBase* Var = MakeFlowVarFromProperty(Variables, Prop);
		if (Var)
		{
			Variables->VariablesMap.Add(Prop->GetFName(), Var);
		}
		else
		{
			Variables->VariablesMap.Add(Prop->GetFName());
		}
	}
	Variables->ConnectionName = PinA->GetName() + " -> " + PinB->GetName();
	return Variables;
}

void UActorFlowGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	FGuidPair Pair(SourcePin->PinId, TargetPin->PinId);
	UActorFlowEdGraphNode* ActorFlowGraphNode = Cast<UActorFlowEdGraphNode>(SourcePin->GetOwningNode());

	UFlowConnectionVariables* Variables = ActorFlowGraphNode->Connections.FindRef(Pair);
	if (Variables)
	{
		ActorFlowGraphNode->Connections.Remove(Pair);
	}
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

void UActorFlowGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	UActorFlowEdGraphNode* ActorFlowGraphNode = Cast<UActorFlowEdGraphNode>(TargetPin.GetOwningNode());

	for (auto It = ActorFlowGraphNode->Connections.CreateIterator(); It; ++It)
	{
		if (It->Key.A == TargetPin.PinId)
		{
			It.RemoveCurrent();
		}
	}
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UActorFlowGraphSchema::CreatePins(UClass* InCls, FName InOwnerName, UActorFlowEdGraphNode* Node)
{
	FString OutputsString = InCls->GetMetaData(TEXT("FlowOutputs"));

	TArray<FString> Outputs;
	OutputsString.ParseIntoArray(Outputs, TEXT(","), true);

	for (FString& Out : Outputs)
	{
		Out = Out.TrimStartAndEnd();
		CreatePin(Node, FName(*Out), InOwnerName, false);
	}

	for (TFieldIterator<UFunction> It(InCls, EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		UFunction* Func = *It;
		if (Func->HasMetaData(TEXT("FlowInput")))
		{
			CreatePin(Node, Func->GetFName(), InOwnerName, false);
		}
	}
}

void UActorFlowGraphSchema::CreatePin(UActorFlowEdGraphNode* Node, FName PinName, FName InOwnerName, bool bIsInput)
{
	auto pin = Node->CreatePin(bIsInput ? EGPD_Input : EGPD_Output, TEXT("FlowPin"), InOwnerName, PinName);
}

FConnectionDrawingPolicy* UActorFlowGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FActorFlowConnectionDrawingPolicy(
		InBackLayerID,
		InFrontLayerID,
		InZoomFactor,
		InClippingRect,
		InDrawElements,
		InGraphObj
	);
}

void UActorFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
}

void UActorFlowGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{

}

FLinearColor UActorFlowGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Green;
}

#undef LOCTEXT_NAMESPACE
