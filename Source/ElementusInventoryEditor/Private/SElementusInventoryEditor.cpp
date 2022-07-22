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
		if (ColumnName == ColumnId_PrimaryIdLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(Item->PrimaryAssetId.ToString()));
		}
		if (ColumnName == ColumnId_ItemIdLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(Item->Id.ToString()));
		}
		if (ColumnName == ColumnId_NameLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(Item->Name.ToString()));
		}
		if (ColumnName == ColumnId_TypeLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(Item->Type.ToString()));
		}
		if (ColumnName == ColumnId_ClassLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(Item->Class.ToString()));
		}
		if (ColumnName == ColumnId_ValueLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(FString::SanitizeFloat(Item->Value)));
		}
		if (ColumnName == ColumnId_WeightLabel)
		{
			return SNew(STextBlock).Text(FText::FromString(FString::SanitizeFloat(Item->Weight)));
		}

		return SNullWidget::NullWidget;
	}

private:
	FElementusItemPtr Item;
};

void USElementusInventoryEditor::Construct([[maybe_unused]] const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SListView<FElementusItemPtr>)
				.ItemHeight(24)
				.ListItemsSource(&ItemArr)
				.OnGenerateRow(this, &USElementusInventoryEditor::OnGenerateWidgetForList)
				.HeaderRow
		        	(
			    	    SNew(SHeaderRow)
			    	    + SHeaderRow::Column(ColumnId_PrimaryIdLabel)
			    	    .DefaultLabel(FText::FromString("Primary Asset Id"))
			    	    .FillWidth(0.5f)
			    	    
			    	    + SHeaderRow::Column(ColumnId_ItemIdLabel)
			    	    .DefaultLabel(FText::FromString("Item Id"))
			    	    .FillWidth(0.2f)
						
			    	    + SHeaderRow::Column(ColumnId_NameLabel)
			    	    .DefaultLabel(FText::FromString("Item Name"))
			    	    .FillWidth(0.2f)
						
			    	    + SHeaderRow::Column(ColumnId_TypeLabel)
			    	    .DefaultLabel(FText::FromString("Item Type"))
			    	    .FillWidth(0.2f)
						
			    	    + SHeaderRow::Column(ColumnId_ClassLabel)
			    	    .DefaultLabel(FText::FromString("Item Class"))
			    	    .FillWidth(0.2f)
						
			    	    + SHeaderRow::Column(ColumnId_ValueLabel)
			    	    .DefaultLabel(FText::FromString("Item Value"))
			    	    .FillWidth(0.2f)
						
			    	    + SHeaderRow::Column(ColumnId_WeightLabel)
			    	    .DefaultLabel(FText::FromString("Item Weight"))
			    	    .FillWidth(0.2f)
		        	)
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
