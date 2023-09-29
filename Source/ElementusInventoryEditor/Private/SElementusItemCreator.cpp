// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusItemCreator.h"
#include <Management/ElementusInventoryData.h>
#include <Management/ElementusInventoryFunctions.h>
#include <PropertyCustomizationHelpers.h>
#include <AssetThumbnail.h>
#include <AssetToolsModule.h>
#include <PackageTools.h>
#include <Engine/AssetManager.h>
#include <Factories/DataAssetFactory.h>
#include <Widgets/Input/SNumericEntryBox.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>
#include <Widgets/Input/STextComboBox.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Layout/SGridPanel.h>

#if ENGINE_MAJOR_VERSION >= 5
#include <UObject/SavePackage.h>
#endif

void SElementusItemCreator::Construct([[maybe_unused]] const FArguments&)
{
    ItemTypesArr = GetEnumValuesAsStringArray();
    UpdateFolders();

    ChildSlot
        [
            ConstructContent()
        ];
}

TSharedRef<SWidget> SElementusItemCreator::ConstructContent()
{
    constexpr float SlotPadding = 4.f;

    ImageIcon_ThumbnailPool = MakeShared<FAssetThumbnailPool>(1024u);

#if ENGINE_MAJOR_VERSION < 5
    using FAppStyle = FEditorStyle;
#endif

    const auto ObjEntryBoxCreator_Lambda = [this](UClass* const ObjClass, const int32 ObjId) -> const TSharedRef<SObjectPropertyEntryBox>
        {
            return SNew(SObjectPropertyEntryBox)
                .IsEnabled(true)
                .AllowedClass(ObjClass)
                .AllowClear(true)
                .DisplayUseSelected(true)
                .DisplayBrowse(true)
                .DisplayThumbnail(true)
                .ThumbnailPool(ImageIcon_ThumbnailPool.ToSharedRef())
                .ObjectPath(this, &SElementusItemCreator::GetObjPath, ObjId)
                .OnObjectChanged(this, &SElementusItemCreator::OnObjChanged, ObjId);
        };

    return SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SNew(SGridPanel)
                .FillColumn(0, 0.3f)
                .FillColumn(1, 0.7f)
                + SGridPanel::Slot(0, 0)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("ID")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 0)
                .Padding(SlotPadding)
                [
                    SNew(SNumericEntryBox<int32>)
                        .AllowSpin(false)
                        .MinValue(1)
                        .Value_Lambda([this] { return ItemId; })
                        .OnValueChanged_Lambda(
                            [this](const int32 InValue)
                            {
                                ItemId = InValue;
                            }
                        )
                ]
                + SGridPanel::Slot(0, 1)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Object")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 1)
                .Padding(SlotPadding)
                [
                    ObjEntryBoxCreator_Lambda(UObject::StaticClass(), 0)
                ]
                + SGridPanel::Slot(0, 2)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Class")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 2)
                .Padding(SlotPadding)
                [
                    SNew(SClassPropertyEntryBox)
                        .AllowAbstract(true)
                        .SelectedClass(this, &SElementusItemCreator::GetSelectedEntryClass)
                        .OnSetClass(this, &SElementusItemCreator::HandleNewEntryClassSelected)
                ]
                + SGridPanel::Slot(0, 3)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Name")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 3)
                .Padding(SlotPadding)
                [
                    SNew(SEditableTextBox)
                        .OnTextChanged_Lambda(
                            [this](const FText& InText)
                            {
                                ItemName = *InText.ToString();
                            }
                        )
                ]
                + SGridPanel::Slot(0, 4)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Description")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 4)
                .Padding(SlotPadding)
                [
                    SNew(SMultiLineEditableTextBox)
                        .OnTextChanged_Lambda(
                            [this](const FText& InText)
                            {
                                ItemDescription = InText;
                            }
                        )
                ]
                + SGridPanel::Slot(0, 5)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Type")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 5)
                .Padding(SlotPadding)
                [
                    SNew(STextComboBox)
                        .OptionsSource(&ItemTypesArr)
                        .OnSelectionChanged_Lambda(
                            [this](const FTextDisplayStringPtr& InStr, [[maybe_unused]] ESelectInfo::Type)
                            {
                                ItemType = ItemTypesArr.Find(InStr);
                            }
                        )
                ]
                + SGridPanel::Slot(0, 6)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Is Stackable")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 6)
                .Padding(SlotPadding)
                [
                    SNew(SCheckBox)
                        .IsChecked(ECheckBoxState::Checked)
                        .OnCheckStateChanged_Lambda(
                            [this](const ECheckBoxState InState)
                            {
                                bIsStackable = InState == ECheckBoxState::Checked;
                            }
                        )
                ]
                + SGridPanel::Slot(0, 7)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Value")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 7)
                .Padding(SlotPadding)
                [
                    SNew(SNumericEntryBox<float>)
                        .AllowSpin(false)
                        .MinValue(0.0f)
                        .Value_Lambda([this] { return ItemValue; })
                        .OnValueChanged_Lambda(
                            [this](const float InValue)
                            {
                                ItemValue = InValue;
                            }
                        )
                ]
                + SGridPanel::Slot(0, 8)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Weight")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 8)
                .Padding(SlotPadding)
                [
                    SNew(SNumericEntryBox<float>)
                        .AllowSpin(false)
                        .MinValue(0.0f)
                        .Value_Lambda([this] { return ItemWeight; })
                        .OnValueChanged_Lambda(
                            [this](const float InValue)
                            {
                                ItemWeight = InValue;
                            }
                        )
                ]
                + SGridPanel::Slot(0, 9)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Icon")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 9)
                .Padding(SlotPadding)
                [
                    ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 1)
                ]
                + SGridPanel::Slot(0, 10)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Image")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 10)
                .Padding(SlotPadding)
                [
                    ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 2)
                ]
                + SGridPanel::Slot(0, 11)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Asset Name")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 11)
                .Padding(SlotPadding)
                [
                    SNew(SEditableTextBox)
                        .OnTextChanged_Lambda(
                            [this](const FText& InText)
                            {
                                AssetName = *InText.ToString();
                            }
                        )
                ]
                + SGridPanel::Slot(0, 12)
                .Padding(SlotPadding)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Asset Folder")))
                        .TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
                        .Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
                ]
                + SGridPanel::Slot(1, 12)
                .Padding(SlotPadding)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        [
                            SNew(STextComboBox)
                                .OptionsSource(&AssetFoldersArr)
                                .OnSelectionChanged_Lambda(
                                    [this](const FTextDisplayStringPtr& InStr, [[maybe_unused]] ESelectInfo::Type)
                                    {
                                        AssetFolder = FName(*InStr.Get());
                                    }
                                )
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .OnClicked_Lambda(
                                    [this]() -> FReply
                                    {
                                        UpdateFolders();
                                        return FReply::Handled();
                                    }
                                )
                                .Content()
                                        [
                                            SNew(SImage)
                                                .Image(FAppStyle::Get().GetBrush("Icons.Refresh"))
                                        ]
                        ]
                ]
                + SGridPanel::Slot(1, 13)
                .Padding(SlotPadding * 2.f)
                .HAlign(HAlign_Left)
                [
                    SNew(SButton)
                        .Text(FText::FromString(TEXT("Create Item")))
                        .OnClicked(this, &SElementusItemCreator::HandleCreateItemButtonClicked)
                        .IsEnabled(this, &SElementusItemCreator::IsCreateEnabled)
                        .ToolTip(
                            SNew(SToolTip)
                            .Text(FText::FromString(TEXT("Already exists a item with this Id.")))
                            .Visibility_Lambda(
                                [this]() -> EVisibility
                                {
                                    return IsCreateEnabled() ? EVisibility::Collapsed : EVisibility::Visible;
                                }
                            )
                        )
                ]
        ];
}

void SElementusItemCreator::OnObjChanged(const FAssetData& AssetData, const int32 ObjId)
{
    ObjectMap.FindOrAdd(ObjId) = AssetData.GetAsset();
}

FString SElementusItemCreator::GetObjPath(const int32 ObjId) const
{
    return ObjectMap.Contains(ObjId) && ObjectMap.FindRef(ObjId).IsValid() ? ObjectMap.FindRef(ObjId)->GetPathName() : FString();
}

void SElementusItemCreator::HandleNewEntryClassSelected(const UClass* Class)
{
    ItemClass = Class;
}

const UClass* SElementusItemCreator::GetSelectedEntryClass() const
{
    return ItemClass.Get();
}

void SElementusItemCreator::UpdateFolders()
{
    AssetFoldersArr.Empty();

    if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
    {
        if (FPrimaryAssetTypeInfo Info; AssetManager->GetPrimaryAssetTypeInfo(FPrimaryAssetType(ElementusItemDataType), Info))
        {
            for (const FString& Path : Info.AssetScanPaths)
            {
                AssetFoldersArr.Add(MakeShared<FString>(Path));
            }
        }
    }

    if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid(); IsValid(AssetManager) && AssetManager->HasInitialScanCompleted() && UElementusInventoryFunctions::HasEmptyParam(AssetFoldersArr))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Asset Manager could not find any folder. Please check your Asset Manager settings.")));
    }
}

FReply SElementusItemCreator::HandleCreateItemButtonClicked() const
{
    if (AssetFolder.IsNone() || AssetName.IsNone())
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Please enter the asset name and folder for the new item.")));

        return FReply::Handled();
    }

    const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

    const FString PackageName = UPackageTools::SanitizePackageName(AssetFolder.ToString() + TEXT("/") + AssetName.ToString());

    UDataAssetFactory* const Factory = NewObject<UDataAssetFactory>();

    if (UObject* const NewData = AssetToolsModule.Get().CreateAsset(AssetName.ToString(), FPackageName::GetLongPackagePath(PackageName), UElementusItemData::StaticClass(), Factory))
    {
        UElementusItemData* const ItemData = Cast<UElementusItemData>(NewData);
        ItemData->ItemId = ItemId;
        ItemData->ItemObject = TSoftObjectPtr<UObject>(Cast<UObject>(ObjectMap.FindRef(0)));
        ItemData->ItemClass = TSoftClassPtr<UClass>(ItemClass.Get());
        ItemData->ItemName = ItemName;
        ItemData->ItemDescription = ItemDescription;
        ItemData->ItemType = static_cast<EElementusItemType>(ItemType);
        ItemData->bIsStackable = bIsStackable;
        ItemData->ItemValue = ItemValue;
        ItemData->ItemWeight = ItemWeight;
        ItemData->ItemIcon = Cast<UTexture2D>(ObjectMap.FindRef(1));
        ItemData->ItemImage = Cast<UTexture2D>(ObjectMap.FindRef(2));

        TArray<FAssetData> SyncAssets;
        SyncAssets.Add(FAssetData(ItemData));
        GEditor->SyncBrowserToObjects(SyncAssets);

        const FString TempPackageName = ItemData->GetPackage()->GetName();
        const FString TempPackageFilename = FPackageName::LongPackageNameToFilename(TempPackageName, FPackageName::GetAssetPackageExtension());

#if ENGINE_MAJOR_VERSION >= 5
        FSavePackageArgs SaveArgs;
        SaveArgs.SaveFlags = RF_Public | RF_Standalone;
        UPackage::SavePackage(ItemData->GetPackage(), ItemData, *TempPackageFilename, SaveArgs);
#else
        UPackage::SavePackage(ItemData->GetPackage(), ItemData, RF_Public | RF_Standalone, *TempPackageFilename);
#endif
    }

    return FReply::Handled();
}

bool SElementusItemCreator::IsCreateEnabled() const
{
    if (const UAssetManager* const AssetManager = UAssetManager::GetIfValid())
    {
        return ItemId != 0 && !AssetManager->GetPrimaryAssetPath(FPrimaryAssetId(ElementusItemDataType, *("Item_" + FString::FromInt(ItemId)))).IsValid();
    }

    return false;
}

TArray<FTextDisplayStringPtr> SElementusItemCreator::GetEnumValuesAsStringArray() const
{
    TArray<FTextDisplayStringPtr> EnumValues;
    for (uint32 iterator = 0; iterator < static_cast<uint32>(EElementusItemType::MAX); iterator++)
    {
        EnumValues.Add(MakeShared<FString>(UElementusInventoryFunctions::ElementusItemEnumTypeToString(static_cast<EElementusItemType>(iterator))));
    }

    return EnumValues;
}