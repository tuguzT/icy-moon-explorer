#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/JuicyAITeam.h"
#include "JuicyAIController.generated.h"

UCLASS()
class ICYMOONEXPLORER_API AJuicyAIController : public AAIController, public IJuicyAITeam
{
	GENERATED_BODY()

public:
	explicit AJuicyAIController(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetTeamId_Implementation(uint8 InTeamID) override;
	virtual uint8 GetTeamId_Implementation() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards_Implementation(const AActor* Other) const override;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
