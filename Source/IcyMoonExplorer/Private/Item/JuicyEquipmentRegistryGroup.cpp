#include "Item/JuicyEquipmentRegistryGroup.h"

FJuicyEquipmentRegistryGroup::FJuicyEquipmentRegistryGroup()
{
}

FJuicyEquipmentRegistryGroup::~FJuicyEquipmentRegistryGroup()
{
}

FJuicyEquipmentRegistryGroup::FJuicyEquipmentRegistryGroup(const FGameplayTag& TagSlotType,
                                                           const FGameplayTagContainer& Labels)
	: TagSlotType(TagSlotType),
	  Labels(Labels)
{
}

uint32 GetTypeHash(const FJuicyEquipmentRegistryGroup& Group)
{
	uint32 Hash = GetTypeHash(Group.TagSlotType);
	for (const FGameplayTag& Label : Group.Labels)
	{
		const uint32 LabelHash = GetTypeHash(Label);
		Hash = HashCombineFast(Hash, LabelHash);
	}
	return Hash;
}
