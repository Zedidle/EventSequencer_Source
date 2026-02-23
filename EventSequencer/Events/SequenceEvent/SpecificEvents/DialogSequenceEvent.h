#pragma once
#include "CoreMinimal.h"
#include "EventSequencer/Events/SequenceEvent/CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "DialogSequenceEvent.generated.h"


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
struct FDialogSequenceEvent : public FNestedSequenceEvent
{
	GENERATED_BODY()
    
	FDialogSequenceEvent() {}
	virtual FString GetDisplayName() const override;
	virtual int GetEventsCount() override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual float Tick(float DeltaTime, float PreRemainTime) override;

	
	UPROPERTY(EditAnywhere)
	FDialogSequenceProperty Property;
	
	int32 CurrentLineIndex = 0;
	float CurrentCharIndex = 0;
};