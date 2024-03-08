#include "Controller/JuicyDetourCrowdAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

AJuicyDetourCrowdAIController::AJuicyDetourCrowdAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}
