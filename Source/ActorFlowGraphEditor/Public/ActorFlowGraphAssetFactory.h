#pragma once
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ActorFlowGraphAssetFactory.generated.h"

UCLASS()
class ACTORFLOWGRAPHEDITOR_API UActorFlowGraphAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UActorFlowGraphAssetFactory();

	virtual UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
	) override;

	virtual bool ShouldShowInNewMenu() const override
	{
		return true;
	}
};
