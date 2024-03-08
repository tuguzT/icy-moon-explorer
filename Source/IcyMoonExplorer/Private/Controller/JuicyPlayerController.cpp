#include "Controller/JuicyPlayerController.h"

#include "AI/JuicyAILibrary.h"

AJuicyPlayerController::AJuicyPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AJuicyPlayerController::SetTeamId_Implementation(const uint8 InTeamID)
{
	if (Execute_GetTeamId(this) != InTeamID)
	{
		TeamID = InTeamID;
		UJuicyAILibrary::UpdateSource(GetPawn());
	}
}

uint8 AJuicyPlayerController::GetTeamId_Implementation() const
{
	return TeamID;
}
