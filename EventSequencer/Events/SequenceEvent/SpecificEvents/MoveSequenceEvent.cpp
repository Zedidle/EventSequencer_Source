#include "MoveSequenceEvent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

FString FMoveSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Move ApproachDistance: %f"), Property.ApproachDistance);
}

void FMoveSequenceEvent::OnEnter()
{
	FBaseSequenceEvent::OnEnter();
		
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::OnEnter")));
	if (NPCPawn)
	{
		StartLocation = NPCPawn->GetActorLocation();
	}
}

bool FMoveSequenceEvent::Execute(int Index)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Next")));
	return true;
}

void FMoveSequenceEvent::Tick(float DeltaTime)
{
	FBaseSequenceEvent::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Tick")));

	if (!IsValid(NPCPawn))
	{
		State = EEventState::Completed;
		return;
	}
		
	FVector CurLocation = NPCPawn->GetActorLocation();
    
	if (FVector::Distance(CurLocation, Property.TargetLocation) < Property.ApproachDistance)
	{
		State = EEventState::Completed;
	}
	else if (!bMoving)
	{
		if (AController* Controller = NPCPawn->GetController())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Property.TargetLocation);
		}
		bMoving = true;
	}
}
