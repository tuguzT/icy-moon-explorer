#include "AI/JuicyAITeam.h"

void IJuicyAITeam::SetTeamId_Implementation(const uint8 TeamID)
{
	IGenericTeamAgentInterface::SetGenericTeamId(TeamID);
}

uint8 IJuicyAITeam::GetTeamId_Implementation() const
{
	return IGenericTeamAgentInterface::GetGenericTeamId();
}

ETeamAttitude::Type IJuicyAITeam::GetTeamAttitudeTowards_Implementation(const AActor* Other) const
{
	return IsValid(Other)
		       ? IGenericTeamAgentInterface::GetTeamAttitudeTowards(*Other)
		       : ETeamAttitude::Type::Neutral;
}

void IJuicyAITeam::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	const auto This = Cast<UObject>(this);
	Execute_SetTeamId(This, TeamID);
}

FGenericTeamId IJuicyAITeam::GetGenericTeamId() const
{
	const auto This = Cast<UObject>(this);
	return Execute_GetTeamId(This);
}

ETeamAttitude::Type IJuicyAITeam::GetTeamAttitudeTowards(const AActor& Other) const
{
	const auto This = Cast<UObject>(this);
	return Execute_GetTeamAttitudeTowards(This, &Other);
}
