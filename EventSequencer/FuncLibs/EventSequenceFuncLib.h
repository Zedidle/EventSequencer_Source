// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Events/SequenceEvent/CommonStructs.h"
#include "EventSequencer/Events/EventSequenceRunning.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "EventSequenceFuncLib.generated.h"


struct FEventSequenceRunning;
class UEventSequenceDA;

UCLASS()
class EVENTSEQUENCER_API UEventSequenceFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 创建NPC移动事件
	UFUNCTION(BlueprintCallable, Category = "Event Factory")
	static FInstancedStruct CreateMoveEvent(const FMoveSequenceProperty& Property);

	// 创建对话事件
	UFUNCTION(BlueprintCallable, Category = "Event Factory")  
	static FInstancedStruct CreateDialogEvent(const FDialogSequenceProperty& Property);

	// 创建等待事件
	UFUNCTION(BlueprintCallable, Category = "Event Factory")
	static FInstancedStruct CreateWaitEvent(const FWaitSequenceProperty& Property);

	// 创建等待事件
	UFUNCTION(BlueprintCallable, Category = "Event Factory")
	static FInstancedStruct CreateChoiceEvent(const FChoiceSequenceProperty& Property);

	
	UFUNCTION(BlueprintCallable, Category = "Event Factory", meta =(WorldContext = "WorldContextObject"))
	static UEventSequenceRunning* CreateEventSequence(UObject* WorldContextObject, UEventSequenceDA* TargetDataAsset);
	
	UFUNCTION(BlueprintCallable, Category = "Event Factory", meta =(WorldContext = "WorldContextObject"))
	static bool RemoveEventSequence(UObject* WorldContextObject, UEventSequenceRunning* EventSequence);
	
};
