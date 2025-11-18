#include "Utils/Libraries/ProxyLibrary.h"

#include "Core/DarkTypes.h"
#include "Core/Subsystems/ProxySubsystem.h"

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralSessionPropertyInt(const FName Key, const int32 Integer)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(Integer);
	return KeyPair;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralSessionPropertyFloat(const FName Key, const float Float)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(Float);
	return KeyPair;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralSessionPropertyBool(const FName Key, const bool Bool)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(Bool);
	return KeyPair;
}

FSessionPropertyKeyPair UProxyLibrary::MakeLiteralSessionPropertyString(const FName Key, const FString& String)
{
	FSessionPropertyKeyPair KeyPair;
	KeyPair.Key = Key;
	KeyPair.Data.SetValue(String);
	return KeyPair;
}

void UProxyLibrary::GetSessionPropertyInt(const FSessionPropertyKeyPair& Property, FName& Key, int32& Value,
	EBlueprintResultSwitch& Result)
{
	if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Int32)
	{
		Key = Property.Key;
		Property.Data.GetValue(Value);
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}
	Result = EBlueprintResultSwitch::OnFailure;
}

void UProxyLibrary::GetSessionPropertyFloat(const FSessionPropertyKeyPair& Property, FName& Key, float& Value,
	EBlueprintResultSwitch& Result)
{
	if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Float)
	{
		Key = Property.Key;
		Property.Data.GetValue(Value);
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}
	Result = EBlueprintResultSwitch::OnFailure;
}

void UProxyLibrary::GetSessionPropertyBool(const FSessionPropertyKeyPair& Property, FName& Key, bool& Value,
	EBlueprintResultSwitch& Result)
{
	if (Property.Data.GetType() == EOnlineKeyValuePairDataType::Bool)
	{
		Key = Property.Key;
		Property.Data.GetValue(Value);
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}
	Result = EBlueprintResultSwitch::OnFailure;
}

void UProxyLibrary::GetSessionPropertyString(const FSessionPropertyKeyPair& Property, FName& Key, FString& Value,
	EBlueprintResultSwitch& Result)
{
	if (Property.Data.GetType() == EOnlineKeyValuePairDataType::String)
	{
		Key = Property.Key;
		Property.Data.GetValue(Value);
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}
	Result = EBlueprintResultSwitch::OnFailure;
}

void UProxyLibrary::GetSessionPropertyByKey(const TArray<FSessionPropertyKeyPair>& ExtraSettings, const FName Key,
	FSessionPropertyKeyPair& Property, EBlueprintResultSwitch& Result)
{
	for (const auto& InProperty : ExtraSettings)
	{
		if (InProperty.Key == Key)
		{
			Property = InProperty;
			Result = EBlueprintResultSwitch::OnSuccess;
			return;
		}
	}
	Result = EBlueprintResultSwitch::OnFailure;
}
