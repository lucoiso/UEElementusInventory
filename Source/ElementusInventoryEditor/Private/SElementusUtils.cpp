// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusUtils.h"
#include "ElementusStaticIds.h"
#include "SElementusTable.h"
#include <Engine/AssetManager.h>
#include <Widgets/Layout/SUniformGridPanel.h>
#include <ObjectTools.h>

void SElementusUtils::Construct(const FArguments& InArgs)
{
    TableSource = InArgs._TableSource;

    ChildSlot
        [
            ConstructContent()
        ];
}

TSharedRef<SWidget> SElementusUtils::ConstructContent()
{
#if ENGINE_MAJOR_VERSION < 5
    using FAppStyle = FEditorStyle;
#endif

    constexpr float SlotPadding = 2.f;

    const auto ButtonCreator_Lambda = [this](const FString& InStr, const FString& Tooltip, const uint32& ButtonId) -> TSharedRef<SButton>
        {
            return SNew(SButton)
                .Text(FText::FromString(InStr))
                .ToolTip(SNew(SToolTip).Text(FText::FromString(Tooltip)))
                .HAlign(HAlign_Center)
                .OnClicked(this, &SElementusUtils::OnButtonClicked, ButtonId);
        };

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SlotPadding)
        [
            SNew(STextBlock)
                .Text(FText::FromString(TEXT("Utils:")))
                .Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SlotPadding)
        [
            SNew(SUniformGridPanel)
                .SlotPadding(SlotPadding / 2.f)
                + SUniformGridPanel::Slot(0, 0)
                [
                    ButtonCreator_Lambda("Create Item", "Open the item creator window", 0)
                ]
                + SUniformGridPanel::Slot(1, 0)
                [
                    ButtonCreator_Lambda("Delete Items", "Delete the selected items", 1)
                ]
                + SUniformGridPanel::Slot(0, 1)
                [
                    ButtonCreator_Lambda("Update Table", "Update the items table", 2)
                ]
        ];
}

FReply SElementusUtils::OnButtonClicked(const uint32 ButtonId) const
{
    if (ButtonId == 0)
    {
        FGlobalTabmanager::Get()->TryInvokeTab(ItemCreatorTabId);
    }
    else if (ButtonId == 1)
    {
        if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
        {
            TArray<FAssetData> AssetsToDelete;
            for (const FElementusItemPtr& Iterator : TableSource->GetSelectedItems())
            {
                FAssetData AssetData;
                AssetManager->GetPrimaryAssetData(Iterator->PrimaryAssetId, AssetData);
                AssetsToDelete.Add(AssetData);
            }

            if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
            {
                TableSource->UpdateItemList();
            }
        }
    }
    else if (ButtonId == 2)
    {
        TableSource->UpdateItemList();
    }
    else
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Work in Progress - ButtonId: ") + FString::FromInt(ButtonId)));
    }

    return FReply::Handled();
}