// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../UI/ChoiceWidget.h"
#include "Commandlets/GatherTextFromSourceCommandlet.h"
#include "UObject/Object.h"
#include "CommonStructs.generated.h"


// 条件操作符枚举
UENUM(BlueprintType)
enum class ESequenceConditionOperator : uint8
{
	Equal,          // ==
	NotEqual,       // !=
	Greater,        // >
	GreaterEqual,   // >=
	Less,           // <
	LessEqual,      // <=
	Contains,       // 字符串包含
	StartsWith,     // 字符串以...开始
	EndsWith        // 字符串以...结束
};

// 条件定义
USTRUCT(BlueprintType)
struct FSequenceCondition
{
	GENERATED_BODY()

	FString GetDisplayString() const
	{
		FString OperatorString;
		switch (Operator)
		{
		case ESequenceConditionOperator::Equal: 
			OperatorString = "=="; 
			break;
		case ESequenceConditionOperator::NotEqual: 
			OperatorString = "!="; 
			break;
		case ESequenceConditionOperator::Greater: 
			OperatorString = ">"; 
			break;
		case ESequenceConditionOperator::GreaterEqual: 
			OperatorString = ">="; 
			break;
		case ESequenceConditionOperator::Less: 
			OperatorString = "<"; 
			break;
		case ESequenceConditionOperator::LessEqual: 
			OperatorString = "<="; 
			break;
		case ESequenceConditionOperator::Contains: 
			OperatorString = "Contains"; 
			break;
		case ESequenceConditionOperator::StartsWith: 
			OperatorString = "StartsWith"; 
			break;
		case ESequenceConditionOperator::EndsWith: 
			OperatorString = "EndsWith"; 
			break;
		default: 
			// 兜底处理，避免空字符串（可选，增强鲁棒性）
			OperatorString = "Unknown"; 
			break;
		}
		
		return "[Condition " + PropertyName.ToString() + " " + OperatorString + " " + ComparisonValue + "]";
	}
	
	// 条件属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName PropertyName;
    
	// 比较操作符
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	ESequenceConditionOperator Operator = ESequenceConditionOperator::Equal;
    
	// 比较值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString ComparisonValue;
};



UCLASS()
class EVENTSEQUENCER_API UCommonStructs : public UObject
{
	GENERATED_BODY()
};

// 事件状态枚举
UENUM(BlueprintType)
enum class EEventState : uint8
{
	Pending,    // 等待中
	Active,     // 进行中
	CurFinished, // 当前已完成，准备切换回Pending，并让事件序列执行下一事件
};
 
// 事件基类结构体
USTRUCT(BlueprintType, meta = (DisplayName = "——————————————————"))
struct FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FBaseSequenceEvent(){}
	virtual ~FBaseSequenceEvent() = default;

	virtual FString GetDisplayName() const { return ""; }
	virtual int GetEventsCount() { return 1; }
	static int GetEventListEventsCount(TArray<FInstancedStruct>& Events)
	{
		if (Events.IsEmpty()) return 0;
		
		int Result = 0;
		for (auto& Event : Events)
		{
			if (FBaseSequenceEvent* EventPtr = Event.GetMutablePtr<FBaseSequenceEvent>())
			{
				Result += EventPtr->GetEventsCount();		
			}
		}
		return Result;
	}
	
	UPROPERTY()
	TWeakObjectPtr<AActor> ContextActor;

	AActor* GetContext() { return ContextActor.Get(); }
	
	// 事件生命周期方法
	virtual void OnEnter() { State = EEventState::Active; }
	virtual float Tick(float DeltaTime, float PreRemainTime = 0) { return 0; }
	
	// 需要主动执行 Execute()，如果执行后状态为 Completed, 则返回true
	// 例如主动触发对话选择之类的
	virtual bool Execute(int Index = 0) { return true; }

	virtual void OnFinished() { State = EEventState::Pending; }
    
	void SetState(EEventState NewState) { State = NewState; }
	EEventState GetState() const { return State; }
	
protected:
	EEventState State = EEventState::Pending;
};



// 事件基类结构体
USTRUCT(BlueprintType)
struct FNestedSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nested Events", meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> NestedEvents;

	virtual int GetEventsCount() { return GetEventListEventsCount(NestedEvents); }
	
};


