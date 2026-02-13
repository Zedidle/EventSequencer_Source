// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventAsyncObject.h"
#include "EventAsyncObjectWithCatch.generated.h"

/**
 * 
 */
UCLASS()
class EVENTSEQUENCER_API UEventAsyncObjectWithCatch : public UEventAsyncObject
{
	GENERATED_BODY()

public:
	// 创建带Catch的异步行为
	UFUNCTION(BlueprintCallable, Category = "Async", 
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", 
				DisplayName = "Create Async Action with Catch", AutoCreateRefTerm = "Context, CatchAction"))
	static UEventAsyncObjectWithCatch* CreateAsyncActionWithCatch(
		UObject* WorldContextObject, const TMap<FString, FString>& Context,
		TSubclassOf<UEventAsyncObject> CatchAction = nullptr,
		EInterruptBehavior InterruptBehavior = EInterruptBehavior::Restart);
    
	// 执行Catch块
	UFUNCTION(BlueprintCallable, Category = "Async")
	void ExecuteCatch(const FString& FailureReason);

	UFUNCTION()
	void OnCatchActionCompleted(EEventExecuteResult Result, const FString& Reason);

	// Catch块完成委托
	UPROPERTY(BlueprintAssignable, Category = "Async|Events")
	FAsyncCompleteDelegate OnCatchCompleted;


	    
protected:
	// 失败处理
	virtual void InternalComplete(EEventExecuteResult Result, const FString& Reason) override;
    
	// Catch块
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Async|Catch")
	TSubclassOf<UEventAsyncObject> CatchActionClass;
    
	UPROPERTY()
	UEventAsyncObject* CatchActionInstance = nullptr;
    
	// Catch块执行结果
	EEventExecuteResult CatchResult = EEventExecuteResult::None;
	FString CatchReason;

};
