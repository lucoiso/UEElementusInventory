// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "Components/ElementusInventoryComponent.h"
#include "Management/ElementusInventoryFunctions.h"
#include "Management/ElementusInventorySettings.h"
#include "LogElementusInventory.h"
#include <Engine/AssetManager.h>
#include <GameFramework/Actor.h>
#include <Algo/ForEach.h>
#include <Net/UnrealNetwork.h>
#include <Net/Core/PushModel/PushModel.h>

#ifdef UE_INLINE_GENERATED_CPP_BY_NAME
#include UE_INLINE_GENERATED_CPP_BY_NAME(ElementusInventoryComponent)
#endif

UElementusInventoryComponent::UElementusInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	CurrentWeight(0.f), MaxWeight(0.f), MaxNumItems(0)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

	if (const UElementusInventorySettings* const Settings = UElementusInventorySettings::Get())
	{
		bAllowEmptySlots = Settings->bAllowEmptySlots;
		MaxWeight = Settings->MaxWeight;
		MaxNumItems = Settings->MaxNumItems;
	}
}

float UElementusInventoryComponent::GetCurrentWeight() const
{
	return CurrentWeight;
}

float UElementusInventoryComponent::GetMaxWeight() const
{
	return MaxWeight <= 0.f ? MAX_flt : MaxWeight;
}

int32 UElementusInventoryComponent::GetCurrentNumItems() const
{
	return ElementusItems.Num();
}

int32 UElementusInventoryComponent::GetMaxNumItems() const
{
	return MaxNumItems <= 0 ? MAX_int32 : MaxNumItems;
}

TArray<FElementusItemInfo> UElementusInventoryComponent::GetItemsArray() const
{
	return ElementusItems;
}

FElementusItemInfo& UElementusInventoryComponent::GetItemReferenceAt(const int32 Index)
{
	return ElementusItems[Index];
}

FElementusItemInfo UElementusInventoryComponent::GetItemCopyAt(const int32 Index) const
{
	return ElementusItems[Index];
}

bool UElementusInventoryComponent::CanReceiveItem(const FElementusItemInfo InItemInfo) const
{
	if (!UElementusInventoryFunctions::IsItemValid(InItemInfo))
	{
		return false;
	}

	bool bOutput = ElementusItems.Num() <= GetMaxNumItems();

	if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(InItemInfo.ItemId, {"Data"}))
	{
		bOutput = bOutput && ((GetCurrentWeight() + (ItemData->ItemWeight * InItemInfo.Quantity)) <= GetMaxWeight());
	}

	if (!bOutput)
	{
		UE_LOG(LogElementusInventory, Warning, TEXT("%s: Actor %s cannot receive %d item(s) with name '%s'"), *FString(__FUNCTION__),
		       *GetOwner()->GetName(), InItemInfo.Quantity, *InItemInfo.ItemId.ToString());
	}

	return bOutput;
}

bool UElementusInventoryComponent::CanGiveItem(const FElementusItemInfo InItemInfo) const
{
	if (!UElementusInventoryFunctions::IsItemValid(InItemInfo))
	{
		return false;
	}

	if (TArray<int32> InIndex; FindAllItemIndexesWithInfo(InItemInfo, InIndex, FGameplayTagContainer::EmptyContainer))
	{
		int32 Quantity = 0u;
		for (const int32& Index : InIndex)
		{
			Quantity += ElementusItems[Index].Quantity;
		}

		return Quantity >= InItemInfo.Quantity;
	}

	UE_LOG(LogElementusInventory, Warning, TEXT("%s: Actor %s cannot give %d item(s) with name '%s'"), *FString(__FUNCTION__), *GetOwner()->GetName(),
	       InItemInfo.Quantity, *InItemInfo.ItemId.ToString());

	return false;
}

void UElementusInventoryComponent::SortInventory(const EElementusInventorySortingMode Mode, const EElementusInventorySortingOrientation Orientation)
{
	const auto SortByOrientation = [Orientation](const auto A, const auto B)
	{
		switch (Orientation)
		{
		case EElementusInventorySortingOrientation::Ascending:
			return A < B;

		case EElementusInventorySortingOrientation::Descending:
			return A > B;

		default:
			return false;
		}
	};

	switch (Mode)
	{
	case EElementusInventorySortingMode::ID:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			return UElementusInventoryFunctions::IsItemValid(A) && SortByOrientation(A.ItemId, B.ItemId);
		});
		break;

	case EElementusInventorySortingMode::Name:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemName.ToString(), ItemDataB->ItemName.ToString());
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::Type:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemType, ItemDataB->ItemType);
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::IndividualValue:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemValue, ItemDataB->ItemValue);
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::StackValue:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemValue * A.Quantity, ItemDataB->ItemValue * B.Quantity);
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::IndividualWeight:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemWeight, ItemDataB->ItemWeight);
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::StackWeight:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			if (!UElementusInventoryFunctions::IsItemValid(A))
			{
				return false;
			}

			if (const UElementusItemData* const ItemDataA = UElementusInventoryFunctions::GetSingleItemDataById(A.ItemId, {"Data"}))
			{
				if (const UElementusItemData* const ItemDataB = UElementusInventoryFunctions::GetSingleItemDataById(B.ItemId, {"Data"}))
				{
					return SortByOrientation(ItemDataA->ItemWeight * A.Quantity, ItemDataB->ItemWeight * B.Quantity);
				}
			}

			return false;
		});
		break;

	case EElementusInventorySortingMode::Quantity:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			return UElementusInventoryFunctions::IsItemValid(A) && SortByOrientation(A.Quantity, B.Quantity);
		});
		break;

	case EElementusInventorySortingMode::Level:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			return UElementusInventoryFunctions::IsItemValid(A) && SortByOrientation(A.Level, B.Level);
		});
		break;

	case EElementusInventorySortingMode::Tags:
		ElementusItems.Sort([SortByOrientation](const FElementusItemInfo& A, const FElementusItemInfo& B)
		{
			return UElementusInventoryFunctions::IsItemValid(A) && SortByOrientation(A.Tags.Num(), B.Tags.Num());
		});
		break;

	default:
		break;
	}
}

void UElementusInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	RefreshInventory();
}

void UElementusInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UElementusInventoryComponent, ElementusItems, SharedParams);
}

void UElementusInventoryComponent::RefreshInventory()
{
	ForceWeightUpdate();
	ForceInventoryValidation();
}

void UElementusInventoryComponent::ForceWeightUpdate()
{
	float NewWeigth = 0.f;
	for (const FElementusItemInfo& Iterator : ElementusItems)
	{
		if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(Iterator.ItemId, {"Data"}))
		{
			NewWeigth += ItemData->ItemWeight * Iterator.Quantity;
		}
	}

	CurrentWeight = NewWeigth;
}

void UElementusInventoryComponent::ForceInventoryValidation()
{
	TArray<FElementusItemInfo> NewItems;
	TArray<int32> IndexesToRemove;

	for (int32 i = 0; i < ElementusItems.Num(); ++i)
	{
		if (ElementusItems[i].Quantity <= 0)
		{
			IndexesToRemove.Add(i);
		}

		else if (ElementusItems[i].Quantity > 1)
		{
			if (!UElementusInventoryFunctions::IsItemStackable(ElementusItems[i]))
			{
				for (int32 j = 0; j < ElementusItems[i].Quantity; ++j)
				{
					NewItems.Add(FElementusItemInfo(ElementusItems[i].ItemId, 1, ElementusItems[i].Tags));
				}

				IndexesToRemove.Add(i);
			}
		}
	}

	if (!UElementusInventoryFunctions::HasEmptyParam(IndexesToRemove))
	{
		for (const int32& Iterator : IndexesToRemove)
		{
			if (bAllowEmptySlots)
			{
				ElementusItems[Iterator] = FElementusItemInfo::EmptyItemInfo;
			}
			else
			{
				ElementusItems.RemoveAt(Iterator, 1, false);
			}
		}
	}
	if (!UElementusInventoryFunctions::HasEmptyParam(NewItems))
	{
		ElementusItems.Append(NewItems);
	}

	NotifyInventoryChange();
}

bool UElementusInventoryComponent::FindFirstItemIndexWithInfo(const FElementusItemInfo& InItemInfo, int32& OutIndex,
                                                              const FGameplayTagContainer& IgnoreTags, const int32 Offset) const
{
	for (int32 Iterator = Offset; Iterator < ElementusItems.Num(); ++Iterator)
	{
		FElementusItemInfo InParamCopy = InItemInfo;
		InParamCopy.Tags.RemoveTags(IgnoreTags);

		FElementusItemInfo InExistingCopy = ElementusItems[Iterator];
		InExistingCopy.Tags.RemoveTags(IgnoreTags);

		if (InExistingCopy == InParamCopy)
		{
			OutIndex = Iterator;
			return true;
		}
	}

	OutIndex = INDEX_NONE;
	return false;
}

bool UElementusInventoryComponent::FindFirstItemIndexWithTags(const FGameplayTagContainer& WithTags, int32& OutIndex,
                                                              const FGameplayTagContainer& IgnoreTags, const int32 Offset) const
{
	for (int32 Iterator = Offset; Iterator < ElementusItems.Num(); ++Iterator)
	{
		FElementusItemInfo InExistingCopy = ElementusItems[Iterator];
		InExistingCopy.Tags.RemoveTags(IgnoreTags);

		if (InExistingCopy.Tags.HasAllExact(WithTags))
		{
			OutIndex = Iterator;
			return true;
		}
	}

	OutIndex = INDEX_NONE;
	return false;
}

bool UElementusInventoryComponent::FindFirstItemIndexWithId(const FPrimaryElementusItemId& InId, int32& OutIndex,
                                                            const FGameplayTagContainer& IgnoreTags, const int32 Offset) const
{
	for (int32 Iterator = Offset; Iterator < ElementusItems.Num(); ++Iterator)
	{
		if (!ElementusItems[Iterator].Tags.HasAny(IgnoreTags) && ElementusItems[Iterator].ItemId == InId)
		{
			OutIndex = Iterator;
			return true;
		}
	}

	OutIndex = INDEX_NONE;
	return false;
}

bool UElementusInventoryComponent::FindAllItemIndexesWithInfo(const FElementusItemInfo& InItemInfo, TArray<int32>& OutIndexes,
                                                              const FGameplayTagContainer& IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		if (IgnoreTags.IsEmpty() && *Iterator == InItemInfo)
		{
			OutIndexes.Add(Iterator.GetIndex());
			continue;
		}

		FElementusItemInfo InItCopy(*Iterator);
		InItCopy.Tags.RemoveTags(IgnoreTags);

		FElementusItemInfo InParamCopy(InItemInfo);
		InParamCopy.Tags.RemoveTags(IgnoreTags);

		if (InItCopy == InParamCopy)
		{
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !UElementusInventoryFunctions::HasEmptyParam(OutIndexes);
}

bool UElementusInventoryComponent::FindAllItemIndexesWithTags(const FGameplayTagContainer& WithTags, TArray<int32>& OutIndexes,
                                                              const FGameplayTagContainer& IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		FElementusItemInfo InCopy(*Iterator);
		if (!IgnoreTags.IsEmpty())
		{
			InCopy.Tags.RemoveTags(IgnoreTags);
		}

		if (InCopy.Tags.HasAll(WithTags))
		{
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !UElementusInventoryFunctions::HasEmptyParam(OutIndexes);
}

bool UElementusInventoryComponent::FindAllItemIndexesWithId(const FPrimaryElementusItemId& InId, TArray<int32>& OutIndexes,
                                                            const FGameplayTagContainer& IgnoreTags) const
{
	for (auto Iterator = ElementusItems.CreateConstIterator(); Iterator; ++Iterator)
	{
		if (!Iterator->Tags.HasAll(IgnoreTags) && Iterator->ItemId == InId)
		{
			OutIndexes.Add(Iterator.GetIndex());
		}
	}

	return !UElementusInventoryFunctions::HasEmptyParam(OutIndexes);
}

bool UElementusInventoryComponent::ContainsItem(const FElementusItemInfo& InItemInfo, const bool bIgnoreTags) const
{
	return ElementusItems.FindByPredicate([&InItemInfo, &bIgnoreTags](const FElementusItemInfo& InInfo)
	{
		if (bIgnoreTags)
		{
			return InInfo.ItemId == InItemInfo.ItemId;
		}

		return InInfo == InItemInfo;
	}) != nullptr;
}

bool UElementusInventoryComponent::IsInventoryEmpty() const
{
	bool bOutput = true;

	for (const FElementusItemInfo& Iterator : ElementusItems)
	{
		if (Iterator.Quantity > 0)
		{
			bOutput = false;
			break;
		}
	}

	return bOutput;
}

void UElementusInventoryComponent::DebugInventory()
{
#if !UE_BUILD_SHIPPING
	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("%s"), *FString(__FUNCTION__));
	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Owning Actor: %s"), *GetOwner()->GetName());

	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Weight: %f"), CurrentWeight);
	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Num: %i"), ElementusItems.Num());
	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Size: %i"), ElementusItems.GetAllocatedSize());

	for (const FElementusItemInfo& Iterator : ElementusItems)
	{
		UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Item: %s"), *Iterator.ItemId.ToString());
		UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Quantity: %i"), Iterator.Quantity);

		for (const FGameplayTag& Tag : Iterator.Tags)
		{
			UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Tag: %s"), *Tag.ToString());
		}
	}

	UE_LOG(LogElementusInventory_Internal, Warning, TEXT("Component Memory Size: %ull"), GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal));
#endif
}

void UElementusInventoryComponent::ClearInventory_Implementation()
{
	UE_LOG(LogElementusInventory, Display, TEXT("%s: Cleaning %s's inventory"), *FString(__FUNCTION__), *GetOwner()->GetName());

	ElementusItems.Empty();
	CurrentWeight = 0.f;
}

void UElementusInventoryComponent::GetItemIndexesFrom_Implementation(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	TArray<FElementusItemInfo> Modifiers;
	for (const int32& Iterator : ItemIndexes)
	{
		if (OtherInventory->ElementusItems.IsValidIndex(Iterator))
		{
			Modifiers.Add(OtherInventory->ElementusItems[Iterator]);
		}
	}

	GetItemsFrom_Implementation(OtherInventory, Modifiers);
}

void UElementusInventoryComponent::GiveItemIndexesTo_Implementation(UElementusInventoryComponent* OtherInventory, const TArray<int32>& ItemIndexes)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	TArray<FElementusItemInfo> Modifiers;
	for (const int32& Iterator : ItemIndexes)
	{
		if (OtherInventory->ElementusItems.IsValidIndex(Iterator))
		{
			Modifiers.Add(ElementusItems[Iterator]);
		}
	}

	GiveItemsTo_Implementation(OtherInventory, Modifiers);
}

void UElementusInventoryComponent::GetItemsFrom_Implementation(UElementusInventoryComponent* OtherInventory, const TArray<FElementusItemInfo>& Items)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	if (!IsValid(OtherInventory))
	{
		return;
	}

	const TArray<FElementusItemInfo> TradeableItems = UElementusInventoryFunctions::FilterTradeableItems(OtherInventory, this, Items);

	OtherInventory->UpdateElementusItems(TradeableItems, EElementusInventoryUpdateOperation::Remove);
	UpdateElementusItems(TradeableItems, EElementusInventoryUpdateOperation::Add);
}

void UElementusInventoryComponent::GiveItemsTo_Implementation(UElementusInventoryComponent* OtherInventory, const TArray<FElementusItemInfo>& Items)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	if (!IsValid(OtherInventory))
	{
		return;
	}

	const TArray<FElementusItemInfo> TradeableItems = UElementusInventoryFunctions::FilterTradeableItems(this, OtherInventory, Items);

	UpdateElementusItems(TradeableItems, EElementusInventoryUpdateOperation::Remove);
	OtherInventory->UpdateElementusItems(TradeableItems, EElementusInventoryUpdateOperation::Add);
}

void UElementusInventoryComponent::DiscardItemIndexes_Implementation(const TArray<int32>& ItemIndexes)
{
	if (GetOwnerRole() != ROLE_Authority || UElementusInventoryFunctions::HasEmptyParam(ItemIndexes))
	{
		return;
	}

	TArray<FElementusItemInfo> Modifiers;
	for (const int32& Iterator : ItemIndexes)
	{
		if (ElementusItems.IsValidIndex(Iterator))
		{
			Modifiers.Add(ElementusItems[Iterator]);
		}
	}

	DiscardItems(Modifiers);
}

void UElementusInventoryComponent::DiscardItems_Implementation(const TArray<FElementusItemInfo>& Items)
{
	if (GetOwnerRole() != ROLE_Authority || UElementusInventoryFunctions::HasEmptyParam(Items))
	{
		return;
	}

	UpdateElementusItems(Items, EElementusInventoryUpdateOperation::Remove);
}

void UElementusInventoryComponent::AddItems_Implementation(const TArray<FElementusItemInfo>& Items)
{
	if (GetOwnerRole() != ROLE_Authority || UElementusInventoryFunctions::HasEmptyParam(Items))
	{
		return;
	}

	UpdateElementusItems(Items, EElementusInventoryUpdateOperation::Add);
}

void UElementusInventoryComponent::UpdateElementusItems(const TArray<FElementusItemInfo>& Modifiers,
                                                        const EElementusInventoryUpdateOperation Operation)
{
	TArray<FItemModifierData> ModifierDataArr;

	const FString OpStr = Operation == EElementusInventoryUpdateOperation::Add ? "Add" : "Remove";
	const FString OpPred = Operation == EElementusInventoryUpdateOperation::Add ? "to" : "from";

	uint32 SearchOffset = 0;
	FElementusItemInfo LastCheckedItem;
	for (const FElementusItemInfo& Iterator : Modifiers)
	{
		UE_LOG(LogElementusInventory_Internal, Display, TEXT("%s: %s %d item(s) with name '%s' %s inventory"), *FString(__FUNCTION__), *OpStr,
		       Iterator.Quantity, *Iterator.ItemId.ToString(), *OpPred);

		if (Iterator != LastCheckedItem)
		{
			SearchOffset = 0u;
		}

		int32 Index;
		if (FindFirstItemIndexWithInfo(Iterator, Index, FGameplayTagContainer::EmptyContainer, SearchOffset) && Operation ==
			EElementusInventoryUpdateOperation::Remove)
		{
			SearchOffset = Index + 1u;
		}

		ModifierDataArr.Add(FItemModifierData(Iterator, Index));
		LastCheckedItem = Iterator;
	}

	switch (Operation)
	{
	case EElementusInventoryUpdateOperation::Add:
		Server_ProcessInventoryAddition_Internal(ModifierDataArr);
		break;

	case EElementusInventoryUpdateOperation::Remove:
		Server_ProcessInventoryRemoval_Internal(ModifierDataArr);
		break;

	default:
		break;
	}
}

void UElementusInventoryComponent::Server_ProcessInventoryAddition_Internal_Implementation(const TArray<FItemModifierData>& Modifiers)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	for (const FItemModifierData& Iterator : Modifiers)
	{
		if (const bool bIsStackable = UElementusInventoryFunctions::IsItemStackable(Iterator.ItemInfo); bIsStackable && Iterator.Index != INDEX_NONE)
		{
			ElementusItems[Iterator.Index].Quantity += Iterator.ItemInfo.Quantity;
		}
		else if (!bIsStackable)
		{
			for (int32 i = 0u; i < Iterator.ItemInfo.Quantity; ++i)
			{
				const FElementusItemInfo ItemInfo{Iterator.ItemInfo.ItemId, 1, Iterator.ItemInfo.Tags};

				ElementusItems.Add(ItemInfo);
			}
		}
		else
		{
			ElementusItems.Add(Iterator.ItemInfo);
		}
	}

	NotifyInventoryChange();
}

void UElementusInventoryComponent::Server_ProcessInventoryRemoval_Internal_Implementation(const TArray<FItemModifierData>& Modifiers)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	for (const FItemModifierData& Iterator : Modifiers)
	{
		if (Iterator.Index == INDEX_NONE || Iterator.Index > ElementusItems.Num())
		{
			UE_LOG(LogElementusInventory_Internal, Warning, TEXT("%s: Item with name '%s' not found in inventory"), *FString(__FUNCTION__),
			       *Iterator.ItemInfo.ItemId.ToString());

			continue;
		}

		ElementusItems[Iterator.Index].Quantity -= Iterator.ItemInfo.Quantity;
	}

	if (bAllowEmptySlots)
	{
		Algo::ForEach(ElementusItems, [](FElementusItemInfo& InInfo)
		{
			if (InInfo.Quantity <= 0)
			{
				InInfo = FElementusItemInfo::EmptyItemInfo;
			}
		});
	}
	else
	{
		ElementusItems.RemoveAll([](const FElementusItemInfo& InInfo)
		{
			return InInfo.Quantity <= 0;
		});
	}

	NotifyInventoryChange();
}

void UElementusInventoryComponent::OnRep_ElementusItems()
{
	if (const int32 LastValidIndex = ElementusItems.FindLastByPredicate([](const FElementusItemInfo& Item)
	{
		return UElementusInventoryFunctions::IsItemValid(Item);
	}); LastValidIndex != INDEX_NONE && ElementusItems.IsValidIndex(LastValidIndex + 1))
	{
		ElementusItems.RemoveAt(LastValidIndex + 1, ElementusItems.Num() - LastValidIndex - 1, false);
	}
	else if (LastValidIndex == INDEX_NONE && !UElementusInventoryFunctions::HasEmptyParam(ElementusItems))
	{
		ElementusItems.Empty();
	}

	ElementusItems.Shrink();

	if (IsInventoryEmpty())
	{
		ElementusItems.Empty();

		CurrentWeight = 0.f;
		OnInventoryEmpty.Broadcast();
	}
	else
	{
		UpdateWeight();
	}

	OnInventoryUpdate.Broadcast();
}

void UElementusInventoryComponent::NotifyInventoryChange()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		OnRep_ElementusItems();
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(UElementusInventoryComponent, ElementusItems, this);
}

void UElementusInventoryComponent::UpdateWeight_Implementation()
{
	float NewWeight = 0.f;
	for (const FElementusItemInfo& Iterator : ElementusItems)
	{
		if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(Iterator.ItemId, {"Data"}))
		{
			NewWeight += ItemData->ItemWeight * Iterator.Quantity;
		}
	}

	CurrentWeight = FMath::Clamp(NewWeight, 0.f, MAX_FLT);
}
