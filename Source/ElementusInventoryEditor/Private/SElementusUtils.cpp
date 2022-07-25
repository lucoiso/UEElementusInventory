// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusUtils.h"
#include "ElementusStaticIds.h"
#include "SElementusTable.h"
#include "Widgets/Layout/SUniformGridPanel.h"

void SElementusUtils::Construct(const FArguments& InArgs)
{
	TableSource = InArgs._TableSource;

	constexpr float Slot_Padding = 2.f;

	const auto& ButtonCreator_Lambda =
		[this](const FString& InStr, const FString& Tooltip, const uint32& ButtonId) -> TSharedRef<SButton>
	{
		return SNew(SButton)
			.Text(FText::FromString(InStr))
			.ToolTip(SNew(SToolTip).Text(FText::FromString(Tooltip)))
			.HAlign(HAlign_Center)
			.OnClicked(this, &SElementusUtils::OnButtonClicked, ButtonId);
	};

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(Slot_Padding)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Utils:"))
			.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(Slot_Padding)
		[
			SNew(SUniformGridPanel)
			.SlotPadding(Slot_Padding / 2.f)
			+ SUniformGridPanel::Slot(0, 0)
			[
				ButtonCreator_Lambda("Create Item",
				                     "Open the item creator window",
				                     0)
			]
			+ SUniformGridPanel::Slot(1, 0)
			[
				ButtonCreator_Lambda("Delete Items",
				                     "Delete the selected items",
				                     1)
			]
			+ SUniformGridPanel::Slot(0, 1)
			[
				ButtonCreator_Lambda("Validate Items",
				                     "Validate items on folders added to the asset manager",
				                     2)
			]
			+ SUniformGridPanel::Slot(1, 1)
			[
				ButtonCreator_Lambda("Validate Folders",
				                     "Search for folders that contains Elementus Items and add to the Asset Manager settings"
				                     " and remove folders that don't contain Elementus Items",
				                     3)
			]
			+ SUniformGridPanel::Slot(0, 2)
			[
				ButtonCreator_Lambda("Randomize Ids",
				                     "Randomize all items ids",
				                     4)
			]
			+ SUniformGridPanel::Slot(1, 2)
			[
				ButtonCreator_Lambda("Export Table",
				                     "Export the items table to a csv file",
				                     5)
			]
			+ SUniformGridPanel::Slot(0, 3)
			[
				ButtonCreator_Lambda("Update Table",
				                     "Update the items table",
				                     6)
			]
		]
	];
}

FReply SElementusUtils::OnButtonClicked(const uint32 ButtonId) const
{
	if (ButtonId == 0)
	{
		FGlobalTabmanager::Get()->TryInvokeTab(ItemCreatorTabId);
	}
	else if (ButtonId == 6)
	{
		TableSource->UpdateItemList();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::FromString("Work in Progress - ButtonId: " + FString::FromInt(ButtonId)));
	}

	return FReply::Handled();
}
