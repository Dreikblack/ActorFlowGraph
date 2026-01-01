#include "Asset/ActorFlowGraphAssetFactory.h"
#include "Asset/ActorFlowGraphAsset.h"

UActorFlowGraphAssetFactory::UActorFlowGraphAssetFactory()
{
	SupportedClass = UActorFlowGraphAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UActorFlowGraphAssetFactory::FactoryCreateNew(
	UClass* Class,
	UObject* InParent,
	FName Name,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn)
{
	UActorFlowGraphAsset* NewAsset = NewObject<UActorFlowGraphAsset>(InParent, Class, Name, Flags);
	return NewAsset;
}
