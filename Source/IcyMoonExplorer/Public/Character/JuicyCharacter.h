#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JuicyCharacter.generated.h"

class UJuicyCharacterMovementComponent;

UCLASS()
class ICYMOONEXPLORER_API AJuicyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	explicit AJuicyCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UJuicyCharacterMovementComponent> JuicyCharacterMovement;
};
