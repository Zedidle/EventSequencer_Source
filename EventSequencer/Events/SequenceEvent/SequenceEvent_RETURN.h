#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_RETURN.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_RETURN : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	// 返回值（可选）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Return")
	FString ReturnValue;
	
};