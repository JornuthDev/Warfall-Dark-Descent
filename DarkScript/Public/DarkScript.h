#pragma once
#include "Modules/ModuleManager.h"
#include "Styling/SlateBrush.h"

DECLARE_DELEGATE_RetVal_OneParam(const FSlateBrush*, FGetCustomIcon, FName);

class FDarkScriptModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	static DARKSCRIPT_API FGetCustomIcon OnGetCustomIcon;
	
	static const FSlateBrush* GetDarkSlate(const FName& IconName)
	{
		if (OnGetCustomIcon.IsBound())
		{
			return OnGetCustomIcon.Execute(IconName);
		}
		return FAppStyle::GetBrush("Icons.Default");
	}
};
