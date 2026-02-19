#pragma once
#include "CoreMinimal.h"
#include "EventSequencer/Events/SequenceEvent/CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "MoveSequenceEvent.generated.h"

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
