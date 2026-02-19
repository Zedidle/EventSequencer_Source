// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceEvent/CommonStructs.h"
#include "UObject/Object.h"
#include "EventSyncObject.generated.h"



// 无法执行下面的 MakeSyncEvent，建议直接通过FuncLib的 AddXXXXEvent 来同步创建

UCLASS(Blueprintable, MinimalAPI)
class UEventSyncObject : public UObject
{
	// GENERATED_BODY()
	GENERATED_UCLASS_BODY()

    UPROPERTY(EditDefaultsOnly)
    EEventType EventType = EEventType::None;
    
public:

    UFUNCTION(BlueprintCallable, Category = "Event Sync", meta = (BlueprintInternalUseOnly = "true"))
    static EVENTSEQUENCER_API UEventSyncObject* MakeSyncEvent(EEventType Type);

    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Move", EditConditionHides))
    FVector TargetLocation;
    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Move", EditConditionHides))
    float ApproachDistance = 10;

    
    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Choice", EditConditionHides))
    TArray<FString> Choices;

    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Choice", EditConditionHides))
    TSubclassOf<UChoiceWidget> ChoiceWidgetClass;


    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Dialog", EditConditionHides))
    TArray<FString> DialogLines;
    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Dialog", EditConditionHides))
    float TextDisplaySpeed = 0.05f;

    
    UPROPERTY(EditAnywhere, Meta = (EditCondition = "EventType == EEventType::Wait", EditConditionHides))
    float WaitDuration = 1.0f;
    
protected:
    // 在蓝图中可重写的函数
    UFUNCTION(BlueprintImplementableEvent, Category = "Interpreter|SyncAction")
    void OnExecute();
    
private:
    bool bIsInitialized = false;

};
