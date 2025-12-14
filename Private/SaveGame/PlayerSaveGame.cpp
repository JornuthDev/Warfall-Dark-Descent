// Copyright Dark Script - All Rights Reserved

#include "SaveGame/PlayerSaveGame.h"
#include "UObject/UObjectGlobals.h"
#include "Utils/Helpers/ContentHelpers.h"

UPlayerSaveGame::UPlayerSaveGame()
{
	
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : GetOrCreateArchetypeData
// ═══════════════════════════════════════════════════════════════════════════

FInternalAttributeData& UPlayerSaveGame::GetOrCreateArchetypeData(EClassArchetype InArchetype)
{
	// Vérifier que l'archétype est valide
	if (InArchetype == EClassArchetype::None)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerSaveGame: Tentative d'accès à l'archétype 'None'"));
		
		// Retourner une référence statique vide pour éviter le crash
		static FInternalAttributeData EmptyData;
		return EmptyData;
	}

	// Si l'archétype existe déjà, le retourner
	if (ArchetypesData.Contains(InArchetype))
	{
		return ArchetypesData[InArchetype];
	}

	// Sinon, créer une nouvelle entrée avec des valeurs par défaut
	UE_LOG(LogTemp, Log, TEXT("PlayerSaveGame: Création des données pour l'archétype %d"), static_cast<int32>(InArchetype));

	const FInternalAttributeData NewData = *GetDefaultGameplay(InArchetype);
	
	ArchetypesData.Add(InArchetype, NewData);
	
	return ArchetypesData[InArchetype];
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : InitializeAllArchetypes
// ═══════════════════════════════════════════════════════════════════════════

void UPlayerSaveGame::InitializeAllArchetypes()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerSaveGame: Initialisation de tous les archétypes"));

	// Récupérer l'enum EClassArchetype
	const UEnum* EnumPtr = StaticEnum<EClassArchetype>();
	if (!EnumPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerSaveGame: Impossible de récupérer l'enum EClassArchetype"));
		return;
	}

	// Vider les données existantes
	ArchetypesData.Empty();

	// Parcourir toutes les valeurs de l'enum
	for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i) // -1 pour exclure MAX
	{
		// Ignorer la valeur "None"
		if (EClassArchetype ClassArchetype = static_cast<EClassArchetype>(EnumPtr->GetValueByIndex(i)); ClassArchetype != EClassArchetype::None)
		{
			// Créer une nouvelle entrée avec des valeurs par défaut
			FInternalAttributeData NewData = *GetDefaultGameplay(ClassArchetype);
			ArchetypesData.Add(ClassArchetype, NewData);
			
			UE_LOG(LogTemp, Log, TEXT("  -> Archétype %s initialisé (Health: %.1f)"), 
				*EnumPtr->GetNameStringByValue(static_cast<int64>(ClassArchetype)), 
				NewData.Health);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("PlayerSaveGame: %d archétypes initialisés"), ArchetypesData.Num());
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : HasArchetypeData
// ═══════════════════════════════════════════════════════════════════════════

bool UPlayerSaveGame::HasArchetypeData(const EClassArchetype InArchetype) const
{
	// Vérifier que l'archétype est valide
	if (InArchetype == EClassArchetype::None)
	{
		return false;
	}

	// Vérifier si l'archétype existe dans la map
	return ArchetypesData.Contains(InArchetype);
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODE : GetArchetypeDataConst
// ═══════════════════════════════════════════════════════════════════════════

const FInternalAttributeData* UPlayerSaveGame::GetArchetypeDataConst(const EClassArchetype InArchetype) const
{
	// Vérifier que l'archétype est valide
	if (InArchetype == EClassArchetype::None)
	{
		return nullptr;
	}

	// Retourner le pointeur (nullptr si inexistant)
	return ArchetypesData.Find(InArchetype);
}

const FInternalAttributeData* UPlayerSaveGame::GetDefaultGameplay(const EClassArchetype InArchetype)
{
	const UDataTable* Table = GetTable(TablePaths::DefaultGameplay);
	if (!Table) return nullptr;

	TArray<FDefaultArchetypeData*> Rows;
	Table->GetAllRows("GetDefaultGameplay", Rows);

	const FDefaultArchetypeData* const* FoundRow = Rows.FindByPredicate([InArchetype](const FDefaultArchetypeData* Row)
	{
		return Row && Row->Archetype == InArchetype;
	});

	return FoundRow ? &(*FoundRow)->Data : nullptr;
}
