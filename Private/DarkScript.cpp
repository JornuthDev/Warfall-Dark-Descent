#include "DarkScript.h"

#define LOCTEXT_NAMESPACE "FDarkScriptModule"

FGetCustomIcon FDarkScriptModule::OnGetCustomIcon;

void FDarkScriptModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDarkScriptModule::ShutdownModule()
{
	OnGetCustomIcon.Unbind();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDarkScriptModule, DarkScript)