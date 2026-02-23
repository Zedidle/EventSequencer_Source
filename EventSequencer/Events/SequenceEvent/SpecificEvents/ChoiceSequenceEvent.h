#pragma once
#include "CoreMinimal.h"
#include "EventSequencer/Events/SequenceEvent/CommonStructs.h"
#include "StructUtils/PropertyBag.h"
#include "ChoiceSequenceEvent.generated.h"

USTRUCT(BlueprintType)
struct FChoiceSequenceProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FString> Choices;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UChoiceWidget> ChoiceWidgetClass;
	
};

USTRUCT(BlueprintType)
struct FChoiceSequenceEvent : public FNestedSequenceEvent
{
	GENERATED_BODY()
	
	FChoiceSequenceEvent() {}
	virtual FString GetDisplayName() const override;
	virtual int GetEventsCount() override;
	
	virtual void OnEnter() override;
	virtual bool Execute(int Index = 0) override;
	virtual float Tick(float DeltaTime, float PreRemainTime) override;
	virtual void OnFinished() override;

	
	UPROPERTY(EditAnywhere)
	FChoiceSequenceProperty Property;
	
	UPROPERTY()
	TWeakObjectPtr<APawn> PlayerPawn;

	UPROPERTY()
	UChoiceWidget* ChoiceWidget;
};