// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "SElementusItemCreator.h"
#include "PropertyCustomizationHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "AssetThumbnail.h"

void SElementusItemCreator::Construct([[maybe_unused]] const FArguments& InArgs)
{
	constexpr float Slot_Padding = 4.f;

	ImageIcon_ThumbnailPool = MakeShareable(new FAssetThumbnailPool(1024));

	const auto& CenterTextCreator_Lambda =
		[](const FString& InStr) -> const TSharedRef<STextBlock>
	{
		return SNew(STextBlock)
							.Text(FText::FromString(InStr))
							.TextStyle(FEditorStyle::Get(), "PropertyEditor.AssetClass")
							.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
							.Justification(ETextJustify::Left);
	};

	const auto& ObjEntryBoxCreator_Lambda =
		[this](UClass* ObjClass, const int32 ObjId) -> const TSharedRef<SObjectPropertyEntryBox>
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

	const auto& ContentPairCreator_Lambda =
		[this](const TSharedRef<SWidget> Content1,
		       const TSharedRef<SWidget> Content2) -> const TSharedRef<SHorizontalBox>
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .FillWidth(0.5f)
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Center)
			[
				Content1
			]
			+ SHorizontalBox::Slot()
			  .FillWidth(0.5f)
			  .MaxWidth(300.f)
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Center)
			[
				Content2
			];
	};

	// Work in Progress
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Image"),
				                          ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 0))
			]
			+ SVerticalBox::Slot()
			  .Padding(Slot_Padding)
			  .AutoHeight()
			[
				ContentPairCreator_Lambda(CenterTextCreator_Lambda("Item Icon"),
				                          ObjEntryBoxCreator_Lambda(UTexture2D::StaticClass(), 1))
			]
		]
	];
}

void SElementusItemCreator::OnObjChanged(const FAssetData& AssetData, const int32 ObjId)
{
	ObjectMap.FindOrAdd(ObjId) = AssetData.GetAsset();
}

FString SElementusItemCreator::GetObjPath(const int32 ObjId) const
{
	return ObjectMap.Contains(ObjId) && ObjectMap.FindRef(ObjId).IsValid()
		       ? ObjectMap.FindRef(ObjId)->GetPathName()
		       : FString("");
}
