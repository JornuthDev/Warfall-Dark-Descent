#pragma once
#include "CoreMinimal.h"
#include "DarkScript.h"
#include "Interfaces/IPluginManager.h"
#include "PropertyCustom/ThumbnailEditor.h"
#include "Styling/SlateStyleRegistry.h"

namespace DarkSlate
{
	static TSharedPtr<FSlateStyleSet> StyleInstance;
	static TArray<FString> DarkIcons {
		"Search_Icon",
		"Triangular_Rule_Icon"   
	};
	inline void Load()
	{
		if (StyleInstance.IsValid())
			return;
        
		StyleInstance  = MakeShareable(new FSlateStyleSet("DarkSlate"));
		
		FString ContentRoot = IPluginManager::Get().FindPlugin("DarkScript")->GetBaseDir() / TEXT("Resources/Icons");
		
		StyleInstance->SetContentRoot(ContentRoot);
		for (const FString& Name : DarkIcons)
		{
			FString Path = Name + ".png";
			FString FullPath = StyleInstance->RootToContentDir(Path);
			StyleInstance->Set(*FString::Printf(TEXT("DarkSlate.%s"), *Name), new FSlateImageBrush(StyleInstance->RootToContentDir(Path), FVector2D(64, 64)));
		}
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	};
	inline void Unload()
	{
		if (!StyleInstance.IsValid())
			return;
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
	inline ISlateStyle& Get()
	{
		return *StyleInstance.Get();
	}
}

namespace DarkProperty
{
	static TMap<FName, FOnGetPropertyTypeCustomizationInstance> CustomProperties = {
		{ "Thumbnail", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FThumbnailEditor::MakeInstance) }
	};
	inline void Load()
	{
		FPropertyEditorModule* PropertyModule = &FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		for (const auto& Pair : CustomProperties)
		{
			PropertyModule->RegisterCustomPropertyTypeLayout(Pair.Key, Pair.Value);
		}
	}
	inline void Unload()
	{
		FPropertyEditorModule* PropertyModule = &FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");		
		for (const auto& Pair : CustomProperties)
		{
			PropertyModule->UnregisterCustomPropertyTypeLayout(Pair.Key);
		}
	}
}