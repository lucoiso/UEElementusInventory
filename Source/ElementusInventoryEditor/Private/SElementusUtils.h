// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

#include "CoreMinimal.h"

class SElementusUtils final : public SCompoundWidget
{
	SLATE_USER_ARGS(SElementusUtils)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	FReply OnButtonClicked() const;
};
