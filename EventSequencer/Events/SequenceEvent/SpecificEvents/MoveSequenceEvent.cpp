#include "MoveSequenceEvent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

FString FMoveSequenceEvent::GetDisplayName() const
{
	return Super::GetDisplayName() + FString::Printf(TEXT("Move ApproachDistance: %f"), Property.ApproachDistance);
}

int FMoveSequenceEvent::GetEventsCount()
{
	int SuperNum = Super::GetEventsCount();
	return SuperNum + 1;
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
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Execute")));
	return true;
}

float FMoveSequenceEvent::Tick(float DeltaTime, float PreRemainTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, *FString::Printf(TEXT("MoveSequenceEvent::Tick")));

	if (!IsValid(NPCPawn))
	{
		OnFinished();
		return 0;
	}
		
	FVector CurLocation = NPCPawn->GetActorLocation();
    
	if (FVector::Distance(CurLocation, Property.TargetLocation) < Property.ApproachDistance)
	{
		OnFinished();
	}
	else if (!bMoving)
	{
		if (AController* Controller = NPCPawn->GetController())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Property.TargetLocation);
		}
		bMoving = true;
	}

	return 0;
}
