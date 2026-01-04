#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal_TwoParams(
    UFlowVariableBase*,
    FFlowVariableFactoryDelegate,
    UObject* /*Outer*/,
    FProperty* /*Property*/
);

class UFlowVariableBase;

struct FFactoryEntry
{
    FFlowVariableFactoryDelegate Delegate;
    FDelegateHandle Handle;
};

class ACTORFLOWGRAPH_API FFlowVariableFactoryRegistry
{
public:
	FFlowVariableFactoryRegistry();
	~FFlowVariableFactoryRegistry();

    static FDelegateHandle RegisterFactory(const FFlowVariableFactoryDelegate& Factory);
    static void UnregisterFactory(FDelegateHandle Handle);

    static UFlowVariableBase* MakeFlowVarFromProperty(UObject* Outer, FProperty* Property);
private:
    static TArray<FFactoryEntry>& GetFactories();
};
