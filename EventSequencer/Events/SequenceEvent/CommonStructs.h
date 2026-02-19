// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../UI/ChoiceWidget.h"
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

UENUM(BlueprintType)
enum class EEventType : uint8
{
	None        UMETA(DisplayName = "None"),

	// 控制流事件
	IF,
	LOOP,
	BREAK,
	RETURN,
	LABEL,
	GOTO,
	
	// 事件类型预设
	Move    UMETA(DisplayName = "Move"), 
	Dialog    UMETA(DisplayName = "Dialog"),
	Wait UMETA(DisplayName = "Wait"),
	Choice UMETA(DisplayName = "Choice")
	// ... 其他类型
};

// 事件状态枚举
UENUM(BlueprintType)
enum class EEventState : uint8
{
	Idle,		// 未就绪
	Pending,    // 等待中
	Active,     // 进行中
	Completed   // 已完成
};
 
// 事件基类结构体
USTRUCT(BlueprintType, meta = (TitleProperty = "Title"))
struct FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FBaseSequenceEvent(){}
	virtual ~FBaseSequenceEvent() = default;

	virtual FString GetDisplayName() const { return ""; }
	
	UPROPERTY()
	TWeakObjectPtr<AActor> ContextActor;

	AActor* GetContext() { return ContextActor.Get(); }
	
	// 核心：事件类型枚举，用于在编辑器中进行选择
	UPROPERTY(BlueprintReadOnly)
	EEventType Type = EEventType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event Sequence", meta = (ShowOnlyInnerProperties, BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> EventSequence;
	
	virtual void OnPending() { State = EEventState::Pending; }
	// 事件生命周期方法
	virtual void OnEnter() { State = EEventState::Active; }
	virtual void Tick(float DeltaTime) {}
	
	// 需要主动执行 Execute()，如果执行后状态为 Completed, 则返回true
	// 例如主动触发对话选择之类的
	virtual bool Execute(int Index = 0) { return true; }

	virtual void OnExit() { State = EEventState::Completed; }
    
	void SetState(EEventState NewState) { State = NewState; }
	EEventState GetState() const { return State; }
	EEventType GetEventType() const { return Type; }


	
protected:
	EEventState State = EEventState::Pending;
};






