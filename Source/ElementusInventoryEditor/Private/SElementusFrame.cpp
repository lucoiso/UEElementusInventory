// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusFrame.h"
#include "SElementusSearch.h"
#include "SElementusTable.h"
#include "SElementusUtils.h"
#include <Widgets/Layout/SScrollBox.h>

void SElementusFrame::Construct([[maybe_unused]] const FArguments& InArgs)
{
    ChildSlot
        [
            ConstructContent()
        ];
}

TSharedRef<SWidget> SElementusFrame::ConstructContent()
{
    constexpr float SlotPadding = 4.f;

    SAssignNew(Table, SElementusTable);

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .MaxWidth(300.f)
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .Padding(SlotPadding)
                        .AutoHeight()
                        [
                            SNew(SElementusSearch)
                                .OnSearchTextChanged(Table.ToSharedRef(), &SElementusTable::OnSearchTextModified)
                                .OnCheckboxStateChanged(Table.ToSharedRef(), &SElementusTable::OnSearchTypeModified)
                        ]
                        + SVerticalBox::Slot()
                        .Padding(SlotPadding)
                        .AutoHeight()
                        [
                            SNew(SElementusUtils)
                                .TableSource(Table)
                        ]
                ]
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.f)
        [
            Table.ToSharedRef()
        ];
}