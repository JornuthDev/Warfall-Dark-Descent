// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToolsLibrary.generated.h"

struct FThumbnailMaterialParams;
struct FThumbnail;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category="Thumbnail")
	static UMaterialInstanceDynamic* ExportMaterial(const FThumbnail Thumbnail, UMaterialInterface* Material,
		const FThumbnailMaterialParams& Params,
		const FLinearColor& Color = FLinearColor::Transparent);
};
