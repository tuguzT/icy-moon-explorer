#include "Save/JuicySaveGameNestedItems.h"

FJuicySaveGameNestedItems::FJuicySaveGameNestedItems(const TArray<FJuicySaveGameItemInventory>& ItemIndices)
	: ItemIndices{ItemIndices}
{
}

FJuicySaveGameNestedItems::~FJuicySaveGameNestedItems()
{
}
