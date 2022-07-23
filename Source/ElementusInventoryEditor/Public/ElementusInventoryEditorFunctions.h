// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

namespace ElementusEdHelper
{
	static FString EnumToString(const TCHAR* InEnumName, const int32 InEnumValue)
	{
		const auto& EnumPtr = FindObject<UEnum>(ANY_PACKAGE, InEnumName, true);
		if (EnumPtr == nullptr)
		{
			return "Invalid";
		}

#if WITH_EDITOR
		return EnumPtr->GetDisplayNameTextByValue(InEnumValue).ToString();
#else
		return EnumPtr->GetEnumName(InEnumValue);
#endif
	}
}
