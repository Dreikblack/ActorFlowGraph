#pragma once

#include "CoreMinimal.h"
#include "FlowVariableBase.generated.h"

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class UFlowVariableBase : public UObject
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
class UFlowVar_Int : public UFlowVariableBase
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
class UFlowVar_Float : public UFlowVariableBase
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
class UFlowVar_Bool : public UFlowVariableBase
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
class UFlowVar_String : public UFlowVariableBase
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
class UFlowVar_Name : public UFlowVariableBase
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
class UFlowVar_Text : public UFlowVariableBase
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
class UFlowVar_Vector : public UFlowVariableBase
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
class UFlowVar_Rotator : public UFlowVariableBase
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
class UFlowVar_Guid : public UFlowVariableBase
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
class UFlowVar_Object : public UFlowVariableBase
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
class UFlowVar_Enum : public UFlowVariableBase
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
class UFlowVar_LinearColor : public UFlowVariableBase
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

static UFlowVariableBase* MakeFlowVarFromProperty(UObject* Outer, FProperty* Prop)
{
    if (!Prop)
        return nullptr;

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


    return nullptr;
}