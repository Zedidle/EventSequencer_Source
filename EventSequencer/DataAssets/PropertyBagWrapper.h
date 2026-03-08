// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/Object.h"
#include "PropertyBagWrapper.generated.h"


UCLASS(BlueprintType, Blueprintable)
class EVENTSEQUENCER_API UPropertyBagWrapper : public UObject
{
	GENERATED_BODY()

	bool bBagInitialized = false;

	
protected:
	// 内部存储
	UPROPERTY()
	FInstancedPropertyBag PropertyBag;
	
public:
    // 构造函数
    UPropertyBagWrapper();
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool InitEmptyPropertyBag();
	
	FORCEINLINE FInstancedPropertyBag& GetPropertyBag() { return PropertyBag; }

	void SetPropertyBag(const FInstancedPropertyBag& Bag) { PropertyBag = Bag;}
	
	bool AddProperty(const FName Name, const EPropertyBagPropertyType& Type);
	
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueBool(const FName Name, bool Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueBool(const FName Name, bool Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueByte(const FName Name, uint8 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueByte(const FName Name, uint8 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueInt(const FName Name, int32 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueInt(const FName Name, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueInt64(const FName Name, int64 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueInt64(const FName Name, int64 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueFloat(const FName Name, float Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueFloat(const FName Name, float Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueDouble(const FName Name, double Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueDouble(const FName Name, double Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueName(const FName Name, const FName Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueName(const FName Name, const FName Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueString(const FName Name, const FString& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueString(const FName Name, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueText(const FName Name, const FText& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueText(const FName Name, const FText& Value);
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool SetValueVector(const FName Name, const FVector& Value);
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool AddValueVector(const FName Name, const FVector& Value);
	
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueObject(const FName Name, UObject* Value);
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueObject(const FName Name, UObject* Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueClass(const FName Name, UClass* Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueClass(const FName Name, UClass* Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueSoftPath(const FName Name, const FSoftObjectPath& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueSoftPath(const FName Name, const FSoftObjectPath& Value);

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool GetValueBool(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	uint8 GetValueByte(const FName Name) const;
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	int32 GetValueInt(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	int64 GetValueInt64(const FName Name) const;
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	float GetValueFloat(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	double GetValueDouble(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FName GetValueName(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FString GetValueString(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FText GetValueText(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FVector GetValueVector(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	UObject* GetValueObject(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	UClass* GetValueClass(const FName Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FSoftObjectPath GetValueSoftPath(const FName Name) const;
	
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool MergeFrom(UPropertyBagWrapper* Other, bool bOverrideExisting = true);


	template<typename T>
	EPropertyBagPropertyType GetPropertyType()
	{
		if constexpr (std::is_same_v<T, bool>)
		{
			return EPropertyBagPropertyType::Bool;
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			return EPropertyBagPropertyType::Byte;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return EPropertyBagPropertyType::Int32;
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return EPropertyBagPropertyType::Int64;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return EPropertyBagPropertyType::Float;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return EPropertyBagPropertyType::Double;
		}
		else if constexpr (std::is_same_v<T, FName>)
		{
			return EPropertyBagPropertyType::Name;
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return EPropertyBagPropertyType::String;
		}
		else if constexpr (std::is_same_v<T, FText>)
		{
			return EPropertyBagPropertyType::Text;
		}

		// 目前标记为 UMETA(Hidden) 的 EPropertyBagPropertyType 属性并不支持蓝图修改
		else if constexpr (std::is_same_v<T, FVector>)
		{
			return EPropertyBagPropertyType::Struct;
		}
		else if constexpr (std::is_same_v<T, UObject>)
		{
			return EPropertyBagPropertyType::Object;
		}
		else if constexpr (std::is_same_v<T, UClass>)
		{
			return EPropertyBagPropertyType::Class;
		}
		else if constexpr (std::is_same_v<T, FSoftObjectPath>)
		{
			return EPropertyBagPropertyType::SoftClass;
		}
		else
		{
			return EPropertyBagPropertyType::None;
		}
	}


	template<typename T>
	bool AddValue(const FName Name, T Value)
	{
		// 获取对应的属性类型枚举
		EPropertyBagPropertyType Type = GetPropertyType<T>();
    
		// 添加属性
		AddProperty(Name, Type);
		return SetValue<T>(Name, Value);
	}

	// 修改SetValue函数，支持类型推导
	template<typename T>
	bool SetValue(const FName Name, T Value)
	{
		if constexpr (std::is_same_v<T, bool>)
		{
			return SetValueBool(Name, Value);
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			return SetValueByte(Name, Value);
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return SetValueInt(Name, Value);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return SetValueInt64(Name, Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return SetValueFloat(Name, Value);
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return SetValueDouble(Name, Value);
		}
		else if constexpr (std::is_same_v<T, FName>)
		{
			return SetValueName(Name, Value);
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return SetValueString(Name, Value);
		}
		else if constexpr (std::is_same_v<T, FText>)
		{
			return SetValueText(Name, Value);
		}
		else if constexpr (std::is_same_v<T, FVector>)
		{
			return SetValueVector(Name, Value);
		}
		else if constexpr (std::is_same_v<T, UObject>)
		{
			return SetValueObject(Name, Value);
		}
		else if constexpr (std::is_same_v<T, UClass>)
		{
			return SetValueObject(Name, Value);
		}
		else if constexpr (std::is_same_v<T, FSoftObjectPath>)
		{
			return SetValueObject(Name, Value);
		}
		else
		{
			return false;
		}
	}

	
	template<typename T>
	bool ConvertStringToType(const FString& StringValue, T& OutValue)
	{
		if constexpr (std::is_same_v<T, bool>)
		{
			// 支持 "true"/"false", "True"/"False", "1"/"0"
			FString Lower = StringValue.ToLower();
			if (Lower == TEXT("true") || Lower == TEXT("1"))
			{
				OutValue = true;
				return true;
			}
			else if (Lower == TEXT("false") || Lower == TEXT("0"))
			{
				OutValue = false;
				return true;
			}
			return false;
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			int32 Temp = FCString::Atoi(*StringValue);
			if (Temp >= 0 && Temp <= 255)
			{
				OutValue = static_cast<uint8>(Temp);
				return true;
			}
			return false;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			OutValue = FCString::Atoi(*StringValue);
			return true;
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			OutValue = FCString::Atoi64(*StringValue);
			return true;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			OutValue = FCString::Atof(*StringValue);
			return true;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			OutValue = FCString::Atod(*StringValue);
			return true;
		}
		else if constexpr (std::is_same_v<T, FName>)
		{
			OutValue = FName(*StringValue);
			return true;
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			OutValue = StringValue;
			return true;
		}
		else if constexpr (std::is_same_v<T, FText>)
		{
			OutValue = FText::FromString(StringValue);
			return true;
		}
		else
		{
			return false;
		}
	}
	

// 主函数：根据 EPropertyBagPropertyType 转换 FString
bool AddValueFromString(const FName Name, EPropertyBagPropertyType Type, const FString& StringValue)
{
    switch (Type)
    {
    case EPropertyBagPropertyType::Bool:
    {
        bool Value = false;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<bool>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Byte:
    {
        uint8 Value = 0;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<uint8>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Int32:
    {
        int32 Value = 0;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<int32>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Int64:
    {
        int64 Value = 0;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<int64>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Float:
    {
        float Value = 0.0f;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<float>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Double:
    {
        double Value = 0.0;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<double>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Name:
    {
        FName Value = NAME_None;
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<FName>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::String:
    {
        FString Value = TEXT("");
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<FString>(Name, Value);
        }
        break;
    }
    case EPropertyBagPropertyType::Text:
    {
        FText Value = FText::GetEmpty();
        if (ConvertStringToType(StringValue, Value))
        {
            return AddValue<FText>(Name, Value);
        }
        break;
    }
    // 蓝图不支持就另说
    case EPropertyBagPropertyType::UInt32:
    {
        int32 Temp = FCString::Atoi(*StringValue);
        if (Temp >= 0)
        {
            uint32 Value = static_cast<uint32>(Temp);
            // 需要对应的 SetValueUInt32 函数
            EPropertyBagResult Result = PropertyBag.SetValueUInt32(Name, Value);
            return Result == EPropertyBagResult::Success;
        }
        break;
    }
    case EPropertyBagPropertyType::UInt64:
    {
        int64 Temp = FCString::Atoi64(*StringValue);
        if (Temp >= 0)
        {
            uint64 Value = static_cast<uint64>(Temp);
            // 需要对应的 SetValueUInt64 函数
            EPropertyBagResult Result = PropertyBag.SetValueUInt64(Name, Value);
            return Result == EPropertyBagResult::Success;
        }
        break;
    }
    default:
        UE_LOG(LogTemp, Warning, TEXT("不支持从字符串转换到类型: %d"), static_cast<uint8>(Type));
        break;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("无法将字符串 '%s' 转换为类型 %d"), *StringValue, static_cast<uint8>(Type));
    return false;
}
	
};
