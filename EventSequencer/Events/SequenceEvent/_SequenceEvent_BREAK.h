#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "_SequenceEvent_BREAK.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "BREAK"))
struct F_SequenceEvent_BREAK : public FBaseSequenceEvent
{
	GENERATED_BODY()

	F_SequenceEvent_BREAK(){}

	virtual FString GetDisplayName() const override
	{
		return TEXT("BREAK");
	}
	virtual int GetEventsCount() override
	{
		return 1;
	}
	
	// 循环退出的条件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	FSequenceCondition Condition;
	
};