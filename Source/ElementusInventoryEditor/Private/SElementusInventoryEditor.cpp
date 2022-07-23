// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusInventoryEditor.h"

static const FName& ColumnId_PrimaryIdLabel = "PrimaryAssetId";
static const FName& ColumnId_ItemIdLabel = "Id";
static const FName& ColumnId_NameLabel = "Name";
static const FName& ColumnId_TypeLabel = "Type";
static const FName& ColumnId_ClassLabel = "Class";
static const FName& ColumnId_ValueLabel = "Value";
static const FName& ColumnId_WeightLabel = "Weight";

class SElementusItemTableRow final : public SMultiColumnTableRow<FElementusItemPtr>
{
public:
	SLATE_BEGIN_ARGS(SElementusItemTableRow) { }
	SLATE_END_ARGS()

	void Construct([[maybe_unused]] const FArguments& InArgs,
					const TSharedRef<STableViewBase>& InOwnerTableView,
					const FElementusItemPtr InEntryItem)
	{
		Item = InEntryItem;
		SMultiColumnTableRow<FElementusItemPtr>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		const FSlateFontInfo& CellFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);
		const FMargin& CellMargin = FMargin(4.f);
		
		if (ColumnName == ColumnId_PrimaryIdLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(Item->PrimaryAssetId.ToString()))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_ItemIdLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(Item->Id.ToString()))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_NameLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(Item->Name.ToString()))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_TypeLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(Item->Type.ToString()))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_ClassLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(Item->Class.ToString()))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_ValueLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(FString::SanitizeFloat(Item->Value)))
			.Font(CellFont)
			.Margin(CellMargin);
		}
		if (ColumnName == ColumnId_WeightLabel)
		{
			return SNew(STextBlock)
			.Text(FText::FromString(FString::SanitizeFloat(Item->Weight)))
			.Font(CellFont)
			.Margin(CellMargin);
		}

		return SNullWidget::NullWidget;
	}

private:
	FElementusItemPtr Item;
};

void USElementusInventoryEditor::Construct([[maybe_unused]] const FArguments& InArgs)
{
	const TSharedPtr<SHeaderRow> HeaderRow = SNew(SHeaderRow);

	const auto& HeaderColumnCreator_Lambda =
		[&](const FName& ColumnId, const FString& ColumnText) -> const SHeaderRow::FColumn::FArguments
		{
			return SHeaderRow::Column(ColumnId)
							.DefaultLabel(FText::FromString(ColumnText))
							.FillWidth(0.5f)
							.SortMode(this, &USElementusInventoryEditor::GetColumnSort, ColumnId)
							.OnSort(this, &USElementusInventoryEditor::OnColumnSort)
							.HeaderComboVisibility(EHeaderComboVisibility::OnHover);
		};

	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_PrimaryIdLabel, "Primary Asset Id"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_ItemIdLabel, "Item Id"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_NameLabel, "Item Name"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_TypeLabel, "Item Type"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_ClassLabel, "Item Class"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_ValueLabel, "Item Value"));
	HeaderRow->AddColumn(HeaderColumnCreator_Lambda(ColumnId_WeightLabel, "Item Weight"));

	EdListView = SNew(SListView<FElementusItemPtr>)
					.ListItemsSource(&ItemArr)
					.SelectionMode(ESelectionMode::Single)
					.IsFocusable(true)
					.OnGenerateRow(this, &USElementusInventoryEditor::OnGenerateWidgetForList)
					.HeaderRow(HeaderRow);
	
	ChildSlot
	[
		SNew(SBorder)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				EdListView.ToSharedRef()
			]
	];

	for (const auto& Iterator : UElementusInventoryFunctions::GetElementusItemIds())
	{
		ItemArr.Add(MakeShareable<FElementusItemData>(new FElementusItemData(Iterator)));
	}
}

TSharedRef<ITableRow> USElementusInventoryEditor::OnGenerateWidgetForList(const FElementusItemPtr InItem,
                                                                          const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(SElementusItemTableRow, OwnerTable, InItem);
}

void USElementusInventoryEditor::OnColumnSort([[maybe_unused]] const EColumnSortPriority::Type SortPriority,
												const FName& ColumnName,
												const EColumnSortMode::Type SortMode)
{
	ColumnBeingSorted = ColumnName;
	CurrentSortMode = SortMode;

	const auto& CompareLambda =
		[&] (const auto& Val1, const auto& Val2) -> bool
		{
			switch (SortMode)
			{
				case EColumnSortMode::Ascending:
					return Val1 < Val2;
				case EColumnSortMode::Descending:
					return Val1 > Val2;
				case EColumnSortMode::None:
					return Val1 < Val2;
				default:
					return false;
			}
		};

	const auto& SortLambda =
		[&] (const TSharedPtr<FElementusItemData>& Val1, const TSharedPtr<FElementusItemData>& Val2) -> bool
		{
			if (ColumnName == ColumnId_PrimaryIdLabel)
			{
				return CompareLambda(Val1->PrimaryAssetId.ToString(), Val2->PrimaryAssetId.ToString());
			}

			if (ColumnName == ColumnId_ItemIdLabel)
			{
				return CompareLambda(Val1->Id.ToString(), Val2->Id.ToString());
			}

			if (ColumnName == ColumnId_NameLabel)
			{
				return CompareLambda(Val1->Name.ToString(), Val2->Name.ToString());
			}

			if (ColumnName == ColumnId_TypeLabel)
			{
				return CompareLambda(Val1->Type.ToString(), Val2->Type.ToString());
			}

			if (ColumnName == ColumnId_ClassLabel)
			{
				return CompareLambda(Val1->Class.ToString(), Val2->Class.ToString());
			}

			if (ColumnName == ColumnId_ValueLabel)
			{
				return CompareLambda(Val1->Value, Val2->Value);
			}

			if (ColumnName == ColumnId_WeightLabel)
			{
				return CompareLambda(Val1->Weight, Val2->Weight);
			}

			return false;
		};

	Algo::Sort(ItemArr, SortLambda);
	EdListView->RequestListRefresh();
}

EColumnSortMode::Type USElementusInventoryEditor::GetColumnSort(const FName ColumnId) const
{
	if (ColumnBeingSorted != ColumnId)
	{
		return EColumnSortMode::None;
	}

	return CurrentSortMode;
}
