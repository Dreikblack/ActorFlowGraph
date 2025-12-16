#include "ActorFlowEdGraphNode.h"
#include "SActorFlowGraphNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuSection.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"
#include "SFunctionPickerWidget.h"
#include "ActorFlowGraphSchema.h"
#include "EdGraph/EdGraphPin.h"

#define LOCTEXT_NAMESPACE "ActorFlowGraphNode"

TSharedPtr<SGraphNode> UActorFlowEdGraphNode::CreateVisualWidget()
{
	return SNew(SActorFlowGraphNode, this);
}


void UActorFlowEdGraphNode::AllocateDefaultPins()
{
	//no default pins
}

FText UActorFlowEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(ActorName);
}


bool UActorFlowEdGraphNode::CanUserDeleteNode() const
{
    return true;
}

bool UActorFlowEdGraphNode::CanDuplicateNode() const
{
    return false;
}

bool UActorFlowEdGraphNode::IncludeParentNodeContextMenu() const
{
    return true;
}

void UActorFlowEdGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context || !Context->Node)
	{
		return;
	}
	if (Context->Pin && !Context->Pin->WasTrashed())
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("EdGraphSchemaPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		Section.AddMenuEntry(
			FName("RemovePin"),
			FText::FromString("Remove Pin"),
			FText::FromString("Remove this pin"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Delete"),
			FUIAction(
				FExecuteAction::CreateLambda([ConstPin = Context->Pin]()
					{
						if (!ConstPin)
						{
							return;
						}
						UEdGraphNode* Node = ConstPin->GetOwningNode();
						if (!Node)
						{
							return;
						}
						UEdGraphPin* Pin = Node->FindPinById(ConstPin->PinId);
						if (Pin)
						{
							const FScopedTransaction Transaction(FText::FromString("Remove Pin"));
							Node->Modify();
							Node->RemovePin(Pin);
							Node->ReconstructNode();
							Node->GetGraph()->NotifyGraphChanged();
						}

					})
			)
		);
	}
	FToolMenuSection& Section = Menu->AddSection("ActorFlowGraphNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));

	UActorFlowEdGraphNode* NonConstThis = const_cast<UActorFlowEdGraphNode*>(this);

	Section.AddSubMenu(
		"AddInputPin",
		NSLOCTEXT("Flow", "AddFunctionPin", "Add Input Pin"),
		NSLOCTEXT("Flow", "AddFunctionPin_Tooltip", "Add output pin based on function"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateAttachAddOnSubMenu, true)
    );
	Section.AddSubMenu(
		"AddOutputPin",
		NSLOCTEXT("Flow", "AddFunctionPin", "Add Output Pin"),
		NSLOCTEXT("Flow", "AddFunctionPin_Tooltip", "Add output pin based on function"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateAttachAddOnSubMenu, false)
	);

	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	Section.AddMenuEntry(GenericCommands.Delete);
}


void UActorFlowEdGraphNode::CreateAttachAddOnSubMenu(UToolMenu* Menu, bool bIsInput)
{
	FToolMenuSection& SubSection = Menu->AddSection("OwnerList", NSLOCTEXT("Flow", "OwnerList", "Actor/Components"));
	TArray<FName> FNames;

	TArray<UObject*> Owners;
	if (AActor* ActorObject = Cast<AActor>(Actor.ResolveObject())) {
		Owners.Add(ActorObject);
		for (UActorComponent* Component : ActorObject->GetComponents())
		{
			Owners.Add(Component);
		}
		/*for (FComponentReference ComponentReference : Components)
		{
			if (UActorComponent* ActorComponent = Cast<UActorComponent>(ComponentReference.GetComponent(ActorObject)))
			{
				Owners.Add(ActorComponent);
			}
		}*/
	}


	for (UObject* Object : Owners)
	{
		TWeakObjectPtr<UObject> WeakObject(Object);

		SubSection.AddMenuEntry(
			Object->GetFName(),
			FText::FromName(Object->GetFName()),
			FText::FromString("Add pin for actor/component"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this, WeakObject, bIsInput]()
				{
					if (!WeakObject.IsValid())
					{
						return;
					}
					// Create Window
					TSharedRef<SWindow> Picker = SNew(SWindow)
						.Title(FText::FromString("Pick Function"))
						.ClientSize(FVector2D(600, 600))
						.SupportsMinimize(false)
						.SupportsMaximize(false);

					TWeakPtr<SWindow> WeakPicker = Picker;

					// Function Picker Widget
					Picker->SetContent(
						SNew(SFunctionPickerWidget)
						.TargetClass(WeakObject.Pin()->GetClass())
						.OnFunctionPicked_Lambda([this, WeakObject, WeakPicker, bIsInput](FName FuncName)
							{
								if (!WeakObject.IsValid())
								{
									return;
								}
								AddFunctionPin(WeakObject.Pin(), FuncName, bIsInput);

								FSlateApplication::Get().DismissAllMenus();

								// Closing window
								AsyncTask(ENamedThreads::GameThread, [WeakPicker]()
									{
										if (WeakPicker.IsValid())
										{
											FSlateApplication::Get().RequestDestroyWindow(
												WeakPicker.Pin().ToSharedRef()
											);
										}
									});
							})
					);

					// Add window
					FSlateApplication::Get().AddWindow(Picker);
				}))
		);
	}
}

void UActorFlowEdGraphNode::AddFunctionPin(TStrongObjectPtr<UObject> InObject, FName FunctionName, bool bIsInput)
{
	UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(GetGraph()->Schema));
	if (Schema)
	{
		FName OwnerName = InObject->GetFName();
		UEdGraphPin* FoundPin = FindPinByPredicate([FunctionName, OwnerName, bIsInput](const UEdGraphPin* Pin)
			{
				return Pin->GetFName() == FunctionName && Pin->GetPrimaryTerminalType().TerminalSubCategory == OwnerName && Pin->Direction == (bIsInput ? EGPD_Input : EGPD_Output);
			});
		if (FoundPin)
		{
			return;
		}
		Modify();
		Schema->CreatePin(this, FunctionName, OwnerName, bIsInput);
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}
}


#undef LOCTEXT_NAMESPACE
