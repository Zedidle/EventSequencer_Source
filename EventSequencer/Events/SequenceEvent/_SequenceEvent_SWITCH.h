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
		meta = (BaseStruct = "/Script/EventSequencer.BaseSequenceEvent"))
	TArray<FInstancedStruct> CaseEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SWITCH")
	bool AutoBreak = true;
};



USTRUCT(BlueprintType, meta = (DisplayName = "SWITCH"))
struct F_SequenceEvent_SWITCH : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	F_SequenceEvent_SWITCH(){}

	int EndIndex;
	
	// Value To Events;
	TArray<FEventCase> EventCases;
	
	// 条件属性名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SequenceEvent | SWITCH")
	FName PropertyName;



};
