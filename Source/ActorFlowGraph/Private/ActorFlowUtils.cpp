#include "ActorFlowUtils.h"

static bool CallFunctionByName(UObject* Object, const FName FuncName, TMap<FName, UFlowVariableBase*> VariablesMap)
{
	if (!Object)
	{
		return false;
	}

	UFunction* Func = Object->FindFunction(FuncName);
	if (!Func)
	{
		UE_LOG(LogTemp, Warning, TEXT("Function %s not found on %s"), *FuncName.ToString(), *Object->GetName());
		return false;
	}

	uint8* ParamsBuffer = (uint8*)FMemory::Malloc(Func->ParmsSize);
	FMemory::Memzero(ParamsBuffer, Func->ParmsSize);

	// Fill func args via FProperty
	for (TFieldIterator<FProperty> It(Func); It; ++It)
	{
		FProperty* Prop = *It;

		if (Prop->HasAnyPropertyFlags(CPF_Parm))
		{
			UFlowVariableBase* Var = VariablesMap.FindRef(Prop->GetFName());
			if (Var)
			{
				void* Dest = Prop->ContainerPtrToValuePtr<void>(ParamsBuffer);
				Var->PushToArgs(Dest);
			}
		}
	}

	Object->ProcessEvent(Func, ParamsBuffer);

	FMemory::Free(ParamsBuffer);

	return true;
}