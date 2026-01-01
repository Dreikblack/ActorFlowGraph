#include "Graph/ActorFlowGraphSchema.h"
#include "Graph/ActorFlowEdGraphNode.h"
#include "Widgets/ActorFlowConnectionDrawingPolicy.h"
#include "Graph/ActorFlowSchemaActions.h"
#include "ActorFlowGraphEditorModule.h"

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

void UActorFlowGraphSchema::CreatePins(UClass* InCls, FName InOwnerName, UActorFlowEdGraphNode* Node, bool bDoCheckIfExist)
{
	FString OutputsString = InCls->GetMetaData(TEXT("FlowOutputs"));
	//if (OutputsString.IsEmpty() && InCls->GetSuperClass())
	//{
	//	OutputsString = InCls->GetSuperClass()->GetMetaData(TEXT("FlowOutputs"));
	//}
	UClass* Class = InCls;
	while (Class && OutputsString.IsEmpty())
	{
		if (Class->HasMetaData(TEXT("FlowOutputs")))
		{
			OutputsString = Class->GetMetaData(TEXT("FlowOutputs"));
			break;
		}
		Class = Class->GetSuperClass();
	}

	TArray<FString> Outputs;
	OutputsString.ParseIntoArray(Outputs, TEXT(","), true);

	for (FString& Out : Outputs)
	{
		Out = Out.TrimStartAndEnd();
		FName PinName = FName(*Out);
		bool bDoCreate = true;
		if (bDoCheckIfExist)
		{
			UEdGraphPin* FoundPin = Node->FindPinByPredicate([PinName, InOwnerName](const UEdGraphPin* Pin)
				{
					return Pin->GetFName() == PinName && Pin->GetPrimaryTerminalType().TerminalSubCategory == InOwnerName && Pin->Direction == EGPD_Output;
				});
			bDoCreate = !FoundPin;
		}
		
		if (bDoCreate)
		{
			CreatePin(Node, FName(*Out), InOwnerName, false);
		}
	}

	for (TFieldIterator<UFunction> It(InCls, EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		UFunction* Func = *It;
		FName PinName = Func->GetFName();
		bool bDoCreate = true;
		if (bDoCheckIfExist)
		{
			UEdGraphPin* FoundPin = Node->FindPinByPredicate([PinName, InOwnerName](const UEdGraphPin* Pin)
				{
					return Pin->GetFName() == PinName && Pin->GetPrimaryTerminalType().TerminalSubCategory == InOwnerName && Pin->Direction == EGPD_Input;
				});
			bDoCreate = !FoundPin;
		}
		if (bDoCreate && Func->HasMetaData(TEXT("FlowInput")))
		{
			CreatePin(Node, PinName, InOwnerName, true);
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
	if (!ContextMenuBuilder.CurrentGraph)
	{
		return;
	}
	
	FActorFlowGraphEditorModule& Module = FModuleManager::LoadModuleChecked<FActorFlowGraphEditorModule>("ActorFlowGraphEditor");

	const TArray<UClass*>& CachedFlowComponents = Module.GetFlowComponentClasses();

	for (UClass* ComponentClass : CachedFlowComponents)
	{
		FString Name = FName::NameToDisplayString(ComponentClass->GetName(), false);
		Name.RemoveFromEnd(TEXT(" Component"));

		ContextMenuBuilder.AddAction(
			MakeShared<FActorFlowSchemaAction_NewNode>(
				FText::FromString("Components"),
				FText::FromString(Name),
				FText::FromString("Add Actor with this Component"),
				0,
				ComponentClass
			)
		);
	}
	if (!ContextMenuBuilder.FromPin)
	{
		ContextMenuBuilder.AddAction(
			MakeShared<FActorFlowSchemaAction_NewComment>(
				FText::GetEmpty(),
				FText::FromString("Add Comment"),
				FText::FromString("Create new Comment"),
				0
			)
		);
	}
	
	
}

void UActorFlowGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{

}

FLinearColor UActorFlowGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Green;
}

#undef LOCTEXT_NAMESPACE
