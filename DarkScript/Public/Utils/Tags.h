#pragma once
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace DamageType
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flesh);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Penetration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ice);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Holy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Poison);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bleeding);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ranged);

	inline const TArray<FGameplayTag>& GetList()
	{
		static TArray<FGameplayTag> List = {
			Flesh,
			Penetration,
			Ice,
			Fire,
			Lightning,
			Holy,
			Poison,
			Bleeding,
			Ranged
		};
		return List;
	}
}