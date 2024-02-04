#include "Save/JuicySaveGameItemEquipment.h"

FJuicySaveGameItemEquipment::FJuicySaveGameItemEquipment(const int32 ItemIndex,
                                                         const FName SlotId,
                                                         const FGameplayTagContainer& AcceptableSlots)
	: ItemIndex{ItemIndex},
	  SlotId{SlotId},
	  AcceptableSlots{AcceptableSlots}
{
}

FJuicySaveGameItemEquipment::~FJuicySaveGameItemEquipment()
{
}
