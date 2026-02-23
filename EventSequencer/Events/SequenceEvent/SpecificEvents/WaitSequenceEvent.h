#pragma once
#include "CoreMinimal.h"
#include "EventSequencer/Events/SequenceEvent/CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "WaitSequenceEvent.generated.h"

USTRUCT(BlueprintType)
struct FWaitSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Duration = 1.0f;
	
};

// 等待事件
USTRUCT(BlueprintType)
struct FWaitSequenceEvent : public FBaseSequenceEvent
{
	GENERATED_BODY()
    
	FWaitSequenceEvent() {}
	virtual FString GetDisplayName() const override;
	virtual int GetEventsCount() override;
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual float Tick(float DeltaTime, float PreRemainTime = 0) override;
	virtual void OnFinished() override;
	
	UPROPERTY(EditAnywhere)
	FWaitSequenceProperty Property;

	float CurTime = 0;
};