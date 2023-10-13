#include "Character/JuicyCharacter.h"

#include "Character/UJuicyCharacterMovementComponent.h"

namespace Detail
{
	static const FObjectInitializer& SetCharacterMovementToJuicy(const FObjectInitializer& Initializer)
	{
		const auto Name = ACharacter::CharacterMovementComponentName;
		return Initializer.SetDefaultSubobjectClass<UJuicyCharacterMovementComponent>(Name);
	}
}

AJuicyCharacter::AJuicyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(Detail::SetCharacterMovementToJuicy(ObjectInitializer))
{
	JuicyCharacterMovement = Cast<UJuicyCharacterMovementComponent>(GetCharacterMovement());
}
