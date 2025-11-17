#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tools/Types/GlobalTypes.h"
#include "Tools/Types/ProxyTypes.h"
#include "ProxyLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UProxyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	/**
	 * Retrieves the session code of the currently active online session.
	 *
	 * This value is stored inside the ProxyInstance subsystem and represents a custom
	 * identifier assigned during session creation. If no session is active, an empty string is returned.
	 *
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @return                    The current session's code, or an empty string if unavailable.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Network", meta=(WorldContext="WorldContextObject", DisplayName="Get Session Code"))
	static FString GetSessionCode(const UObject* WorldContextObject);
	/**
	 * Returns whether the currently active session was created as a public session.
	 *
	 * The public/private state is defined at session creation and retrieved through the ProxyInstance
	 * subsystem. If no session exists, this function returns false.
	 *
	 * @param WorldContextObject  Context object used to resolve the GameInstance.
	 * @return                    True if the current session is public.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Network", meta=(WorldContext="WorldContextObject", DisplayName="Is Public"))
	static bool IsPublic(const UObject* WorldContextObject);
	/**
	 * Extracts the custom session code from a Blueprint session search result.
	 *
	 * This function reads the "SESSION_CODE" key stored inside the session's settings.
	 * If the key does not exist, an empty string is returned.
	 *
	 * @param Session   The session result from which to extract the code.
	 * @return          The extracted session code, or an empty string if unavailable.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Get Session Code"))
	static FString GetSessionCode_FromSession(const FBlueprintSessionResult& Session);
	/**
	 * Retrieves all custom session properties from a found session.
	 *
	 * This function iterates through the session's key/value settings and converts them
	 * into an array of FSessionPropertyKeyPair, which can then be used for filtering or display.
	 *
	 * @param WorldContextObject  Not used directly but required for Blueprint compatibility.
	 * @param Session             The Blueprint session result to extract settings from.
	 * @return                    Array of all key/value session settings.
	 */
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(WorldContext="WorldContextObject", DisplayName="Get Extra Settings"))
	static TArray<FSessionPropertyKeyPair> GetExtraSettings(const UObject* WorldContextObject, const FBlueprintSessionResult& Session);
	/**
	 * Retrieves the full configuration of the currently active online session.
	 *
	 * This includes connection limits, LAN state, invitation permissions, build ID,
	 * and all custom extra settings.  
	 * If retrieval succeeds, the Result will be OnSuccess; otherwise OnFailure.
	 *
	 * @param WorldContextObject     Context object used to resolve the GameInstance.
	 * @param NumConnections         Output: maximum number of public connections.
	 * @param NumPrivateConnections  Output: maximum number of private connections.
	 * @param bIsLAN                 Output: whether the session uses LAN mode.
	 * @param bAllowInvites          Output: whether players may send invites.
	 * @param bAllowJoinInProgress   Output: whether players can join while the session is active.
	 * @param bIsAnticheatEnabled    Output: whether anticheat features were enabled.
	 * @param BuildUniqueID          Output: build identifier used by the OnlineSubsystem.
	 * @param ExtraSettings          Output: collection of additional key/value session settings.
	 * @param Result                 Execution result indicating success or failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Online|Session", meta=(ExpandEnumAsExecs = "Result", WorldContext="WorldContextObject", DisplayName="Get Session Settings", 
		AdvancedDisplay="bIsLAN,bAllowInvites,bAllowJoinInProgress,bIsAnticheatEnabled,BuildUniqueID,ExtraSettings"))
	static void GetSessionSettings(UObject* WorldContextObject, int32 &NumConnections, int32 &NumPrivateConnections, bool &bIsLAN, bool &bAllowInvites, bool &bAllowJoinInProgress, bool &bIsAnticheatEnabled, int32 &BuildUniqueID, TArray<FSessionPropertyKeyPair> &ExtraSettings, EBlueprintResultSwitch &Result);
	/**
	 * Creates a session property key/value pair using an integer value.
	 *
	 * This is used when constructing custom session settings for creation or filtering.
	 *
	 * @param Key       The name of the session property.
	 * @param Integer   The integer value to assign to the property.
	 * @return          A key/value pair containing the integer data.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property Int"))
	static FSessionPropertyKeyPair MakeLiteralIntegerSetting(const FName Key, const int32 Integer = 0);
	/**
	 * Creates a session property key/value pair using a boolean value.
	 *
	 * @param Key    The name of the session property.
	 * @param Bool   The boolean value to assign to the property.
	 * @return       A key/value pair containing the boolean data.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property Bool"))
	static FSessionPropertyKeyPair MakeLiteralBoolSetting(const FName Key, const bool Bool = false);
	/**
	 * Creates a session property key/value pair using a string value.
	 *
	 * @param Key      The name of the session property.
	 * @param String   The string value to assign to the property.
	 * @return         A key/value pair containing the string data.
	 */
	UFUNCTION(BlueprintPure, Category="Online|Session", meta=(DisplayName="Make Literal Session Property String"))
	static FSessionPropertyKeyPair MakeLiteralStringSetting(const FName Key, const FString String);
	/**
	 * Kicks a player from the current online session or server.
	 *
	 * This function forwards the kick request to the active GameSession and removes
	 * the specified PlayerController from the match.  
	 *
	 * Upon a successful kick, the DarkGameInstance broadcasts its OnKicked delegate,
	 * passing the same KickReason provided to this function.  
	 * This allows UI systems, gameplay logic, or network handlers to respond to the
	 * kick event and display or process the reason passed to the player.
	 *
	 * @param WorldContextObject   Context object used to resolve the GameMode and session.
	 * @param PlayerToKick         Player controller representing the player to remove.
	 * @param KickReason           Text describing the reason for the kick. This value is also
	 *                             emitted through UDarkGameInstance's OnKicked delegate.
	 *
	 * @return                     True if the kick request was successfully processed.
	 */
	UFUNCTION(BlueprintCallable, Category="Online|Session", meta=(WorldContext="WorldContextObject", DisplayName="Kick Player"))
	static bool KickPlayer(const UObject* WorldContextObject, APlayerController* PlayerToKick, const FString KickReason);
};
