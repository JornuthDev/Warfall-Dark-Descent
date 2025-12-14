// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToolsLibrary.generated.h"

struct FThumbnailMaterialParams;
struct FThumbnail;

/**
 * Tools Library
 * Bibliothèque de fonctions utilitaires pour les outils de développement
 */
UCLASS()
class DARKSCRIPT_API UToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════════════
	// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════════════

	/**
	 * Exporte un matériel à partir d'une thumbnail
	 * Crée une MaterialInstanceDynamic configurée selon les paramètres fournis
	 * @param Thumbnail La thumbnail source
	 * @param Material Le matériel de base à utiliser
	 * @param Params Les paramètres de configuration du matériel
	 * @param Color La couleur à appliquer (par défaut: transparent)
	 * @return L'instance de matériel dynamique créée
	 */
	UFUNCTION(BlueprintPure, Category="Thumbnail")
	static UMaterialInstanceDynamic* ExportMaterial(const FThumbnail Thumbnail, UMaterialInterface* Material,
		const FThumbnailMaterialParams& Params,
		const FLinearColor& Color = FLinearColor::Transparent);
};
