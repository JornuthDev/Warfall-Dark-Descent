#include "Utils/Libraries/UtilsLibrary.h"

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Make Literal Session Property ───

FPropertyKeyPair UUtilsLibrary::MakeLiteralSessionPropertyByte(const FName Key, const uint8 Value)
{
	FPropertyKeyPair Property;
	Property.Key = Key;
	Property.Data.SetValue(Value);
	return Property;
}

FPropertyKeyPair UUtilsLibrary::MakeLiteralSessionPropertyBool(const FName Key, const bool Value)
{
	FPropertyKeyPair Property;
	Property.Key = Key;
	Property.Data.SetValue(Value);
	return Property;
}

FPropertyKeyPair UUtilsLibrary::MakeLiteralSessionPropertyString(const FName Key, const FString& Value)
{
	FPropertyKeyPair Property;
	Property.Key = Key;
	Property.Data.SetValue(Value);
	return Property;
}

FPropertyKeyPair UUtilsLibrary::MakeLiteralSessionPropertyInt(const FName Key, const int32 Value)
{
	FPropertyKeyPair Property;
	Property.Key = Key;
	Property.Data.SetValue(Value);
	return Property;
}

FPropertyKeyPair UUtilsLibrary::MakeLiteralSessionPropertyFloat(const FName Key, const float Value)
{
	FPropertyKeyPair Property;
	Property.Key = Key;
	Property.Data.SetValue(Value);
	return Property;
}

// ─── Get Session Property ───

void UUtilsLibrary::GetSessionPropertyByte(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, uint8& Value)
{
	for (const auto& Property : Properties)
	{
		if (Property.Key == Key)
		{
			if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Int32)
			{
				int32 Val;
				Property.Data.GetValue(Val);
				Value = static_cast<uint8>(Val);
				Result = EBlueprintResultSwitch::Success;
				return;
			}
			Result = EBlueprintResultSwitch::Failure;
			return;
		}
	}
	Result = EBlueprintResultSwitch::Failure;
}

void UUtilsLibrary::GetSessionPropertyBool(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, bool& Value)
{
	for (const auto& Property : Properties)
	{
		if (Property.Key == Key)
		{
			if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Bool)
			{
				Property.Data.GetValue(Value);
				Result = EBlueprintResultSwitch::Success;
				return;
			}
			Result = EBlueprintResultSwitch::Failure;
			return;
		}
	}
	Result = EBlueprintResultSwitch::Failure;
}

void UUtilsLibrary::GetSessionPropertyString(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result,
	FString& Value)
{
	for (const auto& Property : Properties)
	{
		if (Property.Key == Key)
		{
			if (Property.Data.GetType() == EOnlineKeyValuePairDataType::String)
			{
				Property.Data.GetValue(Value);
				Result = EBlueprintResultSwitch::Success;
				return;
			}
			Result = EBlueprintResultSwitch::Failure;
			return;
		}
	}
	Result = EBlueprintResultSwitch::Failure;
}

void UUtilsLibrary::GetSessionPropertyInt(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, int32& Value)
{
	for (const auto& Property : Properties)
	{
		if (Property.Key == Key)
		{
			if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Int32)
			{
				Property.Data.GetValue(Value);
				Result = EBlueprintResultSwitch::Success;
				return;
			}
			Result = EBlueprintResultSwitch::Failure;
			return;
		}
	}
	Result = EBlueprintResultSwitch::Failure;
}

void UUtilsLibrary::GetSessionPropertyFloat(const TArray<FPropertyKeyPair>& Properties, const FName Key, EBlueprintResultSwitch &Result, float& Value)
{
	for (const auto& Property : Properties)
	{
		if (Property.Key == Key)
		{
			if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Float)
			{
				Property.Data.GetValue(Value);
				Result = EBlueprintResultSwitch::Success;
				return;
			}
			Result = EBlueprintResultSwitch::Failure;
			return;
		}
	}
	Result = EBlueprintResultSwitch::Failure;
}



