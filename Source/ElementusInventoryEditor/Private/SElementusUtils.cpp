// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusUtils.h"
#include "Widgets/Layout/SUniformGridPanel.h"

void SElementusUtils::Construct([[maybe_unused]] const FArguments& InArgs)
{
	constexpr float Slot_Padding = 2.f;

	const auto& ButtonCreator_Lambda =
		[this](const FString& InStr) -> TSharedRef<SButton>
	{
		return SNew(SButton)
			.Text(FText::FromString(InStr))
			.HAlign(HAlign_Center)
			.OnClicked(this, &SElementusUtils::OnButtonClicked);
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
				ButtonCreator_Lambda("Button 1")
			]
			+ SUniformGridPanel::Slot(1, 0)
			[
				ButtonCreator_Lambda("Button 2")
			]
			+ SUniformGridPanel::Slot(0, 1)
			[
				ButtonCreator_Lambda("Button 3")
			]
			+ SUniformGridPanel::Slot(1, 1)
			[
				ButtonCreator_Lambda("Button 4")
			]
		]
	];
}

FReply SElementusUtils::OnButtonClicked() const
{
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Work in Progress"));
	return FReply::Handled();
}
