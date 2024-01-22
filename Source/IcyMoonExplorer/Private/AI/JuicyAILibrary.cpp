#include "AI/JuicyAILibrary.h"

ETeamAttitude::Type UJuicyAILibrary::GetTeamAttitudeTowards(const AActor* Target, const AActor* Other)
{
	const auto TargetTeamAgent = Cast<const IGenericTeamAgentInterface>(Target);
	if (!TargetTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	const auto OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(Other);
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	const auto TargetTeamId = TargetTeamAgent->GetGenericTeamId();
	const auto OtherTeamId = OtherTeamAgent->GetGenericTeamId();
	return FGenericTeamId::GetAttitude(TargetTeamId, OtherTeamId);
}
