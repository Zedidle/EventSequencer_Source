#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "SequenceEvent_LABEL.generated.h"

USTRUCT(BlueprintType)
struct FSequenceEvent_LABEL : public FBaseSequenceEvent
{
	GENERATED_BODY()
	
	FSequenceEvent_LABEL(){ Type = EEventType::LABEL; }
	
	// 标签名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Label")
	FName LabelName;
    
	// 标签描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Label")
	FString LabelDescription;

};