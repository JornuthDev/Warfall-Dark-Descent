#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSystem.generated.h"

UENUM(BlueprintType)
enum class ESaveType : uint8
{
	PlayerSave,
	RunSave
};

class USaveGame;

UCLASS()
class DARKSCRIPT_API USaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Delegates ─────────────────────────────────────────────────────────────────
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveComplete, bool);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadComplete, bool);
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
		
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	void Create();
	void SaveGame(const ESaveType Type, UObject* ObjectToSave, const FString& Context);
	void LoadGame(const ESaveType Type, UObject* ObjectToLoad, const FString& Context);
	
	void OnSaveCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess);
	void OnLoadCompleteHandle(const FString& SlotName, const int32 UserIndex, const bool bSuccess);

	static TSubclassOf<USaveGame> GetSaveClass(const ESaveType Type);
	FString GetSlot(const ESaveType Type);
	

	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
	FOnSaveComplete OnSaveComplete;
	FOnLoadComplete OnLoadComplete;
	
private:
	TMap<ESaveType, FString> Slots;
	UPROPERTY()
	TMap<ESaveType, USaveGame*> SaveGames;
};
