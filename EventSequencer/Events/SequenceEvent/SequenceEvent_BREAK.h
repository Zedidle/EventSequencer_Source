#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_BREAK.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_BREAK : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	// 跳出循环的层数（默认1，跳出最近一层）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Break", meta = (ClampMin = "1"))
	int32 BreakDepth = 1;

};