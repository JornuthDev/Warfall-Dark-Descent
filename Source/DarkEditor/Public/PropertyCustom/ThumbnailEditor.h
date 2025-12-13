#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "Utils/Tools/ThumbnailMaker.h"

class SColorPicker;
// Forward declarations
class IPropertyHandle;
class IPropertyUtilities;
class AThumbnailStudio;
class SThumbnailCopilot;
class SButton;

// ==========================================================================
// FTHUMBNAILEDITOR - Property Type Customization
// ==========================================================================

class FThumbnailEditor : public IPropertyTypeCustomization
{
public:
	
	FThumbnailEditor()
	:ThumbnailStudio(nullptr)
	,ThumbnailHandle(nullptr)
	,bIsPreviewVisible(false)
	{}
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(
		TSharedRef<IPropertyHandle> StructPropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	virtual void CustomizeChildren(
		TSharedRef<IPropertyHandle> StructPropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	// === Widget Creation ===
	TSharedRef<SWidget> CreateParametersWidget();
	TSharedRef<SWidget> CreatePreviewAndControlsWidget();
	TSharedRef<SWidget> CreateBrightnessSlider() const;
	TSharedRef<SWidget> CreateAttenuationSlider() const;
	TSharedRef<SWidget> CreateResetButton();

	// === Button Callbacks ===
	FReply OnCreateOrUpdateClicked();
	void OnDefaultParamsClicked(const bool bTransformParams = true) const;

	// === Helpers ===
	void InitializeStudio();
	void CleanupStudio();
	void Refresh() const;
	void SaveThumbnail() const;
	static void LoadThumbnail();
	bool HasExistingThumbnail() const;
	void TogglePreviewVisibility();
	
	/** Get the thumbnail struct pointer from property handle */
	template<typename StructType>
	static StructType* GetStructPropertyPtr(const TSharedRef<IPropertyHandle>& Handle)
	{
		const FStructProperty* StructProp = CastField<FStructProperty>(Handle->GetProperty());
		void* RawData = nullptr;

		if (StructProp && Handle->GetValueData(RawData) == FPropertyAccess::Success)
		{
			return static_cast<StructType*>(RawData);
		}

		return nullptr;
	}

	// === References ===
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyUtilities> PropertyUtils;
	
	TWeakObjectPtr<AThumbnailStudio> ThumbnailStudio;
	FThumbnail* ThumbnailHandle;
	mutable FThumbnail ThumbnailBackup;

	// === UI Elements ===
	TSharedPtr<SThumbnailCopilot> ThumbnailCopilot;
	TSharedPtr<SButton> SaveButton;
	TSharedPtr<SButton> LastSaveButton;
	TSharedPtr<SButton> ResetTransformButton;
	TSharedPtr<SButton> ResetLightButton;
	
	// === State ===
	bool bIsPreviewVisible;
};