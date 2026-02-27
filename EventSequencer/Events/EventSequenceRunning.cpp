// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSequenceRunning.h"
#include "../Events/SequenceEvent/CommonStructs.h"
#include "EventSequencer/EventSequenceSystem.h"
#include "EventSequencer/DataAssets/PropertyBagWrapper.h"
#include "Misc/DefaultValueHelper.h"
#include "SequenceEvent/_SequenceEvent_BREAK.h"
#include "SequenceEvent/_SequenceEvent_GOTO.h"
#include "SequenceEvent/_SequenceEvent_IF.h"
#include "SequenceEvent/_SequenceEvent_LOOP.h"
#include "SequenceEvent/_SequenceEvent_RETURN.h"
#include "SequenceEvent/_SequenceEvent_SWITCH.h"

void UEventSequenceRunning::AddEvent(FInstancedStruct& Event)
{
	if(!Event.IsValid()) return;
    
	if (auto* DestEvent = Event.GetMutablePtr<FBaseSequenceEvent>())
	{
		EventQueue.Add(Event);
	}
}

void UEventSequenceRunning::AppendEvents(TArray<FInstancedStruct>& Events)
{
	if (Events.IsEmpty()) return;
	
	for (auto& Event : Events)
	{
		AddEvent(Event);
	}
}

void UEventSequenceRunning::GOTO(FName Label)
{
	if(int* index = Label2Index.Find(Label))
	{
		GOTO(*index);
	}
}

void UEventSequenceRunning::GOTO(int Index)
{
	if(EventQueue.IsValidIndex(Index))
	{
		CurEventIndex = Index;
		if (auto* DestEvent =  EventQueue[Index].GetMutablePtr<FBaseSequenceEvent>())
		{
			DestEvent->SetState(EEventState::Pending);
		}
	}
	else
	{
		Exit();
	}
}


bool UEventSequenceRunning::IsCompleted() const
{
	return bCompleted;
}

void UEventSequenceRunning::GetCurEvent(FInstancedStruct& OutStruct)
{
	if (EventQueue.IsValidIndex(CurEventIndex))
	{
		OutStruct = EventQueue[CurEventIndex];
	}
}

void UEventSequenceRunning::AddLabel(FName Label)
{
	// 暂不考虑Label重复问题
	Label2Index.Add(Label,EventQueue.Num());
}

void UEventSequenceRunning::Start()
{
	bPause = false;
}

bool UEventSequenceRunning::Pause()
{
	return false;
}

bool UEventSequenceRunning::Resume()
{
	return false;
}

void UEventSequenceRunning::Stop()
{
	bPause = true;
}

bool UEventSequenceRunning::ExecuteStep()
{
	return false;
	
}

void UEventSequenceRunning::Next()
{
	FInstancedStruct CurEvent;
	GetCurEvent(CurEvent);
	if (FBaseSequenceEvent* E = CurEvent.GetMutablePtr<FBaseSequenceEvent>())
	{
		if (E->Execute())
		{
			CurEventIndex++;
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
			}
		}
	}
}

void UEventSequenceRunning::Exit()
{
	Destroy();
}

void UEventSequenceRunning::Destroy()
{
	if (UEventSequenceSystem* EventSequenceSystem = UEventSequenceSystem::GetInstance(this))
	{
		EventSequenceSystem->RemoveEventSequence(this);
	}
}

UPropertyBagWrapper* UEventSequenceRunning::GetPropertyBagWrapper() const
{
	UPropertyBagWrapper* Wrapper = NewObject<UPropertyBagWrapper>();
	Wrapper->SetPropertyBag(PropertyBagRuntime);
	return Wrapper;
}

bool UEventSequenceRunning::IsWaitingForAsync() const
{
	return WaitingForAsyncEventIndex != -1 && WaitingForAsyncOperationID.IsValid();
}

void UEventSequenceRunning::OnAsyncOperationCompleted(int32 EventIndex, EAsyncActionResult Result,
	const FString& Reason)
{
	if (EventIndex != WaitingForAsyncEventIndex)
	{
		return;
	}
    
	// 查找对应的异步事件
	if (EventIndex >= 0 && EventIndex < EventSequenceRuntime.Num())
	{
		FInstancedStruct& Event = EventSequenceRuntime[EventIndex];
		if (FSequenceEvent_AsyncBlueprintCall* AsyncEvent = Event.GetMutablePtr<FSequenceEvent_AsyncBlueprintCall>())
		{
			AsyncEvent->HandleAsyncComplete(Result, Reason);
            
			if (Result == EAsyncActionResult::Success)
			{
				// 同步输出数据
				if (AsyncEvent->AsyncInstance)
				{
					SyncPortData(AsyncEvent->AsyncInstance, AsyncEvent->PortBindings, false);
				}
                
				// 如果实例不支持缓存，销毁它
				if (AsyncEvent->AsyncInstance->GetInterruptBehavior() == EAsyncInterruptBehavior::Skip)
				{
					AsyncEvent->AsyncInstance->ConditionalBeginDestroy();
					AsyncEvent->AsyncInstance = nullptr;
				}
			}
            
			// 序列化状态（如果支持）
			if (AsyncEvent->AsyncInstance && AsyncEvent->AsyncInstance->SupportsStateSerialization())
			{
				AsyncEvent->SerializedState = AsyncEvent->AsyncInstance->SerializeState();
			}
		}
	}
    
	// 停止等待
	StopWaitingForAsync();
    
	// 如果序列在等待，恢复执行
	if (State == ESequenceState::WaitingForAsync)
	{
		SetState(ESequenceState::Running);
	}
}

TArray<FName> UEventSequenceRunning::GetAllLabels() const
{
	TArray<FName> LabelNames;
	Label2Index.GetKeys(LabelNames);
	return LabelNames;
}

int32 UEventSequenceRunning::FindLabelIndex(const FName& LabelName) const
{
	return Label2Index.FindRef(LabelName);
}


bool UEventSequenceRunning::ExecuteEvent(FInstancedStruct& Event, int32 EventIndex)
{
	return false;
}

bool UEventSequenceRunning::ExecuteAsyncBlueprintCallEvent(FSequenceEvent_AsyncBlueprintCall& AsyncEvent,
	int32 EventIndex)
{
	return false;
}

bool UEventSequenceRunning::ExecuteCatchBlock(FSequenceEvent_AsyncBlueprintCall& AsyncEvent, int32 EventIndex)
{
	return false;
}

void UEventSequenceRunning::StartWaitingForAsync(int32 EventIndex, const FGuid& AsyncOperationID)
{
}

void UEventSequenceRunning::StopWaitingForAsync()
{
}

bool UEventSequenceRunning::SyncPortData(UEventSequenceAsyncBlueprintAction* Instance, const TArray<FPortBinding>& PortBindings,
	bool bToBlueprint)
{
	return false;
}


bool UEventSequenceRunning::HandleControlFlowEvent(FInstancedStruct& Event, int32 EventIndex)
{
	return false;
}

bool UEventSequenceRunning::HandleActionEvent(FInstancedStruct& Event, int32 EventIndex)
{
	return false;
}



void UEventSequenceRunning::SetState(ESequenceState NewState)
{
}

bool UEventSequenceRunning::ExecuteBlueprintCallEvent(FSequenceEvent_BlueprintCall& BlueprintCallEvent, int32 EventIndex)
{
	// 1. 创建蓝图实例
	if (!BlueprintCallEvent.BlueprintInstance)
	{
		BlueprintCallEvent.BlueprintInstance = BlueprintCallEvent.GetOrCreateBlueprintInstance(this);
		if (!BlueprintCallEvent.BlueprintInstance)
		{
			return false;
		}
	}
    
	// 2. 创建执行上下文
	FSequenceBlueprintContext Context;
	Context.DeltaTime = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.0f;
	Context.CurrentEventIndex = EventIndex;
	Context.WorldContextObject = this;
    
	// 3. 执行蓝图
	bool bResult = BlueprintCallEvent.ExecuteBlueprint(
		BlueprintCallEvent.BlueprintInstance,
		Context,
		PropertyBagRuntime
	);
    
	// 4. 清理实例（如果需要）
	// if (bResult && BlueprintCallEvent.bAutoDestroyInstance)
	// {
	// 	BlueprintCallEvent.DestroyBlueprintInstance();
	// }
    
	return bResult;
}


bool UEventSequenceRunning::EvaluateCondition(const FSequenceCondition& Condition)
{
	if (Condition.PropertyName.IsNone()) return true;
    
    // 获取属性描述
    const FPropertyBagPropertyDesc* Desc = PropertyBagRuntime.FindPropertyDescByName(Condition.PropertyName);
    if (!Desc)
    {
        return false;
    }
    
    // 根据属性类型执行不同的比较,目前支持还不全面
    switch (Desc->ValueType)
    {
    case EPropertyBagPropertyType::Bool:
        {
            TValueOrError<bool, EPropertyBagResult> V = PropertyBagRuntime.GetValueBool(Condition.PropertyName);
            if (V.GetError() == EPropertyBagResult::Success)
            {
                bool ComparisonValue = false;
                if (TryParseBool(Condition.ComparisonValue, ComparisonValue))
                {
                    return CompareBool(V.GetValue(), ComparisonValue, Condition.Operator);
                }
            }
        }
        break;
        
    case EPropertyBagPropertyType::Int32:
        {
            TValueOrError<int32, EPropertyBagResult> V = PropertyBagRuntime.GetValueInt32(Condition.PropertyName);
    		if (V.HasError())
    		{
    		}
		    else
		    {
		    	int32 ComparisonValue;
		    	if (TryParseInt32(Condition.ComparisonValue, ComparisonValue))
		    	{
		    		return CompareInt32(V.GetValue(), ComparisonValue, Condition.Operator);
		    	}
		    }
        }
        break;
        
    case EPropertyBagPropertyType::Float:
        {
    		TValueOrError<float, EPropertyBagResult> V = PropertyBagRuntime.GetValueFloat(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			float ComparisonValue;
    			if (TryParseFloat(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareFloat(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Double:
        {
    		TValueOrError<double, EPropertyBagResult> V = PropertyBagRuntime.GetValueDouble(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			double ComparisonValue;
    			if (TryParseDouble(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareDouble(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::String:
        {
    		TValueOrError<FString, EPropertyBagResult> V = PropertyBagRuntime.GetValueString(Condition.PropertyName);
    		if (V.HasError())
    		{
    		}
		    else
		    {
    			FString ComparisonValue;
    			if (TryParseString(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareString(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Name:
        {
    		TValueOrError<FName, EPropertyBagResult> V = PropertyBagRuntime.GetValueName(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			FName ComparisonValue;
    			if (TryParseName(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareName(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
        
    case EPropertyBagPropertyType::Text:
        {
    		TValueOrError<FText, EPropertyBagResult> V = PropertyBagRuntime.GetValueText(Condition.PropertyName);
    		if (V.GetError() == EPropertyBagResult::Success)
    		{
    			FText ComparisonValue;
    			if (TryParseText(Condition.ComparisonValue, ComparisonValue))
    			{
    				return CompareText(V.GetValue(), ComparisonValue, Condition.Operator);
    			}
    		}
        }
        break;
    }
    
    return false;
	
}

bool UEventSequenceRunning::CompareBool(bool Left, bool Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal: return Left == Right;
	case ESequenceConditionOperator::NotEqual: return Left != Right;
	default: return false;
	}
}

bool UEventSequenceRunning::CompareInt32(int32 Left, int32 Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal: return Left == Right;
	case ESequenceConditionOperator::NotEqual: return Left != Right;
	case ESequenceConditionOperator::Greater: return Left > Right;
	case ESequenceConditionOperator::GreaterEqual: return Left >= Right;
	case ESequenceConditionOperator::Less: return Left < Right;
	case ESequenceConditionOperator::LessEqual: return Left <= Right;
	default: return false;
	}
}

bool UEventSequenceRunning::CompareFloat(const float& Left, const float& Right, ESequenceConditionOperator Op)
{
	// 浮点数比较需要处理精度误差
	const float Epsilon = 0.0001f;
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::NotEqual:
		return !FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Contains:
	case ESequenceConditionOperator::StartsWith:
	case ESequenceConditionOperator::EndsWith:
		// 对于浮点数，这些操作符没有意义，返回 false
		return false;
	default:
		return false; // 未知操作符
	}
}

bool UEventSequenceRunning::CompareDouble(const double& Left, const double& Right, ESequenceConditionOperator Op)
{
	// 双精度浮点数比较需要处理精度误差
	const double Epsilon = 0.000001;
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::NotEqual:
		return !FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right || FMath::IsNearlyEqual(Left, Right, Epsilon);
	default:
		return false; // 不支持的操作符（Contains, StartsWith, EndsWith 对数值类型无意义）
	}
}

bool UEventSequenceRunning::CompareString(const FString& Left, const FString& Right, ESequenceConditionOperator Op)
{
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return Left.Equals(Right, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::NotEqual:
		return !Left.Equals(Right, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::Greater:
		return Left > Right;
	case ESequenceConditionOperator::GreaterEqual:
		return Left >= Right;
	case ESequenceConditionOperator::Less:
		return Left < Right;
	case ESequenceConditionOperator::LessEqual:
		return Left <= Right;
	case ESequenceConditionOperator::Contains:
		return Left.Contains(Right);
	case ESequenceConditionOperator::StartsWith:
		return Left.StartsWith(Right);
	case ESequenceConditionOperator::EndsWith:
		return Left.EndsWith(Right);
	default:
		return false;
	}
}

bool UEventSequenceRunning::CompareName(const FName& Left, const FName& Right, ESequenceConditionOperator Op)
{
	// FName 的比较是大小写不敏感的
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return Left.IsEqual(Right, ENameCase::IgnoreCase);
	case ESequenceConditionOperator::NotEqual:
		return !Left.IsEqual(Right, ENameCase::IgnoreCase);
	case ESequenceConditionOperator::Contains:
		// FName 没有 Contains 方法，转换为字符串处理
		return Left.ToString().Contains(Right.ToString(), ESearchCase::IgnoreCase);
	case ESequenceConditionOperator::StartsWith:
		return Left.ToString().StartsWith(Right.ToString(), ESearchCase::IgnoreCase);
	case ESequenceConditionOperator::EndsWith:
		return Left.ToString().EndsWith(Right.ToString(), ESearchCase::IgnoreCase);
	default:
		return false; // 不支持的操作符（如大于、小于）
	}
}

bool UEventSequenceRunning::CompareText(const FText& Left, const FText& Right, ESequenceConditionOperator Op)
{
	// 转换为字符串进行比较
	FString LeftString = Left.ToString();
	FString RightString = Right.ToString();
    
	switch (Op)
	{
	case ESequenceConditionOperator::Equal:
		return LeftString.Equals(RightString, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::NotEqual:
		return !LeftString.Equals(RightString, ESearchCase::CaseSensitive);
	case ESequenceConditionOperator::Contains:
		return LeftString.Contains(RightString);
	case ESequenceConditionOperator::StartsWith:
		return LeftString.StartsWith(RightString);
	case ESequenceConditionOperator::EndsWith:
		return LeftString.EndsWith(RightString);
	default:
		return false; // 不支持的操作符（如大于、小于）
	}
}

bool UEventSequenceRunning::TryParseBool(const FString& StringValue, bool& OutValue)
{
	// 移除首尾空格
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 转换为小写以简化比较
	FString Lower = Trimmed.ToLower();
    
	// 尝试解析常见的布尔表示形式
	if (Lower == "true" || Lower == "1" || Lower == "yes" || Lower == "on" || Lower == "t")
	{
		OutValue = true;
		return true;
	}

	if (Lower == "false" || Lower == "0" || Lower == "no" || Lower == "off" || Lower == "f")
	{
		OutValue = false;
		return true;
	}
    
	// 尝试转换为整数
	int32 IntValue = 0;
	if (FDefaultValueHelper::ParseInt(Trimmed, IntValue))
	{
		OutValue = IntValue != 0;
		return true;
	}
    
	// 尝试转换为浮点数
	float FloatValue = 0.0f;
	if (FDefaultValueHelper::ParseFloat(Trimmed, FloatValue))
	{
		OutValue = FloatValue != 0.0f;
		return true;
	}
    
	// 无法解析
	return false;
}

bool UEventSequenceRunning::TryParseInt32(const FString& StringValue, int32& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	// 移除首尾空格
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 检查是否为空字符串
	if (Trimmed.IsEmpty())
	{
		return false;
	}
    
	if (FDefaultValueHelper::ParseInt(Trimmed, OutValue))
	{
		return true;
	}
	
	return false;
}

bool UEventSequenceRunning::TryParseFloat(const FString& StringValue, float& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (FDefaultValueHelper::ParseFloat(Trimmed, OutValue))
	{
		return true;
	}

	return false;
}

bool UEventSequenceRunning::TryParseDouble(const FString& StringValue, double& OutValue)
{
	if (StringValue.IsEmpty())
	{
		return false;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (FDefaultValueHelper::ParseDouble(Trimmed, OutValue))
	{
		return true;
	}
	return false;
}

bool UEventSequenceRunning::TryParseString(const FString& StringValue, FString& OutValue)
{
	if (StringValue.IsEmpty())
	{
		OutValue = TEXT("");
		return true;
	}
    
	OutValue = StringValue.TrimStartAndEnd();
	return true;
}

bool UEventSequenceRunning::TryParseName(const FString& StringValue, FName& OutValue)
{
	if (StringValue.IsEmpty())
	{
		OutValue = NAME_None;
		return true;
	}
    
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	// 检查是否是特殊名称
	if (Trimmed.Equals(TEXT("none"), ESearchCase::IgnoreCase))
	{
		OutValue = NAME_None;
		return true;
	}
    
	// 处理可能的命名空间格式：Package.Group.Object
	if (Trimmed.Contains(TEXT(".")))
	{
		// 已经是有效的 FName 格式
		OutValue = FName(*Trimmed);
		return true;
	}
    
	// 处理可能的空名称
	if (Trimmed.Equals(TEXT("")) || Trimmed.Equals(TEXT(" ")))
	{
		OutValue = NAME_None;
		return true;
	}
    
	// 检查是否是布尔值转名称
	if (Trimmed.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		OutValue = FName(TEXT("true"));
		return true;
	}
	else if (Trimmed.Equals(TEXT("false"), ESearchCase::IgnoreCase))
	{
		OutValue = FName(TEXT("false"));
		return true;
	}
    
	// 检查是否是数字转名称
	if (Trimmed.IsNumeric())
	{
		OutValue = FName(*Trimmed);
		return true;
	}
    
	// 默认情况：创建 FName
	OutValue = FName(*Trimmed);
	return true;
}

bool UEventSequenceRunning::TryParseText(const FString& StringValue, FText& OutValue)
{
	FString Trimmed = StringValue.TrimStartAndEnd();
    
	if (Trimmed.IsEmpty() || Trimmed.Equals(TEXT("empty"), ESearchCase::IgnoreCase))
	{
		OutValue = FText::GetEmpty();
	}
	else if (Trimmed.Equals(TEXT("none"), ESearchCase::IgnoreCase))
	{
		OutValue = FText::GetEmpty();
	}
	else
	{
		OutValue = FText::FromString(Trimmed);
	}
    
	return true;
}

void UEventSequenceRunning::SetDataAsset(UEventSequenceDA* DataAsset)
{
	if (!DataAsset) return;
	
	InitDataAsset = DataAsset;
	PropertyBagRuntime = InitDataAsset->PropertyBagDefault;
	// MigrateToNewBagInstance 合并逻辑有问题
	// PropertyBagRuntime.MigrateToNewBagInstance(InitDataAsset->PropertyBagInput);
	if (InitDataAsset->PropertyBagInput.IsValid())
	{
		if (const UPropertyBag* Bag = InitDataAsset->PropertyBagInput.GetPropertyBagStruct())
		{
			PropertyBagRuntime.AddProperties(Bag->GetPropertyDescs());
		}
		PropertyBagRuntime.CopyMatchingValuesByName(InitDataAsset->PropertyBagInput);
	}
}

void UEventSequenceRunning::LOOP(const FEventState_LOOP& LoopState)
{
	if (LoopStateStack.IsEmpty())
	{
		LoopStateStack.Push(LoopState);
		GOTO(LoopState.LoopStartIndex);
		return;
	}
	
	FEventState_LOOP& CurLoopState = LoopStateStack.Last();
	if (CurLoopState.LoopEventSelf == LoopState.LoopEventSelf)
	{
		CurLoopState.CurLoopTimes ++;
		if (CurLoopState.MaxLoopTimes > 0 && CurLoopState.CurLoopTimes == CurLoopState.MaxLoopTimes)
		{
			GOTO(CurLoopState.LoopEndIndex);
			LoopStateStack.Pop();
		}
		// 无限循环，只能等Break或Return跳出
		else if (CurLoopState.MaxLoopTimes == 0)
		{
			GOTO(CurLoopState.LoopStartIndex);
		}
	}
	else
	{
		LoopStateStack.Push(LoopState);
		GOTO(LoopState.LoopStartIndex);
	}
}

void UEventSequenceRunning::BREAK()
{
	if (LoopStateStack.IsEmpty())
	{
		return;
	}

	FEventState_LOOP& CurLoopState = LoopStateStack.Last();
	GOTO(CurLoopState.LoopEndIndex);
	LoopStateStack.Pop();
}


void UEventSequenceRunning::Tick(float DeltaTime)
{
	if (bPause) return;
	
	if (!EventQueue.IsValidIndex(CurEventIndex))
	{
		Exit();
		return;
	}
	
	FInstancedStruct& CurEventStruct = EventQueue[CurEventIndex];
	if (const F_SequenceEvent_GOTO* CurEvent_GOTO = CurEventStruct.GetPtr<F_SequenceEvent_GOTO>())
	{
		if (EvaluateCondition(CurEvent_GOTO->Condition))
		{
			GOTO(CurEvent_GOTO->TargetLabel);
		}
	}
	else if (const F_SequenceEvent_BREAK* CurEvent_BREAK = CurEventStruct.GetPtr<F_SequenceEvent_BREAK>())
	{
		// 跳出一层循环状态
		if (EvaluateCondition(CurEvent_BREAK->Condition))
		{
			BREAK();
		}
	}
	else if (const F_SequenceEvent_LOOP* CurEvent_LOOP = CurEventStruct.GetPtr<F_SequenceEvent_LOOP>())
	{
		// 循环状态
		if (EvaluateCondition(CurEvent_LOOP->Condition))
		{
			LOOP(CurEvent_LOOP->State);
		}
	}
	else if (const F_SequenceEvent_IF* CurEvent_IF = CurEventStruct.GetPtr<F_SequenceEvent_IF>())
	{
		// 进入条件分支判断
		if (EvaluateCondition(CurEvent_IF->Condition))
		{
			GOTO(CurEvent_IF->TrueEventsStartIndex);
		}
		else
		{
			GOTO(CurEvent_IF->FalseEventsStartIndex);
		}
	}
	else if (const F_SequenceEvent_SWITCH* CurEvent_SWITCH = CurEventStruct.GetPtr<F_SequenceEvent_SWITCH>())
	{
		for (auto& E : CurEvent_SWITCH->EventCases)
		{
			FSequenceCondition Condition = FSequenceCondition(CurEvent_SWITCH->PropertyName, ESequenceConditionOperator::Equal, E.ComparisonValue);
			if (EvaluateCondition(Condition))
			{
				// 这部分需要严格测试，跳出去有跳回来感觉有点怪。可能Switch的解析还需要更平铺直叙
				GOTO(E.CaseEventIndex);
				if (E.AutoBreak)
				{
					break;
				}
			}
		}
		// 错误。如果有Wait还会直接跳出，需要平铺解析混合Goto
		GOTO(CurEvent_SWITCH->EndIndex);
	}
	else if (const F_SequenceEvent_RETURN* CurEvent_RETURN = CurEventStruct.GetPtr<F_SequenceEvent_RETURN>())
	{
		// 结束该序列
		if (EvaluateCondition(CurEvent_RETURN->Condition))
		{
			Exit();
		}
	}
	// 具体事件
	else if (FBaseSequenceEvent* Event = CurEventStruct.GetMutablePtr<FBaseSequenceEvent>())
	{
		EEventState CurEventState = Event->GetState();
		if (CurEventState == EEventState::Pending)
		{
			Event->OnEnter();
		}
		if (CurEventState == EEventState::Active)
		{
			PreEventRemainTime = Event->Tick(DeltaTime, PreEventRemainTime);
		}
		if (CurEventState == EEventState::CurFinished)
		{
			Event->OnFinished();
			CurEventIndex++;
            
			if (CurEventIndex >= EventQueue.Num())
			{
				UE_LOG(LogTemp, Log, TEXT("Sequence Completed!"));
				CurEventIndex = INDEX_NONE;
				bCompleted = true;
			}
		}
	}
}
