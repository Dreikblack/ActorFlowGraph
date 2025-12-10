#include "ActorFlowUtils.h"

static bool CallFunctionByName(UObject* Object, const FName FuncName, const TMap<FName, void*>& ArgValues)
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
			void* ValuePtr = ArgValues.FindRef(Prop->GetFName());
			if (ValuePtr)
			{
				Prop->CopyCompleteValue_InContainer(ParamsBuffer, ValuePtr);
			}
		}
	}

	Object->ProcessEvent(Func, ParamsBuffer);

	FMemory::Free(ParamsBuffer);

	return true;
}