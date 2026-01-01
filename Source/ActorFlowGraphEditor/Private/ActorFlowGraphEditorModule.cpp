#include "ActorFlowGraphEditorModule.h"
#include "AssetToolsModule.h"
#include "Asset/AssetTypeActions_ActorFlowGraph.h"
#include "EdGraphUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Selection.h"
#include "Asset/ActorFlowGraphAssetEditor.h"
#include "ActorFlowEditorStyle.h"
#include "ILiveCodingModule.h"

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

    if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (TSharedPtr<IAssetTypeActions>& AssetIt : RegisteredAssetTypeActions) {
            AssetTools.UnregisterAssetTypeActions(AssetIt.ToSharedRef());
        }
    }
    RegisteredAssetTypeActions.Empty();
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
