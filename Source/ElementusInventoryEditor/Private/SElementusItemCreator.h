// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include <CoreMinimal.h>

class SElementusItemCreator final : public SCompoundWidget
{
public:
    SLATE_USER_ARGS(SElementusItemCreator)
        {
        }

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedRef<SWidget> ConstructContent();

    FString GetObjPath(const int32 ObjId) const;
    void OnObjChanged(const struct FAssetData& AssetData, const int32 ObjId);

    const class UClass* GetSelectedEntryClass() const;
    void HandleNewEntryClassSelected(const class UClass* Class);

    void UpdateFolders();

    FReply HandleCreateItemButtonClicked() const;
    bool IsCreateEnabled() const;

    TArray<FTextDisplayStringPtr> GetEnumValuesAsStringArray() const;

    TMap<int32, TWeakObjectPtr<class UObject>> ObjectMap;
    TSharedPtr<class FAssetThumbnailPool> ImageIcon_ThumbnailPool;
    TArray<FTextDisplayStringPtr> ItemTypesArr;
    TArray<FTextDisplayStringPtr> AssetFoldersArr;

    FName AssetName;
    FName AssetFolder;
    int32 ItemId = 1;
    TWeakObjectPtr<const class UClass> ItemClass;
    FName ItemName;
    FText ItemDescription;
    uint8 ItemType = 0;
    bool bIsStackable = false;
    float ItemValue = 0.f;
    float ItemWeight = 0.f;
};
