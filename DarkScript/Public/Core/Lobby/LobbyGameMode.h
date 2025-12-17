#pragma once
#include "CoreMinimal.h"
#include "Core/Framework/Base_GameMode.h"
#include "Utils/Types/OnlineTypes.h"
#include "LobbyGameMode.generated.h"

class UOnlineSystem;
enum class EClassArchetype : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API ALobbyGameMode : public ABase_GameMode
{
	GENERATED_BODY()
	
public:
	// ═══════════════════════════════════════════════════════════════════════
	// CONSTRUCTEUR
	// ═══════════════════════════════════════════════════════════════════════
	
    ALobbyGameMode();
	virtual void BeginPlay();
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Blueprint API)
	// ═══════════════════════════════════════════════════════════════════════
    
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (Helpers)
	// ═══════════════════════════════════════════════════════════════════════

	static bool GetConnectInfos(const APlayerController* NewPlayer, EClassArchetype& NewArchetype, int32& NewLevel);
	bool IsHostPlayer(const APlayerController* NewPlayer) const;
	void CheckingPlayer(TWeakObjectPtr<APlayerController> NewPlayer, const EClassArchetype InArchetype, const int32 Level, bool bIsHost = false) const;
	int32 GetPlayerCount() const;
	
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	UPROPERTY()
	TObjectPtr<UOnlineSystem> OnlineSystem;
	FDarkSessionSettings* Settings;
	FCriticalSection SessionMutex;
};
