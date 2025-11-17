#pragma once

#include "CoreMinimal.h"
#include "DarkGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DARK_SCRIPT_API ALobbyGameMode : public ADarkGameMode
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void WaitingBeforeChecking(const float Duration = 2);
	void CheckingPlayers();
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	int32 NumConnections;
	bool bAllowInvites;
	bool bAllowJoinInProgress;
	TArray<FSessionPropertyKeyPair> ExtraSettings;
	UPROPERTY()
	TArray<APlayerController*> PlayerControllers;
	
private:
	FTimerHandle CheckingPlayerHandle;
};
