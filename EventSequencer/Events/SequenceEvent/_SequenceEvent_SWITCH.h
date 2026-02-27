#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_SWITCH.generated.h"


USTRUCT(Blueprintable)
struct FEventCase
{
	GENERATED_BODY()

	int CaseEventIndex;
	
	// 比较值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString ComparisonValue;

	// 条件为真时执行的事件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SwitchCase Branch",
		meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent", TitleProperty = "EventTitle"))
	TArray<FEventWrapper> CaseEvents;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SWITCH")
	bool AutoBreak = true;
};



USTRUCT(BlueprintType, meta = (DisplayName = "SWITCH"))
struct F_SequenceEvent_SWITCH : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	F_SequenceEvent_SWITCH(){}

	virtual FString GetDisplayName() const override
	{
		FString StartIndexString = FString::Printf(TEXT("%03d"), StartIndex);
		FString EndIndexString = FString::Printf(TEXT("%03d"), EndIndex - 1);

		return "SWITCH " + PropertyName.ToString() + " [From " + StartIndexString + " To " + EndIndexString + "]";
	}
	virtual int GetEventsCount() override
	{
		int R = 0;
		for (FEventCase& Case : EventCases)
		{
			R += GetEventListEventsCount(Case.CaseEvents);
		}
		
		// SwitchCase 的处理包含哪些搭配 IF / GOTO 的优化？
		return Super::GetEventsCount() + R;
	}

	int StartIndex = -1;
	int EndIndex = -1;
	
	// Value To Events;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SWITCH",  meta = (TitleProperty = "ComparisonValue"))
	TArray<FEventCase> EventCases;
	
	// 条件属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SWITCH")
	FName PropertyName;



};
