// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSequenceSystem.h"
#include "UObject/UnrealType.h"
#include "Engine/World.h"

class AAIController;


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

UEventSequenceRunning* UEventSequenceSystem::CreateEventSequence(UEventSequenceDA* TargetDataAsset, UEventSequenceComponent* Component)
{
    if (!TargetDataAsset) return nullptr;
    if (TargetDataAsset->GetSequenceLength() == 0) return nullptr;
    
    UEventSequenceRunning* EventSequence = NewObject<UEventSequenceRunning>();
    
    for (int32 i = 0; i < TargetDataAsset->GetSequenceLength(); ++i)
    {
        FInstancedStruct EventStruct;
        if (TargetDataAsset->GetEventAtIndex(i, EventStruct))
        {
            // 创建事件的运行时副本
            FInstancedStruct RuntimeEvent;
            RuntimeEvent.InitializeAs(EventStruct.GetScriptStruct());
            
            if (auto* DestEvent = RuntimeEvent.GetMutablePtr<FBaseSequenceEvent>())
            {
                // 基础事件数据复制
                DestEvent->SetState(EEventState::Pending);
                
                // 根据具体类型进行特殊处理
                if (EventStruct.GetScriptStruct()->IsChildOf(FMoveSequenceEvent::StaticStruct()))
                {
                    // NPC移动事件特殊处理
                    const FMoveSequenceEvent* SourceEvent = EventStruct.GetPtr<FMoveSequenceEvent>();
                    FMoveSequenceEvent* DEvent = RuntimeEvent.GetMutablePtr<FMoveSequenceEvent>();
                    if (SourceEvent && DEvent)
                    {
                        DEvent->Property.TargetLocation = SourceEvent->Property.TargetLocation;
                        DEvent->Property.ApproachDistance = SourceEvent->Property.ApproachDistance;
                        DEvent->bMoving = false;
                    }
                }
                else if (EventStruct.GetScriptStruct()->IsChildOf(FDialogSequenceEvent::StaticStruct()))
                {
                    // 对话事件特殊处理
                    const FDialogSequenceEvent* SourceEvent = EventStruct.GetPtr<FDialogSequenceEvent>();
                    FDialogSequenceEvent* DEvent = RuntimeEvent.GetMutablePtr<FDialogSequenceEvent>();
                    if (SourceEvent && DEvent)
                    {
                        DEvent->Property.DialogLines = SourceEvent->Property.DialogLines;
                        DEvent->Property.TextDisplaySpeed = SourceEvent->Property.TextDisplaySpeed;
                    }
                }
                else if (EventStruct.GetScriptStruct()->IsChildOf(FWaitSequenceEvent::StaticStruct()))
                {
                    // 延迟事件特殊处理
                    const FWaitSequenceEvent* SourceEvent = EventStruct.GetPtr<FWaitSequenceEvent>();
                    FWaitSequenceEvent* DEvent = RuntimeEvent.GetMutablePtr<FWaitSequenceEvent>();
                    if (SourceEvent && DEvent)
                    {
                        DEvent->Property.Duration = SourceEvent->Property.Duration;
                    }
                }
                else if (EventStruct.GetScriptStruct()->IsChildOf(FChoiceSequenceEvent::StaticStruct()))
                {
                    // 延迟事件特殊处理
                    const FChoiceSequenceEvent* SourceDelayEvent = EventStruct.GetPtr<FChoiceSequenceEvent>();
                    FChoiceSequenceEvent* DEvent = RuntimeEvent.GetMutablePtr<FChoiceSequenceEvent>();
                    if (SourceDelayEvent && DEvent)
                    {
                    }
                }
                
                DestEvent->SetState(EEventState::Idle);
                
                EventSequence->AddEventStruct(RuntimeEvent);
            }
        }
    }

    if (Component)
    {
        EventSequenceComponents.Add(Component);
    }
    else
    {
        EventSequencesRunning.Add(EventSequence);
    }
    return EventSequence;
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


// 以下是参考目录
/************************************************************************/
/* FEventSequenceVariables 实现                                         */
/************************************************************************/
// void FEventSequenceVariables::Init(const TMap<FName, FInstancedStruct>& ExternalOverrides)
// {
//     RuntimeValues.Empty();
//     for (const FEventSequenceVarDef& VarDef : VarDefs)
//     {
//         if (!VarDef.IsValid()) continue;
//
//         // FInstancedStruct VarValue;
//         // // 优先使用外部覆写的值
//         // if (VarDef.bIsReference && ExternalOverrides.Contains(VarDef.VarName))
//         // {
//         //     VarValue = ExternalOverrides.at(VarDef.VarName);
//         // }
//         // else
//         // {
//         //     VarValue = VarDef.DefaultValue;
//         // }
//         // RuntimeValues.Add(VarDef.VarName, VarValue);
//     }
// }

/************************************************************************/
/* UEventSequenceDataAsset 实现                                         */
/************************************************************************/
// bool UEventSequenceDataAsset2::IsSequenceValid() const
// {
//     if (!SequenceVariables.VarDefs.IsEmpty())
//     {
//         for (const FEventSequenceVarDef& VarDef : SequenceVariables.VarDefs)
//         {
//             if (!VarDef.IsValid()) return false;
//         }
//     }
//     if (RootCommands.IsEmpty()) return false;
//     if (MaxExecuteCountPerUpdate < 1) return false;
//     return true;
// }

/************************************************************************/
/* UEventSequenceInterpreter 实现                                       */
/************************************************************************/
// UEventSequenceInterpreter::UEventSequenceInterpreter()
//     : CurrentCmdIndex(0)
//     , ExecuteCountInCurrentUpdate(0)
// {
//     bIsRunning = false;
//     bIsWaiting = false;
// }
//
// bool UEventSequenceInterpreter::InitInterpreter(UEventSequenceDataAsset2* InSequenceDA, UObject* InContext, const TMap<FName, FInstancedStruct>& ExternalVarOverrides)
// {
//     // 合法性检查
//     if (!InSequenceDA || !InSequenceDA->IsSequenceValid() || !InContext || !InContext->IsA(InSequenceDA->ContextClass))
//     {
//         UE_LOG(LogTemp, Error, TEXT("EventSequence Interpreter Init Failed: Invalid DA/Context"));
//         return false;
//     }
//
//     // 初始化核心成员
//     SequenceDA = InSequenceDA;
//     Context = InContext;
//     InterpreterVariables = SequenceDA->SequenceVariables;
//     InterpreterVariables.Init(ExternalVarOverrides);
//
//     // 扁平化解命令树
//     FlattenCommandTree();
//
//     // 重置执行状态
//     CurrentCmdIndex = 0;
//     RemainingWaitTime = 0.0f;
//     bIsWaiting = false;
//     bIsRunning = true;
//
//     UE_LOG(LogTemp, Log, TEXT("EventSequence Interpreter Init Success: %s"), *InSequenceDA->SequenceName.ToString());
//     return true;
// }
//
// void UEventSequenceInterpreter::Update(float DeltaTime)
// {
//     if (!bIsRunning || !SequenceDA || FlattenedCommands.IsEmpty()) return;
//
//     // 处理等待状态
//     if (bIsWaiting)
//     {
//         RemainingWaitTime -= DeltaTime;
//         if (RemainingWaitTime <= 0.0f)
//         {
//             bIsWaiting = false;
//             RemainingWaitTime = 0.0f;
//         }
//         else
//         {
//             return; // 仍在等待，直接返回
//         }
//     }
//
//     // 重置本次Update的执行计数
//     ExecuteCountInCurrentUpdate = 0;
//     const int32 MaxExecuteCount = SequenceDA->MaxExecuteCountPerUpdate;
//
//     // 循环执行指令，直到达到阈值/需要等待/执行完毕
//     while (bIsRunning && !bIsWaiting && CurrentCmdIndex < FlattenedCommands.Num() && ExecuteCountInCurrentUpdate < MaxExecuteCount)
//     {
//         int32 NextCmdIndex = CurrentCmdIndex + 1; // 默认下一条指令
//         // 执行当前指令
//         if (ExecuteSingleCommand(CurrentCmdIndex, NextCmdIndex))
//         {
//             CurrentCmdIndex = NextCmdIndex;
//         }
//         else
//         {
//             StopInterpreter(false); // 指令执行失败，强制停止
//             break;
//         }
//         ExecuteCountInCurrentUpdate++;
//     }
//
//     // 检查是否执行完毕
//     if (CurrentCmdIndex >= FlattenedCommands.Num())
//     {
//         StopInterpreter(true); // 正常结束
//     }
// }
//
// void UEventSequenceInterpreter::StopInterpreter(bool bIsNormalQuit)
// {
//     bIsRunning = false;
//     bIsWaiting = false;
//     RemainingWaitTime = 0.0f;
//
//     if (bIsNormalQuit)
//     {
//         UE_LOG(LogTemp, Log, TEXT("EventSequence Interpreter Stop Normal: %s"), *SequenceDA->SequenceName.ToString());
//     }
//     else
//     {
//         UE_LOG(LogTemp, Warning, TEXT("EventSequence Interpreter Stop Abnormal: %s"), *SequenceDA->SequenceName.ToString());
//     }
// }
//
// UWorld* UEventSequenceInterpreter::GetWorldContext() const
// {
//     if (Context)
//     {
//         return Context->GetWorld();
//     }
//     return GetWorld();
// }

// void UEventSequenceInterpreter::OnAsyncCommandComplete(EAsyncExecuteResult Result, const FInstancedStruct& ResultData)
// {
//     bIsWaiting = false;
//     RemainingWaitTime = 0.0f;
//     // 此处可扩展：将异步结果写入解释器变量，供后续命令使用
//     // 示例：InterpreterVariables.SetVarValue(TEXT("AsyncResult"), Result);
//     //      InterpreterVariables.SetVarValue(TEXT("AsyncResultData"), ResultData);
// }

// void UEventSequenceInterpreter::Serialize(FArchive& Ar)
// {
//     Super::Serialize(Ar);
//     // 序列化执行状态（核心：支持存档/读档）
//     Ar << bIsRunning;
//     Ar << bIsWaiting;
//     Ar << CurrentCmdIndex;
//     Ar << RemainingWaitTime;
//     Ar << CurrentAsyncInterruptPolicy;
//     // 序列化解释器变量（运行时值）
//     Ar << InterpreterVariables.RuntimeValues;
// }
//
// void UEventSequenceInterpreter::FlattenCommandTree()
// {
//     FlattenedCommands.Empty();
//     LabelIndexMap.Empty();
//
//     if (!SequenceDA || SequenceDA->RootCommands.IsEmpty()) return;
//
//     // 遍历根命令，扁平化解所有嵌套命令
//     // for (const FInstancedStruct& RootCmd : SequenceDA->RootCommands)
//     // {
//     //     if (RootCmd.IsValid() && RootCmd.GetStruct()->IsChildOf(FEventSequenceCommand::StaticStruct()))
//     //     {
//     //         const FEventSequenceCommand* Cmd = RootCmd.GetPtr<FEventSequenceCommand>();
//     //         if (Cmd)
//     //         {
//     //             Cmd->Flatten(FlattenedCommands, LabelIndexMap);
//     //         }
//     //     }
//     // }
//
//     UE_LOG(LogTemp, Log, TEXT("EventSequence Flatten Success: %d Commands, %d Labels"), FlattenedCommands.Num(), LabelIndexMap.Num());
// }
//
// bool UEventSequenceInterpreter::ExecuteSingleCommand(int32 CmdIndex, int32& OutNextCmdIndex)
// {
//     if (CmdIndex < 0 || CmdIndex >= FlattenedCommands.Num()) return false;
//
//     const FInstancedStruct& CmdStruct = FlattenedCommands[CmdIndex];
//     if (!CmdStruct.IsValid()) return false;
//
//     // 执行命令的核心逻辑
//     // const FEventSequenceCommand* Cmd = CmdStruct.GetPtr<FEventSequenceCommand>();
//     // if (Cmd && Cmd->IsValid())
//     // {
//     //     return Cmd->Execute(this, CmdIndex, OutNextCmdIndex);
//     // }
//
//     UE_LOG(LogTemp, Error, TEXT("Execute Command Failed: Invalid Command at Index %d"), CmdIndex);
//     return false;
// }

/************************************************************************/
/* UEventSequenceSyncBlueprint 实现                                     */
/************************************************************************/
// 空实现（策划蓝图重载Execute_Implementation）

/************************************************************************/
/* UEventSequenceAsyncBlueprint 实现                                    */
/************************************************************************/
// void UEventSequenceAsyncBlueprint::Resolve(const FInstancedStruct& ResultData)
// {
//     if (Interpreter)
//     {
//         Interpreter->OnAsyncCommandComplete(EAsyncExecuteResult::Success, ResultData);
//     }
// }
//
// void UEventSequenceAsyncBlueprint::Reject(EAsyncExecuteResult FailReason, const FInstancedStruct& ResultData)
// {
//     if (Interpreter)
//     {
//         Interpreter->OnAsyncCommandComplete(FailReason, ResultData);
//     }
// }

/************************************************************************/
/* UEventSequenceBlueprintLibrary 实现                                  */
/************************************************************************/
// UEventSequenceInterpreter* UEventSequenceBlueprintLibrary::CreateInterpreter(UObject* WorldContextObject)
// {
//     if (!WorldContextObject) return nullptr;
//     UWorld* World = WorldContextObject->GetWorld();
//     if (!World) return nullptr;
//
//     // 创建解释器实例（可挂载到任意UObject，此处默认挂载到WorldContext）
//     UEventSequenceInterpreter* Interpreter = NewObject<UEventSequenceInterpreter>(WorldContextObject);
//     Interpreter->AddToRoot(); // 防止被GC
//     return Interpreter;
// }
//
// UEventSequenceInterpreter* UEventSequenceBlueprintLibrary::StartEventSequence(UObject* WorldContextObject, UEventSequenceDataAsset2* SequenceDA, UObject* Context, const TMap<FName, FInstancedStruct>& ExternalVarOverrides)
// {
//     UEventSequenceInterpreter* Interpreter = CreateInterpreter(WorldContextObject);
//     if (Interpreter && Interpreter->InitInterpreter(SequenceDA, Context, ExternalVarOverrides))
//     {
//         return Interpreter;
//     }
//     // 初始化失败，销毁实例
//     if (Interpreter)
//     {
//         Interpreter->RemoveFromRoot();
//         Interpreter->MarkAsGarbage();
//     }
//     return nullptr;
// }
//
// void UEventSequenceBlueprintLibrary::SetInterpreterVar(UEventSequenceInterpreter* Interpreter, FName VarName, const FInstancedStruct& Value)
// {
//     if (Interpreter && Interpreter->GetInterpreterVariables())
//     {
//         Interpreter->GetInterpreterVariables()->RuntimeValues.FindOrAdd(VarName) = Value;
//     }
// }
//
// FInstancedStruct UEventSequenceBlueprintLibrary::GetInterpreterVar(UEventSequenceInterpreter* Interpreter, FName VarName)
// {
//     if (Interpreter && Interpreter->GetInterpreterVariables() && Interpreter->GetInterpreterVariables()->RuntimeValues.Contains(VarName))
//     {
//         return Interpreter->GetInterpreterVariables()->RuntimeValues[VarName];
//     }
//     return FInstancedStruct();
// }