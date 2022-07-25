// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

class SElementusItemCreator final : public SCompoundWidget
{
	SLATE_USER_ARGS(SElementusItemCreator)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FString GetObjPath(const int32 ObjId) const;
	void OnObjChanged(const FAssetData& AssetData, const int32 ObjId);

private:
	TMap<int32, TWeakObjectPtr<UObject>> ObjectMap;
	TSharedPtr<FAssetThumbnailPool> ImageIcon_ThumbnailPool;
};
