#include "Gameplay/AbilitySystem/Attributes/DefaultAttributeSet.h"

#include "Gameplay/AbilitySystem/DefaultAbilitySystem.h"

class UWorld;

UDefaultAttributeSet::UDefaultAttributeSet()
{
}

UWorld* UDefaultAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);
	
	return Outer->GetWorld();
}

UDefaultAbilitySystem* UDefaultAttributeSet::GetDefaultAbilitySystem() const
{
	return Cast<UDefaultAbilitySystem>(GetOwningAbilitySystemComponent());
}

void UDefaultAttributeSet::Init()
{
}

void UDefaultAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
}
