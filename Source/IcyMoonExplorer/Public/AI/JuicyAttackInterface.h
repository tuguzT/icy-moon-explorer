#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JuicyAttackInterface.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE(FOnAttackEndSignature);

UINTERFACE(BlueprintType, Blueprintable, Category="AI")
class UJuicyAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class ICYMOONEXPLORER_API IJuicyAttackInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI")
	void Attack(AActor* ActorToAttack);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI")
	void BindEventToOnAttackEnd(const FOnAttackEndSignature& Event);
};
