// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusSearch.h"
#include <Management/ElementusInventoryData.h>
#include <Management/ElementusInventoryFunctions.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Layout/SUniformGridPanel.h>

void SElementusSearch::Construct(const FArguments& InArgs)
{
    OnCheckStateChanged = InArgs._OnCheckboxStateChanged;
    OnTextChangedDelegate = InArgs._OnSearchTextChanged;

    ChildSlot
        [
            ConstructContent()
        ];
}

TSharedRef<SWidget> SElementusSearch::ConstructContent()
{
#if ENGINE_MAJOR_VERSION < 5
    using FAppStyle = FEditorStyle;
#endif

    constexpr float SlotPadding = 4.f;

    const auto CheckBoxCreator_Lambda = [this](const EElementusItemType& InType) -> const TSharedRef<SCheckBox>
        {
            constexpr float CheckBoxPadding = 2.f;
            const int32 Index = static_cast<int32>(InType);

            return SNew(SCheckBox)
                .Padding(CheckBoxPadding)
                .OnCheckStateChanged(this, &SElementusSearch::TriggerOnCheckboxStateChanged, Index)
                .Content()
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(UElementusInventoryFunctions::ElementusItemEnumTypeToString(static_cast<EElementusItemType>(Index))))
                        .Margin(CheckBoxPadding)
                ];
        };

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SSearchBox)
                .OnTextChanged(this, &SElementusSearch::TriggerOnSearchTextChanged)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SlotPadding)
        [
            SNew(STextBlock)
                .Text(FText::FromString(TEXT("Show types:")))
                .Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(SlotPadding)
        [
            SNew(SUniformGridPanel)
                .SlotPadding(1.f)
                + SUniformGridPanel::Slot(0, 0)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Accessory)
                ]
                + SUniformGridPanel::Slot(1, 0)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Armor)
                ]
                + SUniformGridPanel::Slot(0, 1)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Weapon)
                ]
                + SUniformGridPanel::Slot(1, 1)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Consumable)
                ]
                + SUniformGridPanel::Slot(0, 2)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Material)
                ]
                + SUniformGridPanel::Slot(1, 2)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Crafting)
                ]
                + SUniformGridPanel::Slot(0, 3)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Information)
                ]
                + SUniformGridPanel::Slot(1, 3)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Event)
                ]
                + SUniformGridPanel::Slot(0, 4)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Quest)
                ]
                + SUniformGridPanel::Slot(1, 4)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Junk)
                ]
                + SUniformGridPanel::Slot(0, 5)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Special)
                ]
                + SUniformGridPanel::Slot(1, 5)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::Other)
                ]
                + SUniformGridPanel::Slot(0, 6)
                [
                    CheckBoxCreator_Lambda(EElementusItemType::None)
                ]
        ];
}

void SElementusSearch::TriggerOnCheckboxStateChanged(const ECheckBoxState NewState, const int32 InType) const
{
    OnCheckStateChanged.ExecuteIfBound(NewState, InType);
}

void SElementusSearch::TriggerOnSearchTextChanged(const FText& InText) const
{
    OnTextChangedDelegate.ExecuteIfBound(InText);
}