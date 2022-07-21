// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#include "ElementusInventoryEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FElementusInventoryEditorStyle::StyleInstance = nullptr;

void FElementusInventoryEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FElementusInventoryEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FElementusInventoryEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ElementusInventoryEditorStyle"));
	return StyleSetName;
}

const FVector2D GIcon16X16(16.0f, 16.0f);
const FVector2D GIcon20X20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FElementusInventoryEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("ElementusInventoryEditorStyle"));
	Style->SetContentRoot(
		IPluginManager::Get().FindPlugin("ElementusInventory")->GetBaseDir() / TEXT("Resources")
	);

	Style->Set("ElementusInventory.OpenPluginWindow",
	           new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"),
	                               GIcon20X20));

	return Style;
}

void FElementusInventoryEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FElementusInventoryEditorStyle::Get()
{
	return *StyleInstance;
}
