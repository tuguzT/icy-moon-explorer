#pragma once

#include "CoreMinimal.h"
#include "AI/JuicyAITeam.h"
#include "GameFramework/PlayerController.h"
#include "JuicyPlayerController.generated.h"

UCLASS()
class ICYMOONEXPLORER_API AJuicyPlayerController : public APlayerController, public IJuicyAITeam
{
	GENERATED_BODY()

public:
	explicit AJuicyPlayerController(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetTeamId_Implementation(uint8 InTeamID) override;
	virtual uint8 GetTeamId_Implementation() const override;

private:
	FGenericTeamId TeamID;
};
