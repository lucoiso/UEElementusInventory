// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusDetailsPanel.h"
#include "ElementusInventoryData.h"
#include "Engine/AssetManager.h"

void ElementusDetailsPanel::CustomizeHeader(const TSharedRef<IPropertyHandle> PropertyHandle,
                                            FDetailWidgetRow& HeaderRow,
                                            IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyHandlePtr = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SObjectPropertyEntryBox)
				.AllowedClass(UElementusItemData::StaticClass())
				.PropertyHandle(PropertyHandle)
				.DisplayThumbnail(true)
				.ThumbnailPool(CustomizationUtils.GetThumbnailPool())
				.ObjectPath(this, &ElementusDetailsPanel::GetObjPath)
				.OnObjectChanged(this, &ElementusDetailsPanel::OnObjChanged)
				.OnShouldFilterAsset_Lambda([](const FAssetData& AssetData) -> bool
			                             {
				                             return AssetData.GetPrimaryAssetId().PrimaryAssetType !=
					                             FPrimaryAssetType(ElementusItemDataType);
			                             })
		];
}

void ElementusDetailsPanel::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                              IDetailChildrenBuilder& StructBuilder,
                                              IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void ElementusDetailsPanel::OnObjChanged(const FAssetData& AssetData) const
{
	// (PrimaryAssetType="VALUE",PrimaryAssetName="VALUE")
	const FString InValue(FString::Printf(
		TEXT("(PrimaryAssetType=\"%s\",PrimaryAssetName=\"%s\")"),
		*AssetData.GetPrimaryAssetId().PrimaryAssetType.ToString(),
		*AssetData.GetPrimaryAssetId().PrimaryAssetName.ToString()
	));

	ensure(PropertyHandlePtr->SetValueFromFormattedString(InValue) == FPropertyAccess::Result::Success);
}

FString ElementusDetailsPanel::GetObjPath() const
{
	if (const UAssetManager* AssetManager = UAssetManager::GetIfValid();
		AssetManager && PropertyHandlePtr.IsValid())
	{
		FString AssetTypeValueStr;
		PropertyHandlePtr->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPrimaryAssetId, PrimaryAssetType))->
		                   GetValueAsDisplayString(AssetTypeValueStr);
		FString AssetIdValueStr;
		PropertyHandlePtr->GetChildHandle(GET_MEMBER_NAME_CHECKED(FPrimaryAssetId, PrimaryAssetName))->
		                   GetValueAsDisplayString(AssetIdValueStr);

		const FPrimaryAssetId AssetId(*AssetTypeValueStr, *AssetIdValueStr);
		return AssetId.IsValid() ? AssetManager->GetPrimaryAssetPath(AssetId).ToString() : "";
	}

	return "";
}
