#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_IF.generated.h"


USTRUCT(BlueprintType, meta = (DisplayName = "IF"))
struct F_SequenceEvent_IF : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	F_SequenceEvent_IF(){}
	virtual FString GetDisplayName() const override
	{
		FString TrueStartIndex = FString::Printf(TEXT("%03d"), TrueEventsStartIndex);
    	FString FalseStartIndex = FString::Printf(TEXT("%03d"), FalseEventsStartIndex);
		
		return "IF [Condition: " + Condition.GetDisplayString()  + " ] [True: " + TrueStartIndex + ", False: " + FalseStartIndex + "]";
	}

	virtual int GetEventsCount() override
	{
		int TrueEventsCount = GetEventListEventsCount(TrueEvents);
		int FalseEventsCount = GetEventListEventsCount(FalseEvents);
		
		// 1 包括了内部构造的GOTO
		return Super::GetEventsCount() + TrueEventsCount + FalseEventsCount + 1;
	}
	
	// True事件开始下标
	int TrueEventsStartIndex = -1;
	// False事件开始下标
	int FalseEventsStartIndex = -1;
	// 条件分支结束下标
	int EndIndex = -1;
	
	// 条件定义
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FSequenceCondition Condition;

	// 条件为真时执行的事件序列
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | IF Branch",
			  meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> TrueEvents;
    
	// 条件为假时执行的事件序列
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | IF Branch",
			  meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> FalseEvents;
};
