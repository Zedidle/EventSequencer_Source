// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../UI/ChoiceWidget.h"
#include "UObject/Object.h"
#include "CommonStructs.generated.h"





UCLASS()
class EVENTSEQUENCER_API UCommonStructs : public UObject
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EEventType : uint8
{
	None        UMETA(DisplayName = "None"),
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
	
	UPROPERTY(EditDefaultsOnly)
	FName Label = FName();

	virtual FString GetDisplayName() const
	{
		if (!Label.IsNone())
		{
			return "[Label: " + Label.ToString() + "] " ;
		}
		return "";
	}
	
	UPROPERTY()
	TWeakObjectPtr<AActor> ContextActor;

	AActor* GetContext()
	{
		return ContextActor.Get();
	}
	
	// 核心：事件类型枚举，用于在编辑器中进行选择
	UPROPERTY(BlueprintReadOnly)
	EEventType Type = EEventType::None;

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




USTRUCT(BlueprintType)
struct FMoveSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere)
	float ApproachDistance = 10;
	
	// UPROPERTY(EditAnywhere, Category = "Event", Meta = (EditCondition = "Type == EEventType::NPCMovement"))
	// float MoveSpeed = 300.0f;
};

// 具体事件
// 移动事件
USTRUCT(BlueprintType)
struct FMoveSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FMoveSequenceEvent(){ Type = EEventType::Move;}
	virtual FString GetDisplayName() const override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FMoveSequenceProperty Property; 
	
	bool bMoving = false;    
	
	// NPCPawn 的设置需要在某个全局的登场NPC上？ 或者玩家当前指定的Active目标上获取
	UPROPERTY()
	APawn* NPCPawn = nullptr;
	FVector StartLocation;
};


USTRUCT(BlueprintType)
struct FDialogSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FString> DialogLines;
	
	UPROPERTY(EditAnywhere)
	float TextDisplaySpeed = 0.05f;

	// UPROPERTY(EditAnywhere)
	// TArray<FDialogItem> DialogItems;

	// 建议后续传入一个对话ID即可，在Enter时读取对话列表到这里。
	// 可能不是 FString， 每个对话的结构 FDialogItem 如下：
	// {
	// 	bPlayer : 是否为角色的话
	// 	Name - 名称
	// 	Content - 所说的话。
	// }
};

// 对话事件
USTRUCT(BlueprintType)
struct FDialogSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FDialogSequenceEvent() { Type = EEventType::Dialog; }
	virtual FString GetDisplayName() const override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere)
	FDialogSequenceProperty Property;
	
	int32 CurrentLineIndex = 0;
	float CurrentCharIndex = 0;
};

USTRUCT(BlueprintType)
struct FWaitSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Duration = 1.0f;
	
};

// 等待事件
USTRUCT(BlueprintType)
struct FWaitSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FWaitSequenceEvent() { Type = EEventType::Wait; }
	virtual FString GetDisplayName() const override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnExit() override;
	
	UPROPERTY(EditAnywhere)
	FWaitSequenceProperty Property;

	float CurTime = 0;
};


USTRUCT(BlueprintType)
struct FChoiceSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FString> Choices;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UChoiceWidget> ChoiceWidgetClass;
	
};

USTRUCT(BlueprintType)
struct FChoiceSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
	
	FChoiceSequenceEvent() { Type = EEventType::Choice; }
	virtual FString GetDisplayName() const override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnExit() override;

	
	UPROPERTY(EditAnywhere)
	FChoiceSequenceProperty Property;
	
	UPROPERTY()
	TWeakObjectPtr<APawn> PlayerPawn;

	UPROPERTY()
	UChoiceWidget* ChoiceWidget;
};