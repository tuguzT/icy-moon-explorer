#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "JuicyAITeam.generated.h"

UINTERFACE(BlueprintType, Blueprintable, Category="AI", DisplayName="Juicy AI Team")
class UJuicyAITeam : public UGenericTeamAgentInterface
{
	GENERATED_BODY()
};

class ICYMOONEXPLORER_API IJuicyAITeam : public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Team")
	void SetTeamId(uint8 TeamID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Team")
	uint8 GetTeamId() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Team")
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Other) const;

	virtual void SetTeamId_Implementation(uint8 TeamID);
	virtual uint8 GetTeamId_Implementation() const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards_Implementation(const AActor* Other) const;

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
