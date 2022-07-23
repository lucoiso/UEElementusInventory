// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryFunctions.h"

static FString EnumToString(const TCHAR* InEnumName, int32 InEnumValue)
{
	const auto& EnumPtr = FindObject<UEnum>(ANY_PACKAGE, InEnumName, true);
	if (EnumPtr == nullptr)
	{
		return "Invalid";
	}

#if WITH_EDITOR
	return EnumPtr->GetDisplayNameTextByValue(InEnumValue).ToString();
#else
	return EnumPtr->GetEnumName(InEnumValue);
#endif
}

struct FElementusItemData
{
	explicit FElementusItemData(const FPrimaryAssetId& InPrimaryAssetId)
	{
		const auto& ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(InPrimaryAssetId,
			                                                       {TEXT("Data"), TEXT("Actor")});

		PrimaryAssetId = InPrimaryAssetId;
		Id = *FString::FromInt(ItemData->ItemId);
		Name = ItemData->ItemName;
		Type = *EnumToString(TEXT("EElementusItemType"), static_cast<uint8>(ItemData->ItemType));
		Class = *ItemData->ItemClass->GetPathName();
		Value = ItemData->ItemValue;
		Weight = ItemData->ItemWeight;
	}

	FPrimaryAssetId PrimaryAssetId;
	FName Id = NAME_None;
	FName Name = NAME_None;
	FName Type = NAME_None;
	FName Class = NAME_None;
	float Value = -1.f;
	float Weight = -1.f;
};

typedef TSharedPtr<FElementusItemData, ESPMode::ThreadSafe> FElementusItemPtr;

class USElementusInventoryEditor final : public SCompoundWidget
{
	SLATE_USER_ARGS(USElementusInventoryEditor)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TArray<TSharedPtr<FElementusItemData>> ItemArr;

	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FElementusItemData> InItem,
	                                              const TSharedRef<STableViewBase>& OwnerTable) const;

	void OnColumnSort(EColumnSortPriority::Type SortPriority, const FName& ColumnName, EColumnSortMode::Type SortMode);
	EColumnSortMode::Type GetColumnSort(const FName ColumnId) const;

private:
	FName ColumnBeingSorted = NAME_None;
	EColumnSortMode::Type CurrentSortMode = EColumnSortMode::None;

	TSharedPtr<SListView<FElementusItemPtr>> EdListView;
};
