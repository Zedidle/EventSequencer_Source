#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "_SequenceEvent_LOOP.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_BREAK.generated.h"

// 关于Break的Runtime解析，如果没有处于循环中，则不会加入到事件序列
USTRUCT(BlueprintType, meta = (DisplayName = "BREAK"))
struct F_SequenceEvent_BREAK : public FBaseSequenceEvent
{
	GENERATED_BODY()

	F_SequenceEvent_BREAK(){}

	virtual FString GetDisplayName() const override
	{
		if (InLoopEndIndex < 0)
		{
			return FString("");
		}
		
		if (Condition.PropertyName.IsNone())
		{
			// 显示为 跳出到循环的结尾。如果没有处于循环中，则跳过直接执行下一条。
			return "BREAK [Goto " + FString::Printf(TEXT("%03d"), InLoopEndIndex) + "]";
		}

		return "BREAK " + Condition.GetDisplayString() + " [Goto " + FString::Printf(TEXT("%03d"), InLoopEndIndex) + "]";

	}
	virtual int GetEventsCount() override
	{
		return Super::GetEventsCount();
	}

	int InLoopEndIndex = -1;
	
	// 循环退出的条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition Condition;
};