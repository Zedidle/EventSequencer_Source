// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyBagWrapper.h"

UPropertyBagWrapper::UPropertyBagWrapper()
{
    PropertyBag.Reset();
}

bool UPropertyBagWrapper::AddProperty(const FName& Name, const FString& TypeString)
{
    EPropertyBagPropertyType Type = GetTypeFromString(TypeString);
    if (Type == EPropertyBagPropertyType::None)
    {
        LogError(FString::Printf(TEXT("无法识别的类型: %s"), *TypeString));
        return false;
    }
    
    // 检查是否已存在同名属性
    if (PropertyBag.FindPropertyDescByName(Name) != nullptr)
    {
        LogError(FString::Printf(TEXT("属性 %s 已存在"), *Name.ToString()));
        return false;
    }
    
    // 添加属性（这里简化，实际需要根据类型处理结构体等复杂类型）
    EPropertyBagAlterationResult Result = PropertyBag.AddProperty(Name, Type);
    return Result == EPropertyBagAlterationResult::Success;
}

bool UPropertyBagWrapper::SetValueInt(const FName& Name, int32 Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueInt32(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::SetValueFloat(const FName& Name, float Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueFloat(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::SetValueString(const FName& Name, const FString& Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueString(Name, Value);
    return Result == EPropertyBagResult::Success;
}

bool UPropertyBagWrapper::SetValueBool(const FName& Name, bool Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueBool(Name, Value);
    return Result == EPropertyBagResult::Success;
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


bool UPropertyBagWrapper::SetValueObject(const FName& Name, UObject* Value)
{
    EPropertyBagResult Result = PropertyBag.SetValueObject(Name, Value);
    return Result == EPropertyBagResult::Success;
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


float UPropertyBagWrapper::GetValueFloat(const FName& Name) const
{
    return 0;
}

FString UPropertyBagWrapper::GetValueString(const FName& Name) const
{
    auto V = PropertyBag.GetValueString(Name);

    if (V.HasError())
    {
        EPropertyBagResult ErrorCode = V.GetError();
        UE_LOG(LogTemp, Error, TEXT("获取 String 属性失败，错误码: %d"), static_cast<int32>(ErrorCode));
        // 返回默认值或采取其他措施
        return "";
    }
    if (V.HasValue()) return V.GetValue();
    
    return "";
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

bool UPropertyBagWrapper::MergeFrom(UPropertyBagWrapper* Other, bool bOverrideExisting)
{
    if (PropertyBag.IsValid())
    {
        PropertyBag.MigrateToNewBagInstance(Other->PropertyBag);
        return true;
    }
    return false;
}

void UPropertyBagWrapper::ClearAllProperties()
{
}

void UPropertyBagWrapper::PrintAllProperties(){
    
    // TConstArrayView<FPropertyBagPropertyDesc> Descs = PropertyBag.GetPropertyDescs();

    // UE_LOG(LogTemp, Log, TEXT("PropertyBag 包含 %d 个属性:"), Descs.Num());
    //
    // for (const FPropertyBagPropertyDesc* Desc : Descs)
    // {
    //     if (Desc)
    //     {
    //         // 获取属性名
    //         FName PropertyName = Desc->Name;
    //         
    //         // 获取属性类型
    //         FString TypeName = TEXT("Unknown");
    //         switch (Desc->ValueType)
    //         {
    //         case EPropertyBagPropertyType::Bool:
    //             TypeName = TEXT("Bool");
    //             break;
    //         case EPropertyBagPropertyType::Int32:
    //             TypeName = TEXT("Int32");
    //             break;
    //         case EPropertyBagPropertyType::Float:
    //             TypeName = TEXT("Float");
    //             break;
    //         case EPropertyBagPropertyType::Double:
    //             TypeName = TEXT("Double");
    //             break;
    //         case EPropertyBagPropertyType::String:
    //             TypeName = TEXT("String");
    //             break;
    //         case EPropertyBagPropertyType::Struct:
    //             TypeName = TEXT("Struct");
    //             if (Desc->ValueTypeObject.IsValid())
    //             {
    //                 TypeName = FString::Printf(TEXT("Struct(%s)"), *Desc->ValueTypeObject->GetName());
    //             }
    //             break;
    //         case EPropertyBagPropertyType::Object:
    //             TypeName = TEXT("Object");
    //             if (Desc->ValueTypeObject.IsValid())
    //             {
    //                 TypeName = FString::Printf(TEXT("Object(%s)"), *Desc->ValueTypeObject->GetName());
    //             }
    //             break;
    //         case EPropertyBagPropertyType::Enum:
    //             TypeName = TEXT("Enum");
    //             if (Desc->ValueTypeObject.IsValid())
    //             {
    //                 TypeName = FString::Printf(TEXT("Enum(%s)"), *Desc->ValueTypeObject->GetName());
    //             }
    //             break;
    //         default:
    //             break;
    //         }
    //         
    //         UE_LOG(LogTemp, Log, TEXT("  - %s: %s"), *PropertyName.ToString(), *TypeName);
    //     }
    // }
}

EPropertyBagPropertyType UPropertyBagWrapper::GetTypeFromString(const FString& TypeString) const
{
    return EPropertyBagPropertyType::Struct;
}

void UPropertyBagWrapper::LogError(const FString& Message) const
{
    
}
