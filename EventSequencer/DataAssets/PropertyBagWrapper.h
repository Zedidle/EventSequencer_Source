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
	
public:
    // 构造函数
    UPropertyBagWrapper();

    // 属性操作接口
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddProperty(const FName& Name, const FString& TypeString);
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueInt(const FName& Name, int32 Value);
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueFloat(const FName& Name, float Value);
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueString(const FName& Name, const FString& Value);

    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueBool(const FName& Name, bool Value);
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueVector(const FName& Name, const FVector& Value);
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueObject(const FName& Name, UObject* Value);
    
    // 获取值接口（返回默认值如果失败）
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    int32 GetValueInt(const FName& Name) const;

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    float GetValueFloat(const FName& Name) const;
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    FString GetValueString(const FName& Name) const;
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool GetValueBool(const FName& Name) const;
    //
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // FVector GetValueVector(const FName& Name) const;
    //
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // UObject* GetValueObject(const FName& Name) const;
    //
    // // 类型检查与验证
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // bool HasProperty(const FName& Name) const;
    //
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // FString GetPropertyType(const FName& Name) const;
    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool MergeFrom(UPropertyBagWrapper* Other, bool bOverrideExisting = true);
    
    // 清空所有属性
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    void ClearAllProperties();

    
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    void PrintAllProperties();
    
    // // 获取所有属性名
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // TArray<FName> GetAllPropertyNames() const;
    //
    // // 序列化支持
    // virtual void Serialize(FArchive& Ar) override;
    
protected:
    // 内部存储
    UPROPERTY()
    FInstancedPropertyBag PropertyBag;
    
private:
    // 类型字符串到枚举的映射
    EPropertyBagPropertyType GetTypeFromString(const FString& TypeString) const;
    
    // 错误处理
    void LogError(const FString& Message) const;
};
