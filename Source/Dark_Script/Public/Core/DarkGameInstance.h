#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DarkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DARK_SCRIPT_API UDarkGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void Init() override;
	virtual void Shutdown() override;
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsPlaying() const { return bIsPlaying; }
	
	UFUNCTION(BlueprintNativeEvent, Category = "Online|Network")
	void OnKicked(const FString& KickReason);
	virtual void OnKicked_Implementation(const FString& KickReason);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	bool bIsPlaying;
};
