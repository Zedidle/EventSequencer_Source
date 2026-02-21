// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventSequencer/DataAssets/EventSequenceDA.h"
#include "SequenceEvent/CommonStructs.h"
#include "SequenceEvent/SequenceEvent_LOOP.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "EventSequenceRunning.generated.h"







UCLASS(BlueprintType)
class EVENTSEQUENCER_API UEventSequenceRunning : public UObject
{
	GENERATED_BODY()

	bool EvaluateCondition(const FSequenceCondition& Condition);
	bool CompareBool(bool Left, bool Right, ESequenceConditionOperator Op);
	bool CompareInt32(int32 Left, int32 Right, ESequenceConditionOperator Op);
	bool CompareFloat(const float& Left, const float& Right, ESequenceConditionOperator Op);
	bool CompareDouble(const double& Left, const double& Right, ESequenceConditionOperator Op);
	bool CompareString(const FString& Left, const FString& Right, ESequenceConditionOperator Op);
	bool CompareName(const FName& Left, const FName& Right, ESequenceConditionOperator Op);
	bool CompareText(const FText& Left, const FText& Right, ESequenceConditionOperator Op);

	bool TryParseBool(const FString& StringValue, bool& OutValue);
	bool TryParseInt32(const FString& StringValue, int32& OutValue);
	bool TryParseFloat(const FString& StringValue, float& OutValue);
	bool TryParseDouble(const FString& StringValue, double& OutValue);
	bool TryParseString(const FString& StringValue, FString& OutValue);
	bool TryParseName(const FString& StringValue, FName& OutValue);
	bool TryParseText(const FString& StringValue, FText& OutValue);
	
public:
	// 可能某些Event标记有Label，则需要记录下来方便跳转
	TMap<FName, int> Label2Index;
	
	bool bPause = false;
	int CurEventIndex = 0;
	TArray<FInstancedStruct> EventQueue;

	int GetEventsNum() { return EventQueue.Num(); }
	
	// 当前循环状态栈
	TArray<FEventState_LOOP> LoopStateStack;
	
	// 初始化定义的DA，存储了 InstancedPropertyBag 数据
	UPROPERTY()
	UEventSequenceDA* InitDataAsset;
	FInstancedPropertyBag PropertyBagRuntime;
	
	UFUNCTION()
	void SetDataAsset(UEventSequenceDA* DataAsset);




	void Tick(float DeltaTime);
	
	void AddEvent(FInstancedStruct& Event);
	void AppendEvents(TArray<FInstancedStruct>& Events);
	
	bool IsCompleted() const;
    
	FInstancedStruct& GetCurEvent();

	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void AddLabel(FName Label);
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void GOTO(FName Label);
	void GOTO(int Index);
	
	void LOOP(const FEventState_LOOP& LoopState);
	void BREAK();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Start();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Stop();
	
	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Next();

	UFUNCTION(BlueprintCallable, Category = "Event Sequence")
	void Exit();
	
	void Destroy();
	
};
