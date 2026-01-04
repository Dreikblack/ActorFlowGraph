#include "Graph/ActorFlowEdGraphNode.h"
#include "Widgets/SActorFlowGraphNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuSection.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"
#include "Widgets/SFunctionPickerWidget.h"
#include "Graph/ActorFlowGraphSchema.h"
#include "EdGraph/EdGraphPin.h"
#include "ActorFlowGraphEditorModule.h"

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
		"AddFlowComponent",
		NSLOCTEXT("Flow", "AddFlowComponent", "Add Logical Component"),
		NSLOCTEXT("Flow", "AddFlowComponent_Tooltip", "Add Logical Flow Component to Actor"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateFlowComponentSubMenu)
	);

	Section.AddSubMenu(
		"AddInputPin",
		NSLOCTEXT("Flow", "AddFunctionPin", "Add Input Pin"),
		NSLOCTEXT("Flow", "AddFunctionPin_Tooltip", "Add output pin based on function"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateFunctionPinSubMenu, true)
	);

	Section.AddSubMenu(
		"AddOutputPin",
		NSLOCTEXT("Flow", "AddFunctionPin", "Add Output Pin"),
		NSLOCTEXT("Flow", "AddFunctionPin_Tooltip", "Add output pin based on function"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateFunctionPinSubMenu, false)
	);
	TWeakObjectPtr<UActorFlowEdGraphNode> WeakNode = const_cast<UActorFlowEdGraphNode*>(Cast<UActorFlowEdGraphNode>(Context->Node.Get()));

	Section.AddMenuEntry(
		FName("AddFlowPins"),
		FText::FromString("Reconstruct Flow Pins"),
		FText::FromString("Add missing Pins from meta"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this, WeakNode]()
			{
				if (UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(GetGraph()->Schema)))
				{
					if (AActor* ActorObject = Cast<AActor>(Actor.ResolveObject()))
					{
						if (UActorFlowEdGraphNode* Node = WeakNode.Get())
						{
							const FScopedTransaction Transaction(FText::FromString("Add Flow Pin"));

							Node->Modify();
							Node->ReconstructNode();
							GetGraph()->NotifyGraphChanged();

							Schema->CreatePins(ActorObject->GetClass(), ActorObject->GetFName(), Node, true);

							for (UActorComponent* Component : ActorObject->GetComponents())
							{
								Schema->CreatePins(Component->GetClass(), Component->GetFName(), Node, true);
							}
						}

					}
				}

			}))
	);

	Section.AddMenuEntry(
		FName("FixComponentPinsAfterRename"),
		FText::FromString("Fix Component's Name in Node"),
		FText::FromString("Fix Components Pins After Component Rename"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this, WeakNode]()
			{
				if (UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(GetGraph()->Schema)))
				{
					if (UActorFlowEdGraphNode* Node = WeakNode.Get())
					{
						const FScopedTransaction Transaction(FText::FromString("Fix Pins After Component Rename"));

						Node->Modify();
						Node->FixComponentPins();

						Node->ReconstructNode();
						GetGraph()->NotifyGraphChanged();
					}
				}

			}))
	);

	Section.AddSubMenu(
		"RemoveComponentPins",
		NSLOCTEXT("Flow", "RemoveComponentPins", "Remove Component's Pins"),
		NSLOCTEXT("Flow", "RemoveComponentPins_Tooltip", "Remove Pins of Selected Component"),
		FNewToolMenuDelegate::CreateUObject(NonConstThis, &UActorFlowEdGraphNode::CreateRemoveComponentPinsSubMenu)
	);

	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	Section.AddMenuEntry(GenericCommands.Delete);
}


void UActorFlowEdGraphNode::CreateFunctionPinSubMenu(UToolMenu* Menu, bool bIsInput)
{
	FToolMenuSection& SubSection = Menu->AddSection("OwnerList", NSLOCTEXT("Flow", "OwnerList", "Actor/Components"));

	TArray<UObject*> Owners;
	if (AActor* ActorObject = Cast<AActor>(Actor.ResolveObject()))
	{
		Owners.Add(ActorObject);
		for (UActorComponent* Component : ActorObject->GetComponents())
		{
			Owners.Add(Component);
		}
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

void UActorFlowEdGraphNode::CreateFlowComponentSubMenu(UToolMenu* Menu)
{
	FToolMenuSection& SubSection = Menu->AddSection("FlowComponentList", NSLOCTEXT("Flow", "FlowComponentList", "Logical Flow Components"));

	FActorFlowGraphEditorModule& Module = FModuleManager::LoadModuleChecked<FActorFlowGraphEditorModule>("ActorFlowGraphEditor");

	const TArray<UClass*>& CachedFlowComponents = Module.GetFlowComponentClasses();

	for (UClass* ComponentClass : CachedFlowComponents)
	{
		SubSection.AddMenuEntry(
			ComponentClass->GetFName(),
			UActorFlowGraphSchema::GetFlowComponentName(ComponentClass),
			FText::FromString("Add component to actor of this node"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this, ComponentClass]()
				{
					AddFlowComponent(ComponentClass);
				})
			)
		);
	}

}

void UActorFlowEdGraphNode::CreateRemoveComponentPinsSubMenu(UToolMenu* Menu)
{
	FToolMenuSection& SubSection = Menu->AddSection("FlowComponentList", NSLOCTEXT("Flow", "NodeComponentList", "Node's Components"));

	TSet<FName> ComponentNames;
	for (UEdGraphPin* Pin : Pins)
	{
		ComponentNames.Add(Pin->GetPrimaryTerminalType().TerminalSubCategory);
	}
	for (FName ComponentName : ComponentNames)
	{
		SubSection.AddMenuEntry(
			ComponentName,
			FText::FromName(ComponentName),
			FText::FromString("Remove Component's pins"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([this, ComponentName]()
				{
					RemoveComponentPins(ComponentName);
				})
			)
		);
	}
}

void UActorFlowEdGraphNode::AddFlowComponent(UClass* InComponentClass)
{
	if (AActor* ActorObject = Cast<AActor>(Actor.ResolveObject()))
	{
		const FScopedTransaction Transaction(FText::FromString("Add Flow Component to Node's Actor"));
		UWorld* World = GEditor->GetEditorWorldContext().World();
		check(World);

		ActorObject->Modify();
		FName UniqueName = MakeUniqueObjectName(
			World,
			InComponentClass
		);
		UActorComponent* Component = NewObject<UActorComponent>(
			ActorObject,
			InComponentClass,
			UniqueName,
			RF_Transactional | RF_Public
		);

		Component->Modify();
		Component->bAutoActivate = true;
		Component->RegisterComponent();
		ActorObject->AddInstanceComponent(Component);

		Modify();
		UActorFlowGraphSchema* Schema = Cast<UActorFlowGraphSchema>(GetMutableDefault<UEdGraphSchema>(GetGraph()->Schema));
		Schema->CreatePins(Component->GetClass(), Component->GetFName(), this, true);

		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
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
		const FScopedTransaction Transaction(FText::FromString("Add Function Pin"));

		Modify();
		Schema->CreatePin(this, InObject->GetClass()->GetFName(), FunctionName, OwnerName, bIsInput);
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}
}

FLinearColor UActorFlowEdGraphNode::GetNodeBodyTintColor() const
{
	return FLinearColor(0.5f, 0.5f, 0.5f, 0.8f);
}

void UActorFlowEdGraphNode::FixComponentPins()
{
	if (AActor* ActorObject = Cast<AActor>(Actor.ResolveObject()))
	{
		FName ActorClassName = ActorObject->GetClass()->GetFName();
		TMap<TPair<FName, FName>, TArray<UEdGraphPin*>> ComponentClassPinMap;

		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin->GetPrimaryTerminalType().TerminalCategory == ActorClassName)
			{
				continue;
			}
			TPair<FName, FName> ClassAndComponentNamesKey(Pin->GetPrimaryTerminalType().TerminalCategory, Pin->GetPrimaryTerminalType().TerminalSubCategory);
			ComponentClassPinMap.FindOrAdd(ClassAndComponentNamesKey).Add(Pin);
		}
		TMap<FName, TArray<UActorComponent*>> ExistingComponentClassMap;
		TMap<FName, TArray<UActorComponent*>> ExistingComponentNameMap;
		for (UActorComponent* Component : ActorObject->GetComponents())
		{
			ExistingComponentClassMap.FindOrAdd(Component->GetClass()->GetFName()).Add(Component);
			ExistingComponentNameMap.FindOrAdd(Component->GetFName()).Add(Component);
		}

		TArray<TPair<FName, FName>> NameKeys;
		ComponentClassPinMap.GetKeys(NameKeys);

		for (TPair<FName, FName> ClassAndComponentNamesKey : NameKeys)
		{
			if (!ExistingComponentNameMap.Contains(ClassAndComponentNamesKey.Value) && ExistingComponentClassMap.Contains(ClassAndComponentNamesKey.Key))
			{
				TArray<UEdGraphPin*>* PinsByKey = ComponentClassPinMap.Find(ClassAndComponentNamesKey);
				TArray<UActorComponent*>* Components = ExistingComponentClassMap.Find(ClassAndComponentNamesKey.Key);
				if (Components->Num() != 1)
				{
					continue;
				}
				FName ComponentName = (*Components)[0]->GetFName();
				for (UEdGraphPin* Pin : *PinsByKey)
				{
					Pin->PinType.PinSubCategory = ComponentName;
				}
			}
		}
	}

}

void UActorFlowEdGraphNode::RemoveComponentPins(FName InComponentName)
{
	const FScopedTransaction Transaction(FText::FromString("Remove Component's Pins"));

	Modify();
	TArray<UEdGraphPin*> PinsToRemove;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->GetPrimaryTerminalType().TerminalSubCategory == InComponentName)
		{
			PinsToRemove.Add(Pin);
		}
	}
	for (UEdGraphPin* Pin : PinsToRemove)
	{
		RemovePin(Pin);
	}
	ReconstructNode();
	GetGraph()->NotifyGraphChanged();
}


#undef LOCTEXT_NAMESPACE
