#include "Core/DarkPlayerState.h"
#include "Core/DarkCharacter.h"

ADarkPlayerState::ADarkPlayerState()
{

}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Blueprint API) ─────────────────────────────────────────────────

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Replicates) ────────────────────────────────────────────────────
void ADarkPlayerState::SpawnCharacterClass_Implementation()
{
	if (IsValid(CharacterClass) && GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FVector(0, 0, 302.012643f));
		ADarkCharacter* Spawned = GetWorld()->SpawnActor<ADarkCharacter>(CharacterClass, SpawnTransform, SpawnParams);
		GetPlayerController()->Possess(Spawned);
		UE_LOG(LogTemp, Warning, TEXT("UDarkPlayerState::SpawnCharacterClass_Implementation Call"))
	}
}

// ───────────────────────────────────────────────────────────────────────────────
// ─── Functions (Internal) ──────────────────────────────────────────────────────
