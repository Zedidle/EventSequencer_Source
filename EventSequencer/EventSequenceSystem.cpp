// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSequenceSystem.h"

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
        
        // 创建事件的运行时副本
        FInstancedStruct RuntimeEventStruct;
        RuntimeEventStruct.InitializeAs(SourceEventStruct.GetScriptStruct());

        if (const F_SequenceEvent_LABEL* SourceEvent_LABEL = SourceEventStruct.GetPtr<F_SequenceEvent_LABEL>())
        {
            if (F_SequenceEvent_LABEL* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_LABEL>())
            {
                // 直接赋值可能因为 NestedEvents 嵌套导致性能问题，可以考虑移动语义
                *DEvent = *SourceEvent_LABEL;
                EventSequenceRunning->AddLabel(DEvent->LabelName);
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
            }
        }
        else if (const F_SequenceEvent_GOTO* SourceEvent_GOTO = SourceEventStruct.GetPtr<F_SequenceEvent_GOTO>())
        {
            if (F_SequenceEvent_GOTO* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_GOTO>())
            {
                *DEvent = *SourceEvent_GOTO;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
            }
        }
        else if (const F_SequenceEvent_IF* SourceEvent_IF = SourceEventStruct.GetPtr<F_SequenceEvent_IF>())
        {
            if (F_SequenceEvent_IF* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_IF>())
            {
                *DEvent = *SourceEvent_IF;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
                
                // 初步分析 TrueEvents 和 FalseEvents 开始的下标 以及 跳出的下标
                // 下标未必准确，可能有 +-1 的差距
                DEvent->TrueEventsStartIndex = EventSequenceRunning->GetEventsNum();
                ParseEventSequence(EventSequenceRunning, DEvent->TrueEvents);
                
                DEvent->FalseEventsStartIndex = EventSequenceRunning->GetEventsNum();
                ParseEventSequence(EventSequenceRunning, DEvent->FalseEvents);

                DEvent->EndIndex = EventSequenceRunning->GetEventsNum();
            }
        }
        else if (const F_SequenceEvent_SWITCH* SourceEvent_SWITCH = SourceEventStruct.GetPtr<F_SequenceEvent_SWITCH>())
        {
            if (F_SequenceEvent_SWITCH* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_SWITCH>())
            {
                *DEvent = *SourceEvent_SWITCH;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);

                for (auto& E : DEvent->EventCases)
                {
                    E.CaseEventIndex = EventSequenceRunning->GetEventsNum();
                    ParseEventSequence(EventSequenceRunning, E.CaseEvents);
                }

                DEvent->EndIndex = EventSequenceRunning->GetEventsNum();
            }
        }
        else if (const F_SequenceEvent_LOOP* SourceEvent_LOOP = SourceEventStruct.GetPtr<F_SequenceEvent_LOOP>())
        {
            if (F_SequenceEvent_LOOP* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_LOOP>())
            {
                // 关于Loop事件，由于解析的平扁化处理，需要记录下它在 EventSequenceRunning 的序列开始下标和结束下标，以便循环和Break跳出。
                *DEvent = *SourceEvent_LOOP;
                DEvent->State.LoopStartIndex = EventSequenceRunning->GetEventsNum();
                ParseEventSequence(EventSequenceRunning, DEvent->LoopEvents);

                DEvent->State.LoopEndIndex = EventSequenceRunning->GetEventsNum();
            }
        }
        else if (const F_SequenceEvent_BREAK* SourceEvent_BREAK = SourceEventStruct.GetPtr<F_SequenceEvent_BREAK>())
        {
            if (F_SequenceEvent_BREAK* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_BREAK>())
            {
                *DEvent = *SourceEvent_BREAK;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
            }
        }
        else if (const F_SequenceEvent_RETURN* SourceEvent_RETURN = SourceEventStruct.GetPtr<F_SequenceEvent_RETURN>())
        {
            if (F_SequenceEvent_RETURN* DEvent = RuntimeEventStruct.GetMutablePtr<F_SequenceEvent_RETURN>())
            {
                *DEvent = *SourceEvent_RETURN;
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
            }
        }
        // 具体事件
        else
        {
            if (auto* DestEvent = RuntimeEventStruct.GetMutablePtr<FNestedSequenceEvent>())
            {
                CopySequenceEventProperty<FMoveSequenceEvent>(SourceEventStruct, RuntimeEventStruct) || 
                CopySequenceEventProperty<FDialogSequenceEvent>(SourceEventStruct, RuntimeEventStruct) ||
                CopySequenceEventProperty<FWaitSequenceEvent>(SourceEventStruct, RuntimeEventStruct) ||
                CopySequenceEventProperty<FChoiceSequenceEvent>(SourceEventStruct, RuntimeEventStruct);
                
                DestEvent->SetState(EEventState::Pending);
                EventSequenceRunning->AddEvent(RuntimeEventStruct);
                ParseEventSequence(EventSequenceRunning, DestEvent->NestedEvents);
            }
        }
    }
}

UEventSequenceSystem* UEventSequenceSystem::GetInstance(UWorld* World)
{
    if (World)
    {
        return World->GetSubsystem<UEventSequenceSystem>();
    }
    return nullptr;	
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

UEventSequenceRunning* UEventSequenceSystem::CreateEventSequence(UEventSequenceDA* TargetDataAsset, UEventSequenceComponent* Component, UPropertyBagWrapper* PropertyBagInput)
{
    if (!TargetDataAsset) return nullptr;
    if (TargetDataAsset->GetSequenceLength() == 0) return nullptr;
    
    UEventSequenceRunning* EventSequenceRunning = NewObject<UEventSequenceRunning>();

    if (PropertyBagInput)
    {
        TargetDataAsset->SetPropertyBagInput(PropertyBagInput->GetPropertyBag());
    }
    EventSequenceRunning->SetDataAsset(TargetDataAsset);

    ParseEventSequence(EventSequenceRunning, TargetDataAsset->EventWrappers);
    
    if (Component)
    {
        EventSequenceComponents.Add(Component);
    }
    else
    {
        EventSequencesRunning.Add(EventSequenceRunning);
    }
    return EventSequenceRunning;
}


bool UEventSequenceSystem::RemoveEventSequence(UEventSequenceRunning* EventSequence)
{
    if (!EventSequence) return false;
    
    if (EventSequencesRunning.Contains(EventSequence))
    {
        EventSequencesRunning.Remove(EventSequence);
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

const FAsyncOperationInfo* UEventSequenceSystem::GetAsyncOperationInfo(const FGuid& AsyncOperationID) const
{
    return nullptr;
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

void UEventSequenceSystem::CleanupCompletedAsyncOperations()
{
}

void UEventSequenceSystem::ProcessAsyncOperations(float DeltaTime)
{
}

FGuid UEventSequenceSystem::StartAsyncOperation(UEventSequenceRunning* Instance, int32 EventIndex,
                                                UEventSequenceAsyncBlueprintAction* AsyncAction)
{
    if (!Instance || !AsyncAction)
    {
        return FGuid();
    }
    
    FGuid AsyncOperationID = GenerateAsyncOperationID();
    
    FAsyncOperationInfo OperationInfo;
    // OperationInfo.SequenceID = Instance->GetSequenceID();
    OperationInfo.EventIndex = EventIndex;
    OperationInfo.AsyncAction = AsyncAction;
    // OperationInfo.AsyncActionID = AsyncAction->GetUniqueID();
    OperationInfo.StartTime = GetWorld()->GetTimeSeconds();
    
    // 添加到映射
    AsyncOperations.Add(AsyncOperationID, OperationInfo);
    
    TArray<FGuid>& SequenceOperations = SequenceToAsyncOperations.FindOrAdd(Instance->GetSequenceID());
    SequenceOperations.Add(AsyncOperationID);
    
    // 绑定委托
    // AsyncAction->OnCompleted.AddDynamic(this, &UEventSequenceSystem::OnAsyncActionCompleted, AsyncOperationID);
    // AsyncAction->OnFailed.AddUObject(this, &UEventSequenceSystem::OnAsyncActionFailed, AsyncAction->GetUniqueID());
    
    // 广播事件
    OnAsyncOperationStarted.Broadcast(Instance->GetSequenceID(), AsyncOperationID, AsyncAction);
    
    return AsyncOperationID;
}

// 取消异步操作
bool UEventSequenceSystem::CancelAsyncOperation(const FGuid& AsyncOperationID, const FString& Reason)
{
    FAsyncOperationInfo* OperationInfo = AsyncOperations.Find(AsyncOperationID);
    if (!OperationInfo)
    {
        return false;
    }
    
    UEventSequenceAsyncBlueprintAction* AsyncAction = OperationInfo->AsyncAction.Get();
    if (AsyncAction)
    {
        AsyncAction->CompleteFailure(Reason);
    }
    
    return true;
}

// 检查是否有挂起的异步操作
bool UEventSequenceSystem::HasPendingAsyncOperation(const FGuid& SequenceID) const
{
    const TArray<FGuid>* Operations = SequenceToAsyncOperations.Find(SequenceID);
    if (!Operations)
    {
        return false;
    }
    
    for (const FGuid& OperationID : *Operations)
    {
        const FAsyncOperationInfo* Info = AsyncOperations.Find(OperationID);
        if (Info && Info->AsyncAction.IsValid() && !Info->AsyncAction->IsCompleted())
        {
            return true;
        }
    }
    
    return false;
}

TArray<FGuid> UEventSequenceSystem::GetPendingAsyncOperations(const FGuid& SequenceID) const
{
    return TArray<FGuid>();
}

// 异步操作完成回调
void UEventSequenceSystem::OnAsyncActionCompleted(const FGuid& AsyncActionID)
{
    // 查找对应的异步操作
    FGuid FoundOperationID;
    FAsyncOperationInfo* FoundOperation = nullptr;
    
    for (auto& Pair : AsyncOperations)
    {
        if (Pair.Value.AsyncActionID == AsyncActionID)
        {
            FoundOperationID = Pair.Key;
            FoundOperation = &Pair.Value;
            break;
        }
    }
    
    if (!FoundOperation)
    {
        return;
    }
    
    // 获取序列实例
    UEventSequenceRunning* Instance = GetAsyncEventSequence(FoundOperation->SequenceID);
    if (Instance)
    {
        // 通知序列实例异步操作完成
        Instance->OnAsyncOperationCompleted(FoundOperation->EventIndex, EAsyncActionResult::Success, TEXT(""));
    }
    
    // 广播完成事件
    OnAsyncOperationCompleted.Broadcast(FoundOperation->SequenceID, FoundOperationID, EAsyncActionResult::Success);
    
    // 从映射中移除
    AsyncOperations.Remove(FoundOperationID);
    
    TArray<FGuid>* SequenceOperations = SequenceToAsyncOperations.Find(FoundOperation->SequenceID);
    if (SequenceOperations)
    {
        SequenceOperations->Remove(FoundOperationID);
    }
}

void UEventSequenceSystem::OnAsyncActionFailed(const FString& Reason, const FGuid& AsyncActionID)
{
    // 查找对应的异步操作
    FGuid FoundOperationID;
    FAsyncOperationInfo* FoundOperation = nullptr;
    
    for (auto& Pair : AsyncOperations)
    {
        if (Pair.Value.AsyncActionID == AsyncActionID)
        {
            FoundOperationID = Pair.Key;
            FoundOperation = &Pair.Value;
            break;
        }
    }
    
    if (!FoundOperation)
    {
        return;
    }
    
    // 获取序列实例
    UEventSequenceRunning* Instance = GetAsyncEventSequence(FoundOperation->SequenceID);
    if (Instance)
    {
        // 通知序列实例异步操作失败
        Instance->OnAsyncOperationCompleted(FoundOperation->EventIndex, EAsyncActionResult::Failed, Reason);
    }
    
    // 广播完成事件
    OnAsyncOperationCompleted.Broadcast(FoundOperation->SequenceID, FoundOperationID, EAsyncActionResult::Failed);
    
    // 从映射中移除
    AsyncOperations.Remove(FoundOperationID);
    
    TArray<FGuid>* SequenceOperations = SequenceToAsyncOperations.Find(FoundOperation->SequenceID);
    if (SequenceOperations)
    {
        SequenceOperations->Remove(FoundOperationID);
    }
}

void UEventSequenceSystem::SerializeAsyncOperations(const FGuid& SequenceID, TArray<FString>& OutSerializedStates)
{
    
}

void UEventSequenceSystem::DeserializeAsyncOperations(const FGuid& SequenceID, const TArray<FString>& SerializedStates)
{
    
}

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
        if (AsyncEvent.AsyncInstance)
        {
            AsyncEvent.AsyncInstance->ResetState();
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
