#include "Controller/JuicyAIController.h"

#include "AI/JuicyAILibrary.h"

AJuicyAIController::AJuicyAIController(const FObjectInitializer& ObjectInitializer)
	: Super{ObjectInitializer}
{
}

void AJuicyAIController::SetTeamId_Implementation(const uint8 InTeamID)
{
	if (Execute_GetTeamId(this) != InTeamID)
	{
		Super::SetGenericTeamId(InTeamID);
		UJuicyAILibrary::UpdateSource(GetPawn());
	}
}

uint8 AJuicyAIController::GetTeamId_Implementation() const
{
	return Super::GetGenericTeamId();
}

ETeamAttitude::Type AJuicyAIController::GetTeamAttitudeTowards_Implementation(const AActor* Other) const
{
	return IsValid(Other)
		       ? Super::GetTeamAttitudeTowards(*Other)
		       : ETeamAttitude::Type::Neutral;
}

void AJuicyAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Execute_SetTeamId(this, InTeamID);
}

FGenericTeamId AJuicyAIController::GetGenericTeamId() const
{
	return Execute_GetTeamId(this);
}

ETeamAttitude::Type AJuicyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	return Execute_GetTeamAttitudeTowards(this, &Other);
}
