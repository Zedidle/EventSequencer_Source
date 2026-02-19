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

	FORCEINLINE FInstancedPropertyBag& GetPropertyBag() { return PropertyBag; }
    // FORCEINLINE const FInstancedPropertyBag& GetPropertyBag() const { return PropertyBag; }

	bool AddProperty(const FName& Name, const EPropertyBagPropertyType& Type);
	
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueBool(const FName& Name, bool Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueBool(const FName& Name, bool Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueByte(const FName& Name, uint8 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueByte(const FName& Name, uint8 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueInt(const FName& Name, int32 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueInt(const FName& Name, int32 Value);

    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // bool SetValueUInt32(const FName& Name, uint32 Value);
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // bool AddValueUInt32(const FName& Name, uint32 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueInt64(const FName& Name, int64 Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueInt64(const FName& Name, int64 Value);

    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // bool SetValueUInt64(const FName& Name, uint64 Value);
    // UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    // bool AddValueUInt64(const FName& Name, uint64 Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueFloat(const FName& Name, float Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueFloat(const FName& Name, float Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueDouble(const FName& Name, double Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueDouble(const FName& Name, double Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueName(const FName& Name, const FName& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueName(const FName& Name, const FName& Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueString(const FName& Name, const FString& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueString(const FName& Name, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueText(const FName& Name, const FText& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueText(const FName& Name, const FText& Value);
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool SetValueVector(const FName& Name, const FVector& Value);
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool AddValueVector(const FName& Name, const FVector& Value);
	
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueObject(const FName& Name, UObject* Value);
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueObject(const FName& Name, UObject* Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueClass(const FName& Name, UClass* Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueClass(const FName& Name, UClass* Value);

    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool SetValueSoftPath(const FName& Name, const FSoftObjectPath& Value);
    UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool AddValueSoftPath(const FName& Name, const FSoftObjectPath& Value);



	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	bool GetValueBool(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	uint8 GetValueByte(const FName& Name) const;
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	int32 GetValueInt(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	int64 GetValueInt64(const FName& Name) const;
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	float GetValueFloat(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	double GetValueDouble(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FName GetValueName(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FString GetValueString(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FText GetValueText(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FVector GetValueVector(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	UObject* GetValueObject(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	UClass* GetValueClass(const FName& Name) const;

	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
	FSoftObjectPath GetValueSoftPath(const FName& Name) const;
	
	
	UFUNCTION(BlueprintCallable, Category = "PropertyBag")
    bool MergeFrom(UPropertyBagWrapper* Other, bool bOverrideExisting = true);


protected:
    // 内部存储
    UPROPERTY()
    FInstancedPropertyBag PropertyBag;
};
