// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DarkGameInstance.generated.h"

enum class EKickReason : uint8;
/**
 * 
 */
UCLASS()
class DARKSCRIPT_API UDarkGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Construction ──────────────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Blueprint API) ─────────────────────────────────────────────────
	UFUNCTION(BlueprintNativeEvent)
	void OnKicked(EKickReason KickReason);
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Functions (Internal) ──────────────────────────────────────────────────────
	
	// ───────────────────────────────────────────────────────────────────────────────
	// ─── Variables ─────────────────────────────────────────────────────────────────
};
