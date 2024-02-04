#include "Item/NestedActors.h"

FNestedActors::FNestedActors(const TArray<AActor*>& Actors)
	: Actors{Actors}
{
}

FNestedActors::~FNestedActors()
{
}
