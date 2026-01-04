#include "ActorFlowGraphEditorModule.h"
#include "AssetToolsModule.h"
#include "Asset/AssetTypeActions_ActorFlowGraph.h"
#include "EdGraphUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Selection.h"
#include "Asset/ActorFlowGraphAssetEditor.h"
#include "ActorFlowEditorStyle.h"
#include "ILiveCodingModule.h"
#include "FlowVariableBase.h"
#include "FlowVariableFactoryRegistry.h"

IMPLEMENT_MODULE(FActorFlowGraphEditorModule, ActorFlowGraphEditor);

EAssetTypeCategories::Type FActorFlowGraphEditorModule::ActorFlowGraphAssetCategory = static_cast<EAssetTypeCategories::Type>(0);


void FActorFlowGraphEditorModule::StartupModule()
{
	FActorFlowEditorStyle::Initialize();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	//Get or create Flow category in Assets Categories
	ActorFlowGraphAssetCategory = AssetTools.RegisterAdvancedAssetCategory("Flow",
		FText::FromString("Flow"));

	//Add Actor Flow Graph creation into Asset Add menu (in Flow)
	TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_ActorFlowGraph());
	AssetTools.RegisterAssetTypeActions(Action);
	RegisteredAssetTypeActions.Add(Action);

	FlowVariableFactoryHandle = FFlowVariableFactoryRegistry::RegisterFactory(FFlowVariableFactoryDelegate::CreateStatic(&FActorFlowGraphEditorModule::MakeCustomFlowVar));

	USelection::SelectionChangedEvent.AddRaw(this, &FActorFlowGraphEditorModule::OnEditorSelectionChanged);

	if (GEditor)
	{
		GEditor->OnLevelActorDeleted().AddRaw(
			this,
			&FActorFlowGraphEditorModule::OnActorDeleted
		);
	}
	RebuildFlowComponentsCache();

	if (FModuleManager::Get().IsModuleLoaded("LiveCoding"))
	{
		ILiveCodingModule& LiveCodingModule =
			FModuleManager::LoadModuleChecked<ILiveCodingModule>("LiveCoding");

		LiveCodingModule.GetOnPatchCompleteDelegate().AddRaw(
			this,
			&FActorFlowGraphEditorModule::RebuildFlowComponentsCache
		);
	}
}

void FActorFlowGraphEditorModule::ShutdownModule()
{
	FActorFlowEditorStyle::Shutdown();

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (TSharedPtr<IAssetTypeActions>& AssetIt : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(AssetIt.ToSharedRef());
		}
	}
	RegisteredAssetTypeActions.Empty();
	FFlowVariableFactoryRegistry::UnregisterFactory(FlowVariableFactoryHandle);
}

void FActorFlowGraphEditorModule::RebuildFlowComponentsCache()
{
	CachedFlowComponents.Reset();

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (!Class->IsChildOf(UActorComponent::StaticClass())
			|| Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)
			|| !Class->HasMetaData(TEXT("FlowComponent")))
		{
			continue;
		}

		CachedFlowComponents.Add(Class);
	}
}

void FActorFlowGraphEditorModule::OnEditorSelectionChanged(UObject* NewSelection)
{
	if (!GEditor)
	{
		return;
	}
	USelection* Selection = Cast<USelection>(NewSelection);
	if (!Selection)
	{
		return;
	}
	TArray<UObject*> SelectedActors;
	Selection->GetSelectedObjects(AActor::StaticClass(), SelectedActors);
	if (SelectedActors.IsEmpty())
	{
		return;
	}
	for (IAssetEditorInstance* Editor : GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllOpenEditors())
	{
		if (Editor->GetEditorName() == "ActorFlowGraphEditor")
		{
			FActorFlowGraphAssetEditor* OpenedEditor = static_cast<FActorFlowGraphAssetEditor*>(Editor);
			if (OpenedEditor)
			{
				for (UObject* SelectedObject : SelectedActors)
				{
					if (AActor* Actor = Cast<AActor>(SelectedObject))
					{
						OpenedEditor->SelectNode(Actor);
					}
				}
			}
		}
	}
}

void FActorFlowGraphEditorModule::OnActorDeleted(AActor* DeletedActor)
{
	if (!DeletedActor || !GEditor || GEditor->PlayWorld)
	{
		return;
	}

	for (IAssetEditorInstance* Editor : GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllOpenEditors())
	{
		if (Editor->GetEditorName() == "ActorFlowGraphEditor")
		{
			FActorFlowGraphAssetEditor* OpenedEditor = static_cast<FActorFlowGraphAssetEditor*>(Editor);
			if (OpenedEditor)
			{
				OpenedEditor->DeleteNodesByActor(DeletedActor);
			}

		}
	}
}

UFlowVariableBase* FActorFlowGraphEditorModule::MakeCustomFlowVar(UObject* Outer, FProperty* Prop)
{
	if (!Prop)
	{
		return nullptr;
	}

	// INT
	if (CastField<FIntProperty>(Prop))
	{
		return NewObject<UFlowVar_Int>(Outer);
	}

	// FLOAT
	if (CastField<FFloatProperty>(Prop))
	{
		return NewObject<UFlowVar_Float>(Outer);
	}

	// BOOL
	if (CastField<FBoolProperty>(Prop))
	{
		return NewObject<UFlowVar_Bool>(Outer);
	}

	// STRING
	if (CastField<FStrProperty>(Prop))
	{
		return NewObject<UFlowVar_String>(Outer);
	}

	// NAME
	if (CastField<FNameProperty>(Prop))
	{
		return NewObject<UFlowVar_Name>(Outer);
	}

	// TEXT
	if (CastField<FTextProperty>(Prop))
	{
		return NewObject<UFlowVar_Text>(Outer);
	}

	// ENUM
	if (auto* EP = CastField<FEnumProperty>(Prop))
	{
		auto* V = NewObject<UFlowVar_Enum>(Outer);
		V->Enum = EP->GetEnum();
		return V;
	}

	// OBJECT
	if (auto* OP = CastField<FObjectPropertyBase>(Prop))
	{
		auto* V = NewObject<UFlowVar_Object>(Outer);
		V->AllowedClass = OP->PropertyClass;
		return V;
	}

	// COLOR
	if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
	{
		if (StructProp->Struct == TBaseStructure<FLinearColor>::Get())
		{
			return NewObject<UFlowVar_LinearColor>(Outer);
		}
	}

	// FLOW VAR MAP
	if (FMapProperty* MapProperty = CastField<FMapProperty>(Prop))
	{
		// Check key type: FName
		if (CastField<FNameProperty>(MapProperty->KeyProp))
		{
			// Check value type: UFlowVariableBase*
			if (FObjectProperty* ObjectValueProperty = CastField<FObjectProperty>(MapProperty->ValueProp))
			{
				if (ObjectValueProperty->PropertyClass->IsChildOf(UFlowVariableBase::StaticClass()))
				{
					return NewObject<UFlowVar_Map>(Outer);
				}
			}
		}
	}

	return nullptr;
}
