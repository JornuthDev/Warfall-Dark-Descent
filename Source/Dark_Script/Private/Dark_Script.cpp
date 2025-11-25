// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dark_Script.h"

#define LOCTEXT_NAMESPACE "FDark_ScriptModule"

void FDark_ScriptModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDark_ScriptModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDark_ScriptModule, Dark_Script)