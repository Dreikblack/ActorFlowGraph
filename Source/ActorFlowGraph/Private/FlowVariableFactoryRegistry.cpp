#include "FlowVariableFactoryRegistry.h"
#include "FlowVariableBase.h"

FFlowVariableFactoryRegistry::FFlowVariableFactoryRegistry(){}

FFlowVariableFactoryRegistry::~FFlowVariableFactoryRegistry(){}

FDelegateHandle FFlowVariableFactoryRegistry::RegisterFactory(const FFlowVariableFactoryDelegate& Factory)
{
    FFactoryEntry Entry;
    Entry.Delegate = Factory;
    Entry.Handle = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
    GetFactories().Add(Entry);
    return Entry.Handle;
}

void FFlowVariableFactoryRegistry::UnregisterFactory(FDelegateHandle Handle)
{
    GetFactories().RemoveAll(
        [Handle](const FFactoryEntry& Entry)
        {
            return Entry.Handle == Handle;
        }
    );
}

UFlowVariableBase* FFlowVariableFactoryRegistry::MakeFlowVarFromProperty(UObject* Outer, FProperty* Property)
{
    for (const FFactoryEntry& Entry : GetFactories())
    {
        if (UFlowVariableBase* Var =
            Entry.Delegate.Execute(Outer, Property))
        {
            return Var;
        }
    }

    return nullptr;
}

TArray<FFactoryEntry>& FFlowVariableFactoryRegistry::GetFactories()
{
    static TArray<FFactoryEntry> Factories;
    return Factories;
}

