#include "Item/JuicyEquipmentManager.h"

#include "Data/AGRLibrary.h"

UJuicyEquipmentManager::UJuicyEquipmentManager()
{
}

TMap<FJuicyEquipmentRegistryGroup, FNestedActors> UJuicyEquipmentManager::GetEquipmentRegistry_Implementation() const
{
	return EquipmentRegistry;
}

bool UJuicyEquipmentManager::GetAllItemsFromRegistry_Implementation(
	TArray<AActor*>& OutItems) const
{
	OutItems.Reset();

	for (const auto& Record : EquipmentRegistry)
	{
		for (const TArray<AActor*>& Items = Record.Value.Actors;
		     AActor* const Item : Items)
		{
			if (IsValid(Item))
			{
				OutItems.Add(Item);
			}
		}
	}

	return !OutItems.IsEmpty();
}

bool UJuicyEquipmentManager::GetAllItemsOfGroupFromRegistry_Implementation(
	const FJuicyEquipmentRegistryGroup Group,
	TArray<AActor*>& OutItems) const
{
	OutItems.Reset();

	if (const FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
		NestedItems != nullptr)
	{
		const TArray<AActor*>& Items = NestedItems->Actors;
		OutItems.Reset(Items.Num());

		for (AActor* const Item : Items)
		{
			if (IsValid(Item))
			{
				OutItems.Add(Item);
			}
		}
	}

	return !OutItems.IsEmpty();
}

bool UJuicyEquipmentManager::GetItemOfGroupFromRegistry_Implementation(
	const FJuicyEquipmentRegistryGroup Group, const int32 Index,
	AActor*& OutItem) const
{
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	AActor* const Item = Items[Index];
	if (!IsValid(Item))
	{
		return false;
	}

	OutItem = Item;
	return true;
}

bool UJuicyEquipmentManager::IsRegistryEmpty_Implementation() const
{
	for (const auto& Record : EquipmentRegistry)
	{
		if (const TArray<AActor*>& Items = Record.Value.Actors;
			!Items.IsEmpty())
		{
			return false;
		}
	}

	return true;
}

bool UJuicyEquipmentManager::FindItemInRegistry_Implementation(
	AActor* const Item, const FGameplayTagContainer GroupLabels,
	FJuicyEquipmentRegistryGroup& OutGroup, int32& OutIndex) const
{
	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FJuicyEquipmentRegistryGroup Group{ItemComponent->ItemTagSlotType, GroupLabels};
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (const bool ItemFound = Items.Find(Item, OutIndex);
		!ItemFound)
	{
		return false;
	}

	OutGroup = Group;
	return true;
}

bool UJuicyEquipmentManager::IsItemRegistered_Implementation(
	AActor* const Item, const FGameplayTagContainer GroupLabels) const
{
	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FJuicyEquipmentRegistryGroup Group{ItemComponent->ItemTagSlotType, GroupLabels};
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	return Items.Contains(Item);
}

bool UJuicyEquipmentManager::IsItemOfGroupRegistered_Implementation(
	const FJuicyEquipmentRegistryGroup Group, const int32 Index) const
{
	const FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	const TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	const AActor* Item = Items[Index];
	return IsValid(Item);
}

void UJuicyEquipmentManager::SetupDefineRegistry_Implementation(
	const TMap<FJuicyEquipmentRegistryGroup, FNestedActors>& Registry)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return;
	}

	EquipmentRegistry = Registry;
}

bool UJuicyEquipmentManager::RegisterItem_Implementation(
	AActor* const Item, const FGameplayTagContainer GroupLabels,
	FJuicyEquipmentRegistryGroup& OutGroup, int32& OutIndex)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FJuicyEquipmentRegistryGroup Group{ItemComponent->ItemTagSlotType, GroupLabels};
	FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	OutIndex = Items.AddUnique(Item);
	OutGroup = Group;
	return true;
}

bool UJuicyEquipmentManager::RegisterItemByIndex_Implementation(
	AActor* const Item, const FGameplayTagContainer GroupLabels, const int32 Index,
	FJuicyEquipmentRegistryGroup& OutGroup)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FJuicyEquipmentRegistryGroup Group{ItemComponent->ItemTagSlotType, GroupLabels};
	FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	if (Items.Contains(Item))
	{
		return false;
	}

	Items.Insert(Item, Index);
	OutGroup = Group;
	return true;
}

bool UJuicyEquipmentManager::UnregisterItem_Implementation(
	AActor* const Item, const FGameplayTagContainer GroupLabels,
	FJuicyEquipmentRegistryGroup& OutGroup)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	const UAGR_ItemComponent* ItemComponent = UAGRLibrary::GetItemComponent(Item);
	if (ItemComponent == nullptr)
	{
		return false;
	}

	const FJuicyEquipmentRegistryGroup Group{ItemComponent->ItemTagSlotType, GroupLabels};
	FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	Items.RemoveSingle(Item);
	OutGroup = Group;
	return true;
}

bool UJuicyEquipmentManager::UnregisterItemOfGroup_Implementation(
	const FJuicyEquipmentRegistryGroup Group, const int32 Index)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return false;
	}

	FNestedActors* NestedItems = EquipmentRegistry.Find(Group);
	if (NestedItems == nullptr)
	{
		return false;
	}

	TArray<AActor*>& Items = NestedItems->Actors;
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	Items.RemoveAt(Index);
	return true;
}

bool UJuicyEquipmentManager::EquipItemFromRegistryInSlot_Implementation(
	const FName Slot, AActor* const Item, const FGameplayTagContainer GroupLabels,
	AActor*& OutPreviousItem, AActor*& OutNewItem)
{
	if (!IsItemRegistered(Item, GroupLabels))
	{
		return false;
	}

	return Super::EquipItemInSlot(Slot, Item, OutPreviousItem, OutNewItem);
}

bool UJuicyEquipmentManager::EquipItemOfGroupFromRegistryInSlot_Implementation(
	const FName Slot, const FJuicyEquipmentRegistryGroup Group, const int32 Index,
	AActor*& OutPreviousItem, AActor*& OutNewItem)
{
	AActor* Item;
	if (!GetItemOfGroupFromRegistry(Group, Index, Item))
	{
		return false;
	}

	return Super::EquipItemInSlot(Slot, Item, OutPreviousItem, OutNewItem);
}
