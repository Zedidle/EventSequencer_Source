// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSequenceSystem.h"

#include "AudioMixerBlueprintLibrary.h"
#include "DataAssets/PropertyBagWrapper.h"
#include "UObject/UnrealType.h"
#include "Engine/World.h"
#include "Events/EventSequenceBlueprintAction.h"
#include "Events/SequenceEvent/CommonStructs.h"
#include "Events/SequenceEvent/_SequenceEvent_BREAK.h"
#include "Events/SequenceEvent/_SequenceEvent_GOTO.h"
#include "Events/SequenceEvent/_SequenceEvent_IF.h"
#include "Events/SequenceEvent/_SequenceEvent_LABEL.h"
#include "Events/SequenceEvent/_SequenceEvent_LOOP.h"
#include "Events/SequenceEvent/_SequenceEvent_RETURN.h"
#include "Events/SequenceEvent/_SequenceEvent_SWITCH.h"
#include "Events/SequenceEvent/SpecificEvents/ChoiceSequenceEvent.h"
#include "Events/SequenceEvent/SpecificEvents/DialogSequenceEvent.h"
#include "Events/SequenceEvent/SpecificEvents/MoveSequenceEvent.h"
#include "Events/SequenceEvent/SpecificEvents/WaitSequenceEvent.h"

class AAIController;


void UEventSequenceSystem::ParseEventSequence(UEventSequenceRunning* EventSequenceRunning, const TArray<FEventWrapper>& EventWrappers)
{
    if (!EventSequenceRunning) return;
    if (EventWrappers.IsEmpty()) return;
    
    for (int32 i = 0; i < EventWrappers.Num(); ++i)
    {
        if (!EventWrappers.IsValidIndex(i)) continue;

        const FInstancedStruct& SourceEventStruct = EventWrappers[i].Event;
        FInstancedStruct RuntimeEventStruct = SourceEventStruct;
        
        if (F_SequenceEvent_LABEL* Event_LABEL = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_LABEL>())
        {
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
            EventSequenceRunning->AddLabel(Event_LABEL->LabelName);
        }
        else if (F_SequenceEvent_GOTO* Event_GOTO = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_GOTO>())
        {
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
        }
        else if (F_SequenceEvent_IF* Event_IF = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_IF>())
        {
            // 初步分析 TrueEvents 和 FalseEvents 开始的下标 以及 跳出的下标
            Event_IF->TrueEventsStartIndex = EventSequenceRunning->GetEventsNum() + 1;
            Event_IF->FalseEventsStartIndex = Event_IF->TrueEventsStartIndex + 
                                FBaseSequenceEvent::GetEventListEventsCount(Event_IF->TrueEvents) + 1;
            Event_IF->EndIndex = Event_IF->FalseEventsStartIndex + FBaseSequenceEvent::GetEventListEventsCount(Event_IF->FalseEvents);
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
            ParseEventSequence(EventSequenceRunning, Event_IF->TrueEvents);
            
            FInstancedStruct EventStruct_GOTO;
            EventStruct_GOTO.InitializeAs<F_SequenceEvent_GOTO>(F_SequenceEvent_GOTO(Event_IF->EndIndex));
            EventSequenceRunning->AddEvent(EventStruct_GOTO);
            
            ParseEventSequence(EventSequenceRunning, Event_IF->FalseEvents);
        }
        else if (F_SequenceEvent_SWITCH* Event_SWITCH = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_SWITCH>())
        {
            Event_SWITCH->StartIndex = EventSequenceRunning->GetEventsNum() + 1;
            Event_SWITCH->EndIndex = Event_SWITCH->StartIndex + Event_SWITCH->GetEventsCount() - 1;
            EventSequenceRunning->AddEvent(RuntimeEventStruct);

            FInstancedStruct& LastSwitchStruct = EventSequenceRunning->EventQueue.Last();
            F_SequenceEvent_SWITCH* Event_SWITCH_LAST = LastSwitchStruct.GetMutablePtr<F_SequenceEvent_SWITCH>();

            for (FEventCase& CaseEvent : Event_SWITCH_LAST->EventCases)
            {
                CaseEvent.CaseEventIndex = EventSequenceRunning->GetEventsNum();
                ParseEventSequence(EventSequenceRunning, CaseEvent.CaseEvents);
                
                if (CaseEvent.AutoBreak)
                {
                    FInstancedStruct EventStruct_GOTO;
                    EventStruct_GOTO.InitializeAs<F_SequenceEvent_GOTO>(F_SequenceEvent_GOTO(Event_SWITCH->EndIndex));
                    EventSequenceRunning->AddEvent(EventStruct_GOTO);
                }
            }
        }
        else if (F_SequenceEvent_LOOP* Event_LOOP = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_LOOP>())
        {
            // 关于Loop事件，由于解析的平扁化处理，需要记录下它在 EventSequenceRunning 的序列开始下标和结束下标，以便循环和Break跳出。
            Event_LOOP->State.LoopStartIndex = EventSequenceRunning->GetEventsNum() + 1;
            Event_LOOP->State.LoopEndIndex = Event_LOOP->State.LoopStartIndex
                                        + FBaseSequenceEvent::GetEventListEventsCount(Event_LOOP->LoopEvents);
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
            
            EventSequenceRunning->LoopStateStack.Push(Event_LOOP->State);
            ParseEventSequence(EventSequenceRunning, Event_LOOP->LoopEvents);
            
            FInstancedStruct EventStruct_GOTO;
            EventStruct_GOTO.InitializeAs<F_SequenceEvent_GOTO>(F_SequenceEvent_GOTO(Event_LOOP->State.LoopStartIndex - 1));
            EventSequenceRunning->AddEvent(EventStruct_GOTO);
            
            EventSequenceRunning->LoopStateStack.Pop();
            
        }
        else if (F_SequenceEvent_BREAK* Event_BREAK = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_BREAK>())
        {
            if (!EventSequenceRunning->LoopStateStack.IsEmpty())
            {
                const FEventState_LOOP& State_LOOP = EventSequenceRunning->LoopStateStack.Last();
                Event_BREAK->InLoopEndIndex = State_LOOP.LoopEndIndex + 1;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
            }
        }
        else if (F_SequenceEvent_RETURN* Event_RETURN = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_RETURN>())
        {
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
        }
        else if (FSequenceEvent_BlueprintCall* Event_BlueprintCall = RuntimeEventStruct.GetMutablePtr<FSequenceEvent_BlueprintCall>())
        {
            Event_BlueprintCall->SetEventSequenceRunning(EventSequenceRunning);
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
        }
        else if (FSequenceEvent_AsyncBlueprintCall* Event_AsyncBlueprintCall = RuntimeEventStruct.GetMutablePtr<FSequenceEvent_AsyncBlueprintCall>())
        {
            EventSequenceRunning->AddEvent(RuntimeEventStruct);
            // Event_AsyncBlueprintCall->SetEventSequenceRunning(EventSequenceRunning);
        }
        // 具体事件
        else
        {
            if (auto* DestEvent = RuntimeEventStruct.GetMutablePtr<FNestedSequenceEvent>())
            {
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
                ParseEventSequence(EventSequenceRunning, DestEvent->NestedEvents);
            }
        }
    }
}

UEventSequenceSystem* UEventSequenceSystem::GetInstance(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
	
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;
	
    return World->GetSubsystem<UEventSequenceSystem>();
}

void UEventSequenceSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TickSequences(DeltaTime);
}

TStatId UEventSequenceSystem::GetStatId() const
{
    return TStatId();  // 或其他安全的返回值
}

UEventSequenceRunning* UEventSequenceSystem::CreateEventSequence(UEventSequenceDA* TargetDataAsset, UPropertyBagWrapper* PropertyBagInput)
{
    if (UEventSequenceRunning* EventSequenceRunning = CreateEventSequenceRunning(TargetDataAsset, PropertyBagInput))
    {
        EventSequencesRunning.Add(EventSequenceRunning);
        return EventSequenceRunning;
    }
    return nullptr;
}

UEventSequenceRunning* UEventSequenceSystem::CreateEventSequenceWithComponent(UEventSequenceDA* TargetDataAsset,
    UPropertyBagWrapper* PropertyBagInput, UEventSequenceComponent* Component)
{
    if (!Component) return nullptr;
    
    if (UEventSequenceRunning* EventSequenceRunning = CreateEventSequenceRunning(TargetDataAsset, PropertyBagInput))
    {
        EventSequenceComponents.Add(Component);
        return EventSequenceRunning;
    }
    return nullptr;    
}

UEventSequenceRunning* UEventSequenceSystem::CreateEventSequenceRunning(UEventSequenceDA* TargetDataAsset,
    UPropertyBagWrapper* PropertyBagInput)
{
    if (!TargetDataAsset) return nullptr;
    if (TargetDataAsset->GetSequenceLength() == 0) return nullptr;
    
    
    UEventSequenceRunning* EventSequenceRunning = NewObject<UEventSequenceRunning>(this);
    EventSequenceRunning->SetDataAsset(TargetDataAsset, PropertyBagInput);
    ParseEventSequence(EventSequenceRunning, TargetDataAsset->EventWrappers);
    
    int N = 0;
    for (FInstancedStruct& EventStruct : EventSequenceRunning->EventQueue)
    {
        if (const FBaseSequenceEvent* Event = EventStruct.GetPtr<FBaseSequenceEvent>())
        {
            FString FormattedNum = FString::Printf(TEXT("%03d"), N++);
            UE_LOG(LogTemp, Log, TEXT("EventQueue: %s %s"), *FormattedNum, *Event->GetDisplayName());
        }
    }
    return EventSequenceRunning;
}


bool UEventSequenceSystem::RemoveEventSequence(UEventSequenceRunning* EventSequence)
{
    if (!EventSequence) return false;
    
    if (EventSequencesRunning.Contains(EventSequence))
    {
        EventSequencesExiting.Add(EventSequence);
        return true;
    }
    return false;
}

bool UEventSequenceSystem::RemoveComponent(const UEventSequenceComponent* Component)
{
    if (!Component) return false;

    for (int32 Index = 0; Index < EventSequenceComponents.Num(); ++Index)
    {
        TWeakObjectPtr<UEventSequenceComponent>& WeakCompPtr = EventSequenceComponents[Index];
        
        if (WeakCompPtr.IsValid() && WeakCompPtr.Get() == Component)
        {
            EventSequenceComponents.RemoveAt(Index);
            return true;
        }
    }

    return false;
}



void UEventSequenceSystem::TickSequences(float DeltaTime)
{
    if (EventSequencesRunning.IsEmpty()) return;
    
    if (!EventSequencesExiting.IsEmpty())
    {
        for (UEventSequenceRunning* EventSequence : EventSequencesExiting)
        {
            EventSequencesRunning.Remove(EventSequence);
        }
        EventSequencesExiting.Empty();
    }
    
    TArray<UEventSequenceRunning*> RemoveSequences;
    for (UEventSequenceRunning* SequencePtr : EventSequencesRunning)
    {
        // 务必检查指针有效性
        if (SequencePtr && SequencePtr->CurEventIndex != INDEX_NONE) 
        {
            // 通过指针直接操作原始数据
            SequencePtr->Tick(DeltaTime);
            
            // 完成后，移除
            if (SequencePtr->CurEventIndex == INDEX_NONE) 
            {
                RemoveSequences.Add(SequencePtr);
            }
        }
    }
    
    if (!RemoveSequences.IsEmpty())
    {
        EventSequencesRunning.RemoveAll([&](UEventSequenceRunning* Ptr)
        {
            return RemoveSequences.Contains(Ptr);
        });
    }
}


UEventSequenceRunning* UEventSequenceSystem::GetAsyncEventSequence(const FGuid& Guid)
{
    return AsyncEventSequenceMap.FindRef(Guid);
}

// 生成异步操作ID
FGuid UEventSequenceSystem::GenerateAsyncOperationID()
{
    FGuid NewID = FGuid::NewGuid();
    return NewID;
}

// 取消异步操作
// bool UEventSequenceSystem::CancelAsyncOperation(const FGuid& AsyncOperationID, const FString& Reason)
// {
//     FAsyncOperationInfo* OperationInfo = AsyncOperations.Find(AsyncOperationID);
//     if (!OperationInfo)
//     {
//         return false;
//     }
//     
//     UEventSequenceAsyncBlueprintAction* AsyncAction = OperationInfo->AsyncAction.Get();
//     if (AsyncAction)
//     {
//         AsyncAction->CompleteFailure(Reason);
//     }
//     
//     return true;
// }
//
// // 检查是否有挂起的异步操作
// bool UEventSequenceSystem::HasPendingAsyncOperation(const FGuid& SequenceID) const
// {
//     const TArray<FGuid>* Operations = SequenceToAsyncOperations.Find(SequenceID);
//     if (!Operations)
//     {
//         return false;
//     }
//     
//     for (const FGuid& OperationID : *Operations)
//     {
//         const FAsyncOperationInfo* Info = AsyncOperations.Find(OperationID);
//         if (Info && Info->AsyncAction.IsValid() && !Info->AsyncAction->IsCompleted())
//         {
//             return true;
//         }
//     }
//     
//     return false;
// }


// 异步操作完成回调
// void UEventSequenceSystem::OnAsyncActionCompleted(const FGuid& AsyncActionID)
// {
//     // 查找对应的异步操作
//     FGuid FoundOperationID;
//     FAsyncOperationInfo* FoundOperation = nullptr;
//     
//     for (auto& Pair : AsyncOperations)
//     {
//         if (Pair.Value.AsyncActionID == AsyncActionID)
//         {
//             FoundOperationID = Pair.Key;
//             FoundOperation = &Pair.Value;
//             break;
//         }
//     }
//     
//     if (!FoundOperation)
//     {
//         return;
//     }
//     
//     // 获取序列实例
//     UEventSequenceRunning* Instance = GetAsyncEventSequence(FoundOperation->SequenceID);
//     if (Instance)
//     {
//         // 通知序列实例异步操作完成
//         Instance->OnAsyncOperationCompleted(FoundOperation->EventIndex, EAsyncActionResult::Success, TEXT(""));
//     }
//     
//     // 广播完成事件
//     OnAsyncOperationCompleted.Broadcast(FoundOperation->SequenceID, FoundOperationID, EAsyncActionResult::Success);
//     
//     // 从映射中移除
//     AsyncOperations.Remove(FoundOperationID);
//     
//     TArray<FGuid>* SequenceOperations = SequenceToAsyncOperations.Find(FoundOperation->SequenceID);
//     if (SequenceOperations)
//     {
//         SequenceOperations->Remove(FoundOperationID);
//     }
// }
//
// void UEventSequenceSystem::OnAsyncActionFailed(const FString& Reason, const FGuid& AsyncActionID)
// {
//     // 查找对应的异步操作
//     FGuid FoundOperationID;
//     FAsyncOperationInfo* FoundOperation = nullptr;
//     
//     for (auto& Pair : AsyncOperations)
//     {
//         if (Pair.Value.AsyncActionID == AsyncActionID)
//         {
//             FoundOperationID = Pair.Key;
//             FoundOperation = &Pair.Value;
//             break;
//         }
//     }
//     
//     if (!FoundOperation)
//     {
//         return;
//     }
//     
//     // 获取序列实例
//     UEventSequenceRunning* Instance = GetAsyncEventSequence(FoundOperation->SequenceID);
//     if (Instance)
//     {
//         // 通知序列实例异步操作失败
//         Instance->OnAsyncOperationCompleted(FoundOperation->EventIndex, EAsyncActionResult::Failed, Reason);
//     }
//     
//     // 广播完成事件
//     OnAsyncOperationCompleted.Broadcast(FoundOperation->SequenceID, FoundOperationID, EAsyncActionResult::Failed);
//     
//     // 从映射中移除
//     AsyncOperations.Remove(FoundOperationID);
//     
//     TArray<FGuid>* SequenceOperations = SequenceToAsyncOperations.Find(FoundOperation->SequenceID);
//     if (SequenceOperations)
//     {
//         SequenceOperations->Remove(FoundOperationID);
//     }
// }


// 处理中断恢复
void UEventSequenceSystem::HandleInterruptRecovery( UEventSequenceRunning* Instance, FSequenceEvent_AsyncBlueprintCall& AsyncEvent)
{
    if (!Instance)
    {
        return;
    }
    
    switch (AsyncEvent.InterruptBehavior)
    {
    case EAsyncInterruptBehavior::Restart:
        // 重启语义：重新创建异步实例
        if (AsyncEvent.ActionInstance)
        {
            AsyncEvent.ActionInstance->ResetState();
        }
        AsyncEvent.AsyncResult = EAsyncActionResult::Pending;
        AsyncEvent.FailureReason.Empty();
        break;
        
    case EAsyncInterruptBehavior::Skip:
        // 跳过语义：直接视为失败
        AsyncEvent.HandleAsyncComplete(EAsyncActionResult::Failed, TEXT("Interrupted"));
        break;
    }
}
