// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EventSequenceComponent.h"
#include "DataAssets/EventSequenceDA.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "Events/EventSequenceRunning.h"
#include "Events/SequenceEvent_BlueprintCall.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EventSequenceSystem.generated.h"


class UPropertyBagWrapper;

UCLASS()
class EVENTSEQUENCER_API UEventSequenceSystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
	
    UPROPERTY()
    TArray<UEventSequenceRunning*> EventSequencesRunning;

    UPROPERTY()
    TArray<TWeakObjectPtr<UEventSequenceComponent>> EventSequenceComponents;

    void ParseEventSequence(UEventSequenceRunning* EventSequenceRunning, const TArray<FInstancedStruct>& EventSequence);
    
public:
    static UEventSequenceSystem* GetInstance(UWorld* World);
    
    virtual void Tick(float DeltaTime) override;
    
    virtual TStatId GetStatId() const override;
    
    UFUNCTION()
    void TickSequences(float DeltaTime);

    // PropertyWrapper 开放给蓝图的话，需要在蓝图中创建并设置值，值多的话会占用不少面积
    UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
    UEventSequenceRunning* CreateEventSequence(UEventSequenceDA* TargetDataAsset, UEventSequenceComponent* Component = nullptr, UPropertyBagWrapper* PropertyBagInput = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Event Sequencer")
    bool RemoveEventSequence(UEventSequenceRunning* EventSequence);

    UFUNCTION()
    bool RemoveComponent(const UEventSequenceComponent* Component);
    
};




// 以下是参考目录

/************************************************************************/
/* 核心枚举：异步命令执行结果/中断策略                                   */
/************************************************************************/
// UENUM(BlueprintType)
// enum class EAsyncExecuteResult : uint8
// {
//     Success,  // 执行成功
//     Failed,   // 执行失败
//     Timeout   // 执行超时
// };
//
// UENUM(BlueprintType)
// enum class EAsyncInterruptPolicy : uint8
// {
//     Restart,  // 中断后重启异步行为
//     Skip      // 中断后直接视为失败
// };

/************************************************************************/
/* 解释器变量：支持多类型、初始值、外部覆写、隐藏配置                   */
/************************************************************************/
// USTRUCT(BlueprintType)
// struct FEventSequenceVarDef
// {
//     GENERATED_BODY()
//
//     // 变量名（唯一标识）
//     UPROPERTY(EditAnywhere, Category = "变量定义")
//     FName VarName = NAME_None;
//
//     // 变量类型（支持UE所有基础类型+自定义结构体）
//     // UPROPERTY(EditAnywhere, Category = "变量定义")
//     FProperty* VarType = nullptr;
//
//     // 常量初始值（通过StructUtils序列化）
//     UPROPERTY(EditAnywhere, Category = "变量定义", meta = (EditCondition = "!bIsReference"))
//     FInstancedStruct DefaultValue;
//
//     // 是否从外部传入（外部覆写）
//     UPROPERTY(EditAnywhere, Category = "变量定义")
//     bool bIsReference = false;
//
//     // 是否对外部隐藏（蓝图节点中不显示）
//     UPROPERTY(EditAnywhere, Category = "变量定义")
//     bool bIsHidden = false;
//
//     // 检查变量定义是否合法
//     bool IsValid() const { return VarName != NAME_None && VarType != nullptr; }
// };

// USTRUCT(BlueprintType)
// struct FEventSequenceVariables
// {
//     GENERATED_BODY()
//
//     // 变量定义列表
//     UPROPERTY(EditAnywhere, Category = "解释器变量")
//     TArray<FEventSequenceVarDef> VarDefs;
//
//     // 运行时变量值（键：变量名，值：实例化结构）
//     TMap<FName, FInstancedStruct> RuntimeValues;
//
//     // 初始化运行时变量（从默认值/外部覆写）
//     void Init(const TMap<FName, FInstancedStruct>& ExternalOverrides = {});
//
//     // 获取变量值
//     template<typename T>
//     T* GetVarValue(FName VarName)
//     {
//         if (RuntimeValues.Contains(VarName))
//         {
//             return RuntimeValues[VarName].GetPtr<T>();
//         }
//         return nullptr;
//     }
//
//     // 设置变量值
//     template<typename T>
//     void SetVarValue(FName VarName, const T& Value)
//     {
//         FInstancedStruct& Struct = RuntimeValues.FindOrAdd(VarName);
//         Struct.InitializeAs<T>();
//         *Struct.GetPtr<T>() = Value;
//     }
//
//     // 检查变量是否存在
//     bool HasVar(FName VarName) const { return RuntimeValues.Contains(VarName); }
// };

/************************************************************************/
/* 命令基类：所有事件命令的统一接口（基于FInstancedStruct）             */
/************************************************************************/
// USTRUCT(BlueprintType)
// struct FEventSequenceCommand
// {
//     GENERATED_BODY()
//
//     FEventSequenceCommand() = default;
//     virtual ~FEventSequenceCommand() = default;
//
//     // 命令显示名称（编辑器中显示）
//     UPROPERTY(EditAnywhere, Category = "基础配置")
//     FText DisplayName = FText::GetEmpty();
//
//     // 【核心】执行命令（解释器调用，返回是否需要等待/是否执行成功）
//     // Interpreter：所属解释器
//     // CmdIndex：当前指令在扁平数组中的索引
//     // OutNextCmdIndex：下一条指令的索引（用于跳转/分支）
//     virtual bool Execute(UEventSequenceInterpreter* Interpreter, int32 CmdIndex, int32& OutNextCmdIndex) { return false; } const
//
//     // 【核心】扁平化解命令（将嵌套命令转换为线性指令，添加到扁平数组）
//     // FlattenedCmds：输出的扁平指令数组
//     // LabelMap：Label与指令索引的映射表
//     virtual void Flatten(TArray<FInstancedStruct>& FlattenedCmds, TMap<FName, int32>& LabelMap) {}
//
//     // 检查命令配置是否合法
//     virtual bool IsValid() const { return !DisplayName.IsEmpty(); }
//
//     // 获取命令的类型名称（编辑器显示）
//     virtual FText GetCommandTypeName() const { return FText::FromString(TEXT("基础命令")); }
// };

// 注册命令基类为可实例化结构（关键：支持FInstancedStruct）
// STRUCT_INSTANCED_TYPE_LAYOUT(FEventSequenceCommand);

/************************************************************************/
/* 事件序列DataAsset：策划编辑的核心资产，存储序列配置和命令列表         */
/************************************************************************/
// UCLASS()
// class UEventSequenceDataAsset2 : public UDataAsset
// {
//     GENERATED_BODY()
//
// public:
//     // 事件序列名称
//     UPROPERTY(EditAnywhere, Category = "事件序列配置")
//     FText SequenceName = FText::GetEmpty();
//
//     // Context类型限制（仅允许指定类型的UObject作为Context）
//     // UPROPERTY(EditAnywhere, Category = "事件序列配置", meta = (MetaClass = "UObject"))
//     UClass* ContextClass = UObject::StaticClass();
//
//     // 解释器变量定义（策划配置）
//     UPROPERTY(EditAnywhere, Category = "事件序列配置")
//     FEventSequenceVariables SequenceVariables;
//
//     // 根命令列表（嵌套的命令树，策划拖拽配置）
//     UPROPERTY(EditAnywhere, Category = "事件序列命令", meta = (InstancedStructClass = "FEventSequenceCommand"))
//     TArray<FInstancedStruct> RootCommands;
//
//     // 单次执行的最大指令数（防止无限循环）
//     UPROPERTY(EditAnywhere, Category = "事件序列配置", meta = (ClampMin = 1, ClampMax = 1000))
//     int32 MaxExecuteCountPerUpdate = 50;
//
//     // 检查事件序列是否合法
//     UFUNCTION(BlueprintPure, Category = "EventSequence")
//     bool IsSequenceValid() const;
// };

/************************************************************************/
/* 解释器核心类：驱动事件序列执行，管理状态/变量/等待条件               */
/************************************************************************/
// UCLASS(BlueprintType, Blueprintable)
// class UEventSequenceInterpreter : public UObject
// {
//     GENERATED_BODY()
//
// public:
//     UEventSequenceInterpreter();
//
//     /************************************************************************/
//     /* 外部接口：蓝图/C++调用                                               */
//     /************************************************************************/
//     // 初始化解释器（必选：传入DataAsset和Context，可选：外部覆写变量）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     bool InitInterpreter(UEventSequenceDataAsset2* InSequenceDA, UObject* InContext, const TMap<FName, FInstancedStruct>& ExternalVarOverrides);
//
//     // 驱动解释器执行（核心：外部每帧调用，如Tick）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     void Update(float DeltaTime);
//
//     // 停止解释器执行（强制结束）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     void StopInterpreter(bool bIsNormalQuit = false);
//
//     // 检查解释器是否正在运行
//     UFUNCTION(BlueprintPure, Category = "EventSequence|Interpreter")
//     bool IsRunning() const { return bIsRunning; }
//
//     // 检查解释器是否处于等待状态
//     UFUNCTION(BlueprintPure, Category = "EventSequence|Interpreter")
//     bool IsWaiting() const { return bIsWaiting; }
//
//     /************************************************************************/
//     /* 核心成员：供命令/蓝图调用                                           */
//     /************************************************************************/
//     // 获取Context（蓝图命令的核心入口）
//     UFUNCTION(BlueprintPure, Category = "EventSequence|Interpreter")
//     UObject* GetContext() const { return Context; }
//
//     // 获取世界上下文（从Context获取）
//     UFUNCTION(BlueprintPure, Category = "EventSequence|Interpreter")
//     UWorld* GetWorldContext() const;
//
//     // 获取解释器变量（读写）
//     // UFUNCTION(BlueprintPure, Category = "EventSequence|Interpreter")
//     FEventSequenceVariables* GetInterpreterVariables() { return &InterpreterVariables; }
//
//     // 异步命令完成回调（供异步蓝图调用Resolve/Reject）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     void OnAsyncCommandComplete(EAsyncExecuteResult Result, const FInstancedStruct& ResultData = FInstancedStruct());
//
//     // 设置等待状态（命令调用，进入等待）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     void SetWaitingState(bool bInWaiting, float WaitTime = 0.0f) { bIsWaiting = bInWaiting; RemainingWaitTime = FMath::Max(0.0f, WaitTime); }
//
//     // 设置异步中断策略
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Interpreter")
//     void SetAsyncInterruptPolicy(EAsyncInterruptPolicy Policy) { CurrentAsyncInterruptPolicy = Policy; }
//
//     /************************************************************************/
//     /* 序列化支持：存档/读档时保存执行状态                                 */
//     /************************************************************************/
//     // virtual void Serialize(FArchive& Ar) override;
//
//     // 执行状态：供序列化使用
//     UPROPERTY(SaveGame)
//     bool bIsRunning = false;
//     UPROPERTY(SaveGame)
//     bool bIsWaiting = false;
//     UPROPERTY(SaveGame)
//     int32 CurrentCmdIndex = 0; // 当前执行的扁平指令索引
//     UPROPERTY(SaveGame)
//     float RemainingWaitTime = 0.0f; // 剩余等待时间（等待命令用）
//     UPROPERTY(SaveGame)
//     EAsyncInterruptPolicy CurrentAsyncInterruptPolicy = EAsyncInterruptPolicy::Restart; // 当前异步命令的中断策略
//
// private:
//     /************************************************************************/
//     /* 内部逻辑：扁平化解/指令执行                                          */
//     /************************************************************************/
//     // 扁平化解命令树（启动时执行，建立Label映射）
//     void FlattenCommandTree();
//
//     // 执行单条指令
//     bool ExecuteSingleCommand(int32 CmdIndex, int32& OutNextCmdIndex);
//
//     /************************************************************************/
//     /* 核心成员变量                                                        */
//     /************************************************************************/
//     UPROPERTY()
//     UEventSequenceDataAsset2* SequenceDA = nullptr; // 关联的DataAsset
//     UPROPERTY()
//     UObject* Context = nullptr; // 上下文（非序列化，读档后重新绑定）
//     FEventSequenceVariables InterpreterVariables; // 运行时解释器变量
//     TArray<FInstancedStruct> FlattenedCommands; // 扁平后的指令数组
//     TMap<FName, int32> LabelIndexMap; // Label->扁平指令索引的映射
//     int32 ExecuteCountInCurrentUpdate = 0; // 本次Update的执行指令数
// };

/************************************************************************/
/* 同步蓝图行为基类：策划继承后实现同步逻辑，支持In/Out变量绑定          */
/************************************************************************/
// UCLASS(Abstract, Blueprintable, BlueprintType)
// class UEventSequenceSyncBlueprint : public UObject
// {
//     GENERATED_BODY()
//
// public:
//     // 执行同步行为（核心：策划蓝图重载）
//     UFUNCTION(BlueprintNativeEvent, Category = "EventSequence|SyncBlueprint")
//     void Execute();
//     virtual void Execute_Implementation() {}
//
//     // 获取所属解释器
//     UFUNCTION(BlueprintPure, Category = "EventSequence|SyncBlueprint")
//     UEventSequenceInterpreter* GetInterpreter() const { return Interpreter; }
//
//     // 获取Context（从解释器继承）
//     UFUNCTION(BlueprintPure, Category = "EventSequence|SyncBlueprint")
//     UObject* GetContext() const { return Interpreter ? Interpreter->GetContext() : nullptr; }
//
//     // 绑定解释器（解释器调用，内部使用）
//     void BindInterpreter(UEventSequenceInterpreter* InInterpreter) { Interpreter = InInterpreter; }
//
// private:
//     UPROPERTY()
//     UEventSequenceInterpreter* Interpreter = nullptr;
// };

/************************************************************************/
/* 异步蓝图行为基类：策划继承后实现异步逻辑，基于Promise的Resolve/Reject */
/************************************************************************/
// UCLASS(Abstract, Blueprintable, BlueprintType)
// class UEventSequenceAsyncBlueprint : public UObject
// {
//     GENERATED_BODY()
//
// public:
//     // 启动异步行为（核心：策划蓝图重载）
//     UFUNCTION(BlueprintNativeEvent, Category = "EventSequence|AsyncBlueprint")
//     void OnExecute();
//     virtual void OnExecute_Implementation() {}
//
//     // 【Promise】执行成功，回调解释器
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|AsyncBlueprint")
//     void Resolve(const FInstancedStruct& ResultData = FInstancedStruct());
//
//     // 【Promise】执行失败/超时，回调解释器
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|AsyncBlueprint")
//     void Reject(EAsyncExecuteResult FailReason = EAsyncExecuteResult::Failed, const FInstancedStruct& ResultData = FInstancedStruct());
//
//     // 获取所属解释器
//     UFUNCTION(BlueprintPure, Category = "EventSequence|AsyncBlueprint")
//     UEventSequenceInterpreter* GetInterpreter() const { return Interpreter; }
//
//     // 获取Context（从解释器继承）
//     UFUNCTION(BlueprintPure, Category = "EventSequence|AsyncBlueprint")
//     UObject* GetContext() const { return Interpreter ? Interpreter->GetContext() : nullptr; }
//
//     // 绑定解释器（解释器调用，内部使用）
//     void BindInterpreter(UEventSequenceInterpreter* InInterpreter) { Interpreter = InInterpreter; }
//
//     // 设置中断策略
//     void SetInterruptPolicy(EAsyncInterruptPolicy Policy) { InterruptPolicy = Policy; }
//
// private:
//     UPROPERTY()
//     UEventSequenceInterpreter* Interpreter = nullptr;
//     EAsyncInterruptPolicy InterruptPolicy = EAsyncInterruptPolicy::Restart;
// };

/************************************************************************/
/* 蓝图函数库：提供全局的事件序列操作接口                               */
/************************************************************************/
// UCLASS()
// class UEventSequenceBlueprintLibrary : public UBlueprintFunctionLibrary
// {
//     GENERATED_BODY()
//
// public:
//     // 创建解释器实例（可挂载到任意UObject）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Library", meta = (WorldContext = "WorldContextObject"))
//     static UEventSequenceInterpreter* CreateInterpreter(UObject* WorldContextObject);
//
//     // 启动事件序列（一键创建+初始化+执行，快捷接口）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Library", meta = (WorldContext = "WorldContextObject"))
//     static UEventSequenceInterpreter* StartEventSequence(UObject* WorldContextObject, UEventSequenceDataAsset2* SequenceDA, UObject* Context, const TMap<FName, FInstancedStruct>& ExternalVarOverrides);
//
//     // 解释器变量赋值（蓝图友好的通用接口）
//     UFUNCTION(BlueprintCallable, Category = "EventSequence|Library")
//     static void SetInterpreterVar(UEventSequenceInterpreter* Interpreter, FName VarName, const FInstancedStruct& Value);
//
//     // 解释器变量取值（蓝图友好的通用接口）
//     UFUNCTION(BlueprintPure, Category = "EventSequence|Library")
//     static FInstancedStruct GetInterpreterVar(UEventSequenceInterpreter* Interpreter, FName VarName);
// };