#pragma once

#include "CoreMinimal.h"
#include "Core/DarkGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Utils/Types/OnlineTypes.h"
#include "LobbyGameMode.generated.h"

class UProgression;
struct FDarkSessionSettings;
class UOnlineSystem;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ALobbyGameMode : public ADarkGameMode
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	ALobbyGameMode();
	virtual void BeginPlay() override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingController) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	int32 GetPlayerCount() const
	{
		const AGameStateBase* GS = GetGameState<AGameStateBase>();
		return GS ? GS->PlayerArray.Num() : 0;
	}
	static bool HasLevelRestriction(const FDarkSessionSettings& Settings)
	{
		return Settings.MinLevelRequired > 1;
	}
	static bool HasArchetypeRestriction(const FDarkSessionSettings& Settings)
	{
		return Settings.AllowedArchetypes.Num() > 0;
	}
	
	bool IsHostPlayer(const APlayerController* PlayerController) const;
	void OnPlayerDataReceived(APlayerController* Player, UProgression* Progression, const EClassArchetype Archetype, const int32 Level);
	
private:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY()
	UOnlineSystem* OnlineSystem;
	FCriticalSection SessionMutex;
};
