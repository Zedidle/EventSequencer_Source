// Fill out your copyright notice in the Description page of Project Settings.


#include "EventAsyncObject.h"
#include "Net/NetworkMetricsDefs.h"

#include "Engine/Engine.h"
#include "Serialization/Archive.h"

const FString UEventAsyncObject::SerializeKeyPrefix = TEXT("AsyncAction_");

UEventAsyncObject::UEventAsyncObject()
{
    // 设置默认值
    CurrentResult = EEventExecuteResult::None;
    bIsExecuting = false;
    bIsCanceled = false;
}

UEventAsyncObject* UEventAsyncObject::CreateAsyncEvent(UObject* WorldContextObject,
    const TMap<FString, FString>& Context, EInterruptBehavior InterruptBehavior)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get world for async action"));
        return nullptr;
    }
    
    UEventAsyncObject* E = NewObject<UEventAsyncObject>(World);
    if (E)
    {
        E->SetContext(Context);
        E->InterruptBehavior = InterruptBehavior;
    }
    
    return E;
}


void UEventAsyncObject::Activate()
{
    Super::Activate();
    
    if (bIsCanceled || CurrentResult != EEventExecuteResult::None)
    {
        // 如果已被取消或有结果，根据中断行为处理
        if (InterruptBehavior == EInterruptBehavior::Restart)
        {
            Restart();
        }
        else
        {
            Skip(TEXT("Skipped due to interrupt"));
        }
        return;
    }
    
    bIsExecuting = true;
    CurrentResult = EEventExecuteResult::None;
    
    // 调用蓝图实现
    OnExecute();
}

void UEventAsyncObject::Cancel(const FString& Reason)
{
    if (bIsCanceled || CurrentResult != EEventExecuteResult::None)
        return;
    
    bIsCanceled = true;
    bIsExecuting = false;
    
    InternalComplete(EEventExecuteResult::Canceled, Reason);
}

void UEventAsyncObject::Restart()
{
    // 重置状态
    bIsCanceled = false;
    bIsExecuting = false;
    CurrentResult = EEventExecuteResult::None;
    
    // 重新激活
    Activate();
}

void UEventAsyncObject::Skip(const FString& Reason)
{
    bIsExecuting = false;
    InternalComplete(EEventExecuteResult::Failed, Reason);
}

void UEventAsyncObject::SetContext(const TMap<FString, FString>& InContext)
{
    ContextData = InContext;
}

FString UEventAsyncObject::GetContextValue(const FString& Key, const FString& DefaultValue) const
{
    const FString* Value = ContextData.Find(Key);
    return Value ? *Value : DefaultValue;
}

void UEventAsyncObject::SetContextValue(const FString& Key, const FString& Value)
{
    ContextData.Add(Key, Value);
}

void UEventAsyncObject::Resolve(const FString& Reason)
{
    if (bIsCanceled || CurrentResult != EEventExecuteResult::None)
        return;
    
    InternalComplete(EEventExecuteResult::Success, Reason);
}

void UEventAsyncObject::Reject(const FString& Reason)
{
    if (bIsCanceled || CurrentResult != EEventExecuteResult::None)
        return;
    
    InternalComplete(EEventExecuteResult::Failed, Reason);
}

void UEventAsyncObject::InternalComplete(EEventExecuteResult Result, const FString& Reason)
{
    bIsExecuting = false;
    CurrentResult = Result;
    
    // 根据结果触发相应委托
    switch (Result)
    {
    case EEventExecuteResult::Success:
        OnSuccess.Broadcast(Result, Reason);
        break;
        
    case EEventExecuteResult::Failed:
    case EEventExecuteResult::Canceled:
        OnFailure.Broadcast(Result, Reason);
        break;
    }
    
    // 触发完成委托
    OnCompleted.Broadcast(Result, Reason);
    
    // 标记为可销毁
    SetReadyToDestroy();
}

void UEventAsyncObject::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    
    // 保存状态
    if (Ar.IsSaving())
    {
        // 保存上下文数据
        int32 ContextCount = ContextData.Num();
        Ar << ContextCount;
        
        for (const auto& Pair : ContextData)
        {
            FString Key = Pair.Key;
            FString Value = Pair.Value;
            Ar << Key;
            Ar << Value;
        }
        
        // 保存其他状态
        uint8 BehaviorByte = (uint8)InterruptBehavior;
        uint8 ResultByte = (uint8)CurrentResult;
        
        Ar << BehaviorByte;
        Ar << ResultByte;
        Ar << bIsExecuting;
        Ar << bIsCanceled;
    }
    else if (Ar.IsLoading())
    {
        // 加载上下文数据
        int32 ContextCount = 0;
        Ar << ContextCount;
        
        ContextData.Empty();
        for (int32 i = 0; i < ContextCount; ++i)
        {
            FString Key, Value;
            Ar << Key;
            Ar << Value;
            ContextData.Add(Key, Value);
        }
        
        // 加载其他状态
        uint8 BehaviorByte, ResultByte;
        Ar << BehaviorByte;
        Ar << ResultByte;
        Ar << bIsExecuting;
        Ar << bIsCanceled;
        
        InterruptBehavior = (EInterruptBehavior)BehaviorByte;
        CurrentResult = (EEventExecuteResult)ResultByte;
    }
}

TMap<FString, FString> UEventAsyncObject::GetSerializeData() const
{
    TMap<FString, FString> Data;
    
    // 保存类名
    Data.Add(SerializeKeyPrefix + TEXT("ClassName"), GetClass()->GetName());
    
    // 保存上下文
    for (const auto& Pair : ContextData)
    {
        Data.Add(SerializeKeyPrefix + Pair.Key, Pair.Value);
    }
    
    // 保存状态
    Data.Add(SerializeKeyPrefix + TEXT("InterruptBehavior"), FString::FromInt((int32)InterruptBehavior));
    Data.Add(SerializeKeyPrefix + TEXT("CurrentResult"), FString::FromInt((int32)CurrentResult));
    Data.Add(SerializeKeyPrefix + TEXT("IsExecuting"), bIsExecuting ? TEXT("1") : TEXT("0"));
    Data.Add(SerializeKeyPrefix + TEXT("IsCanceled"), bIsCanceled ? TEXT("1") : TEXT("0"));
    
    return Data;
}

void UEventAsyncObject::RestoreFromData(const TMap<FString, FString>& Data)
{
    // 清空现有数据
    ContextData.Empty();
    
    // 恢复数据
    for (const auto& Pair : Data)
    {
        FString Key = Pair.Key;
        FString Value = Pair.Value;
        
        // 去掉前缀
        if (Key.RemoveFromStart(SerializeKeyPrefix))
        {
            if (Key == TEXT("InterruptBehavior"))
            {
                InterruptBehavior = (EInterruptBehavior)FCString::Atoi(*Value);
            }
            else if (Key == TEXT("CurrentResult"))
            {
                CurrentResult = (EEventExecuteResult)FCString::Atoi(*Value);
            }
            else if (Key == TEXT("IsExecuting"))
            {
                bIsExecuting = (Value == TEXT("1"));
            }
            else if (Key == TEXT("IsCanceled"))
            {
                bIsCanceled = (Value == TEXT("1"));
            }
            else
            {
                // 其他键作为上下文
                ContextData.Add(Key, Value);
            }
        }
    }
}


























// UEventAsyncObject* UEventAsyncObject::CreateAsyncEvent(EEventType Type)
// {
// 	UEventAsyncObject* SyncObj = NewObject<UEventAsyncObject>();
// 	SyncObj->EventType = Type;
// 	SyncObj->AddToRoot();
//
// 	FTimerHandle Timer;
// 	
// 	GEngine->GetCurrentPlayWorld()->GetTimerManager().SetTimer(Timer, SyncObj, &UEventAsyncObject::Execute, 1.0f, false);
// 	return SyncObj;
// }
//
// void UEventAsyncObject::Execute()
// {
// 	if (FMath::RandBool())
// 	{
// 		Resolve.Broadcast(EEventExecuteResult::Success);
// 	}
// 	else
// 	{
// 		Reject.Broadcast(EEventExecuteResult::Failed);
// 	}
// 	RemoveFromRoot();
// }
