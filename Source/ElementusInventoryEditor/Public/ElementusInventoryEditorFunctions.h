// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

namespace ElementusEdHelper
{
	static UEnum* GetUEnum(const TCHAR* InEnumName)
	{
		return FindObject<UEnum>(ANY_PACKAGE, InEnumName, true);
	}

	static FString EnumToString(const TCHAR* InEnumName, const int32 InEnumValue)
	{
		const auto& EnumPtr = GetUEnum(InEnumName);
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

	static TArray<TSharedPtr<FString>> GetEnumValuesAsStringArray(const TCHAR* InEnumName)
	{
		const auto& EnumPtr = GetUEnum(InEnumName);
		if (EnumPtr == nullptr)
		{
			return TArray<TSharedPtr<FString>>();
		}

		TArray<TSharedPtr<FString>> EnumValues;
		for (int32 i = 0; i < EnumPtr->NumEnums(); i++)
		{
			EnumValues.Add(MakeShareable(new FString(EnumPtr->GetDisplayNameTextByIndex(i).ToString())));
		}
		return EnumValues;
	}
}
