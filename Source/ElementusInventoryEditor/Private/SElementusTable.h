// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "ElementusInventoryFunctions.h"

struct FElementusItemRowData
{
	explicit FElementusItemRowData(const FPrimaryAssetId& InPrimaryAssetId)
	{
		const auto& ItemData =
			UElementusInventoryFunctions::GetElementusItemDataById(InPrimaryAssetId,
			                                                       {TEXT("Data"), TEXT("Actor")});

		PrimaryAssetId = InPrimaryAssetId;
		Id = ItemData->ItemId;
		Name = ItemData->ItemName;
		Type = ItemData->ItemType;
		Class = *ItemData->ItemClass->GetPathName();
		Value = ItemData->ItemValue;
		Weight = ItemData->ItemWeight;
	}

	FPrimaryAssetId PrimaryAssetId;
	int32 Id = -1;
	FName Name = NAME_None;
	EElementusItemType Type = EElementusItemType::None;
	FName Class = NAME_None;
	float Value = -1.f;
	float Weight = -1.f;
};

using FElementusItemPtr = TSharedPtr<FElementusItemRowData, ESPMode::ThreadSafe>;

class SElementusTable final : public SCompoundWidget
{
	SLATE_USER_ARGS(SElementusTable)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TArray<TSharedPtr<FElementusItemRowData>> ItemArr;

	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FElementusItemRowData> InItem,
	                                              const TSharedRef<STableViewBase>& OwnerTable) const;

	void OnColumnSort(EColumnSortPriority::Type SortPriority, const FName& ColumnName, EColumnSortMode::Type SortMode);
	EColumnSortMode::Type GetColumnSort(const FName ColumnId) const;

	EVisibility GetIsVisible(const FElementusItemPtr InItem) const;

	TArray<int32> AllowedTypes;
	FText SearchText;

	void OnSearchTextModified(const FText& InText);
	void OnSearchTypeModified(const ECheckBoxState InState, const int32 InType);

private:
	FName ColumnBeingSorted = NAME_None;
	EColumnSortMode::Type CurrentSortMode = EColumnSortMode::None;

	TSharedPtr<SListView<FElementusItemPtr>> EdListView;
};
