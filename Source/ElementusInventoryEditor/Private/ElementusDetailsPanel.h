// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"
#include "PropertyCustomizationHelpers.h"

class ElementusDetailsPanel : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new ElementusDetailsPanel());
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	                             FDetailWidgetRow& HeaderRow,
	                             IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	                               IDetailChildrenBuilder& StructBuilder,
	                               IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	FString GetObjPath() const;
	void OnObjChanged(const FAssetData& AssetData) const;

private:
	TSharedPtr<IPropertyHandle> PropertyHandlePtr;
};
