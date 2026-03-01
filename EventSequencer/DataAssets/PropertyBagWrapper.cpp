// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyBagWrapper.h"

UPropertyBagWrapper::UPropertyBagWrapper()
{

}

void UPropertyBagWrapper::PostInitProperties()
{
    Super::PostInitProperties();
}

void UPropertyBagWrapper::PostLoad()
{
    Super::PostLoad();

}

bool UPropertyBagWrapper::InitEmptyPropertyBag()
{
    if (bBagInitialized) return true;
    
    // 1. 安全创建空壳 UPropertyBag（绑定 Outer 避免 GC）
    UPropertyBag* DefaultBag = NewObject<UPropertyBag>(this, TEXT("EmptyPropertyBag"));
    if (!DefaultBag)
    {
        UE_LOG(LogTemp, Error, TEXT("创建空UPropertyBag失败！"));
        return false;
    }
    
    // 3. 初始化 FInstancedPropertyBag（此时反射完全就绪）
    PropertyBag.Reset(); // 清空残留状态
    DefaultBag->PrepareCppStructOps();
    PropertyBag.InitializeFromBagStruct(DefaultBag);
    
    // 4. 验证并标记
    if (PropertyBag.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("空壳PropertyBag初始化成功，UPropertyBag地址：%p"), DefaultBag);
        bBagInitialized = true;
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FInstancedPropertyBag初始化失败！"));
        return false;
    }
}

bool UPropertyBagWrapper::AddProperty(const FName& Name, const EPropertyBagPropertyType& Type)
{
    InitEmptyPropertyBag();
    
    // 检查是否已存在同名属性
    if (PropertyBag.FindPropertyDescByName(Name) != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("属性 %s 已存在"), *Name.ToString());
        return false;
    }
    
    // 添加属性（这里简化，实际需要根据类型处理结构体等复杂类型）
    EPropertyBagAlterationResult Result = PropertyBag.AddProperty(Name, Type);
    return Result == EPropertyBagAlterationResult::Success;
}

bool UPropertyBagWrapper::SetValueBool(const FName& Name, bool Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueBool(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueBool(const FName& Name, bool Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Bool);
    return SetValueBool(Name, Value);
}

bool UPropertyBagWrapper::SetValueByte(const FName& Name, uint8 Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueByte(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueByte(const FName& Name, uint8 Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Byte);
    return SetValueByte(Name, Value);
}

bool UPropertyBagWrapper::SetValueInt(const FName& Name, int32 Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueInt32(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueInt(const FName& Name, int32 Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Int32);
    return SetValueInt(Name, Value);
}

// bool UPropertyBagWrapper::SetValueUInt32(const FName& Name, uint32 Value)
// {
//     EPropertyBagResult Result = PropertyBag.SetValueUInt32(Name, Value);
//     return Result == EPropertyBagResult::Success;
// }
//
// bool UPropertyBagWrapper::AddValueUInt32(const FName& Name, uint32 Value)
// {
//     AddProperty(Name, EPropertyBagPropertyType::UInt32);
//     return SetValueUInt32(Name, Value);
// }

bool UPropertyBagWrapper::SetValueInt64(const FName& Name, int64 Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueInt64(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueInt64(const FName& Name, int64 Value)
{
    AddProperty(Name, EPropertyBagPropertyType::UInt64);
    return SetValueInt64(Name, Value);
}

// bool UPropertyBagWrapper::SetValueUInt64(const FName& Name, uint64 Value)
// {
//     EPropertyBagResult Result = PropertyBag.SetValueUInt64(Name, Value);
//     return Result == EPropertyBagResult::Success;
// }
//
// bool UPropertyBagWrapper::AddValueUInt64(const FName& Name, uint64 Value)
// {
//     AddProperty(Name, EPropertyBagPropertyType::UInt64);
//     return SetValueUInt64(Name, Value);
// }

bool UPropertyBagWrapper::SetValueFloat(const FName& Name, float Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueFloat(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueFloat(const FName& Name, float Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Float);
    return SetValueFloat(Name, Value);
}

bool UPropertyBagWrapper::SetValueDouble(const FName& Name, double Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueDouble(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueDouble(const FName& Name, double Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Double);
    return SetValueDouble(Name, Value);
}

bool UPropertyBagWrapper::SetValueName(const FName& Name, const FName& Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueName(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueName(const FName& Name, const FName& Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Name);
    return SetValueName(Name, Value);
}

bool UPropertyBagWrapper::SetValueString(const FName& Name, const FString& Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueString(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueString(const FName& Name, const FString& Value)
{
    AddProperty(Name, EPropertyBagPropertyType::String);
    return SetValueString(Name, Value);
}

bool UPropertyBagWrapper::SetValueText(const FName& Name, const FText& Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueText(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueText(const FName& Name, const FText& Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Text);
    return SetValueText(Name, Value);
}

bool UPropertyBagWrapper::SetValueVector(const FName& Name, const FVector& Value)
{
    // 直接使用 FConstStructView::Make
    FConstStructView StructView = FConstStructView::Make(Value);
    
    // 调用 SetValueStruct，让它自动处理类型
    EPropertyBagResult Result = PropertyBag.SetValueStruct(Name, StructView);
    
    // 如果返回 TypeMismatch 错误，先删除再添加
    if (Result == EPropertyBagResult::TypeMismatch)
    {
        PropertyBag.RemovePropertyByName(Name);
        Result = PropertyBag.SetValueStruct(Name, StructView);
    }
    
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueVector(const FName& Name, const FVector& Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Struct);
    return SetValueVector(Name, Value);
}

bool UPropertyBagWrapper::SetValueObject(const FName& Name, UObject* Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueObject(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueObject(const FName& Name, UObject* Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Object);
    return SetValueObject(Name, Value);
}

bool UPropertyBagWrapper::SetValueClass(const FName& Name, UClass* Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueClass(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueClass(const FName& Name, UClass* Value)
{
    AddProperty(Name, EPropertyBagPropertyType::Class);
    return SetValueClass(Name, Value);
}

bool UPropertyBagWrapper::SetValueSoftPath(const FName& Name, const FSoftObjectPath& Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueSoftPath(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::AddValueSoftPath(const FName& Name, const FSoftObjectPath& Value)
{
    AddProperty(Name, EPropertyBagPropertyType::SoftObject);
    return SetValueSoftPath(Name, Value);
}



bool UPropertyBagWrapper::GetValueBool(const FName& Name) const
{
    auto V = PropertyBag.GetValueBool(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 bool 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return false;
    }
    if (V.HasValue()) return V.GetValue();
    
    return false;
}

uint8 UPropertyBagWrapper::GetValueByte(const FName& Name) const
{
    auto V = PropertyBag.GetValueByte(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 Byte 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return false;
    }
    if (V.HasValue()) return V.GetValue();
    
    return false;
}

int32 UPropertyBagWrapper::GetValueInt(const FName& Name) const
{
    auto V = PropertyBag.GetValueInt32(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 int 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return 0;
    }
    if (V.HasValue()) return V.GetValue();
    
    return 0;
}

int64 UPropertyBagWrapper::GetValueInt64(const FName& Name) const
{
    auto V = PropertyBag.GetValueInt64(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 int64 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return 0;
    }
    if (V.HasValue()) return V.GetValue();
    
    return 0;
}


float UPropertyBagWrapper::GetValueFloat(const FName& Name) const
{
    auto V = PropertyBag.GetValueFloat(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 float 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return 0;
    }
    if (V.HasValue()) return V.GetValue();
    
    return 0;
}

double UPropertyBagWrapper::GetValueDouble(const FName& Name) const
{
    auto V = PropertyBag.GetValueDouble(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 double 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return 0;
    }
    if (V.HasValue()) return V.GetValue();
    
    return 0;
}

FName UPropertyBagWrapper::GetValueName(const FName& Name) const
{
    auto V = PropertyBag.GetValueName(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 FName 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return FName();
    }
    if (V.HasValue()) return V.GetValue();
    
    return FName();
}

FString UPropertyBagWrapper::GetValueString(const FName& Name) const
{
    auto V = PropertyBag.GetValueString(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 FString 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return "";
    }
    if (V.HasValue()) return V.GetValue();
    
    return "";
}

FText UPropertyBagWrapper::GetValueText(const FName& Name) const
{
    auto V = PropertyBag.GetValueText(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 Text 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return FText();
    }
    if (V.HasValue()) return V.GetValue();
    
    return FText();
}



FVector UPropertyBagWrapper::GetValueVector(const FName& Name) const
{
    auto V = PropertyBag.GetValueStruct(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 FVector 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return FVector::ZeroVector;
    }

    if (V.HasValue())
    {
        // 3. 验证 Struct 类型是否为 FVector
        FStructView View = V.GetValue();
        const UScriptStruct* StructType = View.GetScriptStruct();
        if (!StructType && StructType != TBaseStructure<FVector>::Get())
        {
            UE_LOG(LogTemp, Error,
                TEXT("属性 %s 不是FVector类型，实际类型: %s"),
                *Name.ToString(),
                StructType ? *StructType->GetName() : TEXT("None"));
        }
        
        return View.Get<FVector>();
    }
    
    return FVector::ZeroVector;
}

UObject* UPropertyBagWrapper::GetValueObject(const FName& Name) const
{
    auto V = PropertyBag.GetValueObject(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 UObject 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return nullptr;
    }
    if (V.HasValue()) return V.GetValue();
    
    return nullptr;
}

UClass* UPropertyBagWrapper::GetValueClass(const FName& Name) const
{
    auto V = PropertyBag.GetValueClass(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 UClass 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return nullptr;
    }
    if (V.HasValue()) return V.GetValue();
    
    return nullptr;
}

FSoftObjectPath UPropertyBagWrapper::GetValueSoftPath(const FName& Name) const
{
    auto V = PropertyBag.GetValueSoftPath(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 UClass 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return nullptr;
    }
    if (V.HasValue()) return V.GetValue();
    
    return nullptr;
}

bool UPropertyBagWrapper::MergeFrom(UPropertyBagWrapper* Other, bool bOverrideExisting)
{
    if (PropertyBag.IsValid())
    {
        PropertyBag.MigrateToNewBagInstance(Other->PropertyBag);
        return true;
    }
    return false;
}
