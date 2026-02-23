#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_LOOP.generated.h"

struct F_SequenceEvent_LOOP;


USTRUCT(BlueprintType)
struct FEventState_LOOP
{
	GENERATED_BODY()

	F_SequenceEvent_LOOP* LoopEventSelf;
	
	// 最大循环次数（0表示无限循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop", meta = (ClampMin = "0"))
	int32 MaxLoopTimes = 0;
    
	// 当前迭代计数（运行时）
	int32 CurLoopTimes = 0;
	// 循环体开始位置（初始扁平化处理序列时）
	int32 LoopStartIndex = 0;
	// 循环结束位置（初始扁平化处理序列时设定）
	int32 LoopEndIndex = 0;    
};


USTRUCT(BlueprintType, meta = (DisplayName = "LOOP"))
struct F_SequenceEvent_LOOP: public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	F_SequenceEvent_LOOP()
	{
		State.LoopEventSelf = this;
	}
	virtual FString GetDisplayName() const override
	{
		FString StartIndex = FString::Printf(TEXT("%03d"), State.LoopStartIndex);
		FString EndIndex = FString::Printf(TEXT("%03d"), State.LoopEndIndex);
		
		return "LOOP [Condition: " + Condition.GetDisplayString()  + " ] [From " + StartIndex + " To " + EndIndex + "]";
	}
	virtual int GetEventsCount() override
	{
		return Super::GetEventsCount() + GetEventListEventsCount(LoopEvents);
	}

	// 循环条件（可选，为空表示无条件循环）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition Condition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop Body", meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> LoopEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop Body")
	FEventState_LOOP State;
};
