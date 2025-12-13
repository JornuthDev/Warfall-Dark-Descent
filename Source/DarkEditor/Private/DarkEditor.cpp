#include "DarkEditor.h"
#include "Utils/Helpers/EditorHelpers.h"
#include "DarkScript.h"

#define LOCTEXT_NAMESPACE "FDarkEditorModule"

void FDarkEditorModule::StartupModule()
{
	DarkSlate::Load();
	FDarkScriptModule::OnGetCustomIcon.BindLambda([](FName IconName) -> const FSlateBrush*
	{
		if (DarkSlate::StyleInstance.IsValid())
		{
			return DarkSlate::Get().GetBrush(IconName);
		}
		return FAppStyle::GetBrush("Icons.Default");
	});
	DarkProperty::Load();
}

void FDarkEditorModule::ShutdownModule()
{
	FDarkScriptModule::OnGetCustomIcon.Unbind();
	DarkSlate::Unload();
	DarkProperty::Unload();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDarkEditorModule, DarkEditor)