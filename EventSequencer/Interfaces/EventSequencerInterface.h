// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EventSequencerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEventSequencerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EVENTSEQUENCER_API IEventSequencerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	void OnChoice(int Index);  
    
	
};
