// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Interfaces/EventSequencerInterface.h"
#include "StructUtils/PropertyBag.h"
#include "UI/ChoiceWidget.h"
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
	
	UPROPERTY(EditAnywhere)
	FInstancedPropertyBag InstancedPropertyBag;
	
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
	
};

// 具体事件
// 移动事件
USTRUCT(BlueprintType)
struct FMoveSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FMoveSequenceEvent()
	{
		Type = EEventType::Move;
	}

	virtual FString GetDisplayName() const override
	{
		return Super::GetDisplayName() + FString::Printf(TEXT("Move ApproachDistance: %f"), Property.ApproachDistance);
	}
	
	bool bMoving = false;    


	// UPROPERTY(EditAnywhere, Meta = (EditCondition = "Type == EEventType::Move"))
	// FInstancedPropertyBag InstancedPropertyBag;
	
	// UPROPERTY(EditAnywhere, Meta = (EditCondition = "Type == EEventType::Move"))
	UPROPERTY(EditAnywhere)
	FMoveSequenceProperty Property; 

    
	// UPROPERTY(EditAnywhere, Category = "Event", Meta = (EditCondition = "Type == EEventType::NPCMovement"))
	// float MoveSpeed = 300.0f;
    
	virtual void OnEnter() override
	{
		FBaseSequenceEvent::OnEnter();
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::OnEnter")));
		if (NPCPawn)
		{
			StartLocation = NPCPawn->GetActorLocation();
		}
	}

	virtual bool Execute(int Index = 0) override
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Next")));

		return true;
	}
	
	virtual void Tick(float DeltaTime) override
	{
		FBaseSequenceEvent::Tick(DeltaTime);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Tick")));

		if (!IsValid(NPCPawn))
		{
			State = EEventState::Completed;
			return;
		}
		
		FVector CurLocation = NPCPawn->GetActorLocation();
    
		if (FVector::Distance(CurLocation, Property.TargetLocation) < Property.ApproachDistance)
		{
			State = EEventState::Completed;
		}
		else if (!bMoving)
		{
			if (AController* Controller = NPCPawn->GetController())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Property.TargetLocation);
			}
			bMoving = true;
		}
	}
    
private:
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
	
};

// 对话事件
USTRUCT(BlueprintType)
struct FDialogSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FDialogSequenceEvent()
	{
		Type = EEventType::Dialog;
	}
	virtual FString GetDisplayName() const override
	{
		return Super::GetDisplayName() + FString::Printf(TEXT("Dialog Num: %d"), Property.DialogLines.Num());
	}
	// 建议后续传入一个对话ID即可，在Enter时读取对话列表到这里。
	// 可能不是 FString， 每个对话的结构 FDialogItem 如下：
	// {
	// 	bPlayer : 是否为角色的话
	// 	Name - 名称
	// 	Content - 所说的话。
	// }

	UPROPERTY(EditAnywhere)
	FDialogSequenceProperty Property;
	
	// UPROPERTY(EditAnywhere)
	// TArray<FDialogItem> DialogItems;
	
    
	virtual void OnEnter() override
	{
		FBaseSequenceEvent::OnEnter();

		// 读表把对话内容加入到DialogItems
		// xxxxx
		
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
	}
	
	virtual bool Execute(int Index = 0) override
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Next")));
		
		return true;
	}
	
	virtual void Tick(float DeltaTime) override
	{
		FBaseSequenceEvent::Tick(DeltaTime);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::Tick")));
	}
    
private:
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
    
	FWaitSequenceEvent()
	{
		Type = EEventType::Wait;
	}
	virtual FString GetDisplayName() const override
	{
		return Super::GetDisplayName() + FString::Printf(TEXT("Wait For: %f"), Property.Duration);
	}

	UPROPERTY(EditAnywhere)
	FWaitSequenceProperty Property;
	
	float CurTime = 0;
    
	virtual void OnEnter() override
	{
		FBaseSequenceEvent::OnEnter();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FDialogSequenceEvent::OnEnter")));
	}

	virtual bool Execute(int Index = 0) override
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Next")));
		OnExit();
		return true;
	}
	
	virtual void Tick(float DeltaTime) override
	{
		CurTime += DeltaTime;
		if (CurTime >= Property.Duration)
		{
			Execute();
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FWaitSequenceEvent::Tick Waiting")));
	}

	virtual void OnExit() override
	{
		FBaseSequenceEvent::OnExit();
	}
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
	
	FChoiceSequenceEvent()
	{
		Type = EEventType::Choice;
	}
	
	virtual FString GetDisplayName() const override
	{
		return Super::GetDisplayName() + FString::Printf(TEXT("Choice Choices Num: %d"), Property.Choices.Num());
	}
	
	UPROPERTY()
	TWeakObjectPtr<APawn> PlayerPawn;

	UPROPERTY(EditAnywhere)
	FChoiceSequenceProperty Property;
	
	UPROPERTY()
	UChoiceWidget* ChoiceWidget;
	
	virtual void OnEnter() override
	{
		FBaseSequenceEvent::OnEnter();

		// 针对对话所用的UMG生成对话选项界面
		if (Property.ChoiceWidgetClass)
		{
			// CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
			// 需要临时给每个选项绑定操作
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::OnEnter")));
	}

	virtual bool Execute(int Index = 0) override
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::Next")));

		if (ChoiceWidget)
		{
			// ChoiceWidget->MakeChoice(Index);
			if (PlayerPawn->Implements<UEventSequencerInterface>())
			{
				// 可以在 IEventSequencerInterface 自定义接口传入任意参数影响Actor自身
				IEventSequencerInterface::Execute_OnChoice(PlayerPawn.Get(), Index);
			}
		}
		
		return true;
	}
	
	virtual void Tick(float DeltaTime) override
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("FChoiceSequenceEvent::Tick Waiting")));
	}

	virtual void OnExit() override
	{
		FBaseSequenceEvent::OnExit();
	}
};