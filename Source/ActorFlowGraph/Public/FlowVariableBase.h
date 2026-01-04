#pragma once

#include "CoreMinimal.h"
#include "FlowVariableBase.generated.h"

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class ACTORFLOWGRAPH_API UFlowVariableBase : public UObject
{
	GENERATED_BODY()

public:
	//Data Read
	virtual void PushToArgs(void* Dest) const
	{

	}

	//Set Value
	virtual void PullFromArgs(const void* Src)
	{

	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Int : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<int32*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const int32*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Float : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<float*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const float*>(Src);
	}
};


UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Bool : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<bool*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const bool*>(Src);
	}
};
UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_String : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FString*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FString*>(Src);
	}
};
UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Name : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FName*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FName*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Text : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FText Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FText*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FText*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Vector : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FVector Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FVector*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FVector*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Rotator : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FRotator Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FRotator*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FRotator*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Guid : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGuid Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FGuid*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FGuid*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Object : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UObject* Value = nullptr;

	UPROPERTY()
	UClass* AllowedClass = UObject::StaticClass();

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<UObject**>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<UObject* const*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Enum : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UEnum* Enum;

	UPROPERTY(EditAnywhere)
	int64 Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<int64*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const int64*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_LinearColor : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FLinearColor Value;

	virtual void PushToArgs(void* Dest) const override
	{
		*static_cast<FLinearColor*>(Dest) = Value;
	}

	virtual void PullFromArgs(const void* Src) override
	{
		Value = *static_cast<const FLinearColor*>(Src);
	}
};

UCLASS(EditInlineNew)
class ACTORFLOWGRAPH_API UFlowVar_Map : public UFlowVariableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced)
	TMap<FName, UFlowVariableBase*> Value;

	virtual void PushToArgs(void* Dest) const override
	{
		TMap<FName, UFlowVariableBase*>& OutMap = *static_cast<TMap<FName, UFlowVariableBase*>*>(Dest);

		OutMap.Empty(Value.Num());

		for (const TPair<FName, UFlowVariableBase*>& Pair : Value)
		{
			OutMap.Add(Pair.Key, Pair.Value);
		}
	}

	virtual void PullFromArgs(const void* Src) override
	{
		const TMap<FName, UFlowVariableBase*>& InMap = *static_cast<const TMap<FName, UFlowVariableBase*>*>(Src);
		Value.Empty(InMap.Num());
		for (const TPair<FName, UFlowVariableBase*>& Pair : InMap)
		{
			if (Pair.Value == nullptr)
			{
				Value.Add(Pair.Key, nullptr);
				continue;
			}
			UFlowVariableBase* NewVar = DuplicateObject<UFlowVariableBase>(Pair.Value, this);
			Value.Add(Pair.Key, NewVar);
		}
	}
};