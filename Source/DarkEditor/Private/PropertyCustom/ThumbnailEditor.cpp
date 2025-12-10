#include "PropertyCustom/ThumbnailEditor.h"

#include "DetailWidgetRow.h"
#include "Utils/Tools/ThumbnailMaker.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Utils/Helpers/ContentHelpers.h"

// ==========================================================================
// FTHUMBNAILEDITOR - Implementation
// ==========================================================================

TSharedRef<IPropertyTypeCustomization> FThumbnailEditor::MakeInstance()
{
	return MakeShareable(new FThumbnailEditor());
}

void FThumbnailEditor::CustomizeHeader(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	PropertyUtils = StructCustomizationUtils.GetPropertyUtilities();

	ThumbnailHandle = GetStructPropertyPtr<FThumbnail>(StructHandle.ToSharedRef());
	ThumbnailBackup = FThumbnail();
	ThumbnailStudio = nullptr;
	
	ThumbnailCopilot = SNew(SThumbnailCopilot).DesiredPreviewSize(512.0f);
	
	bIsPreviewVisible = ThumbnailHandle->bEditorPreviewVisible;
	ThumbnailHandle->bEditorPreviewVisible = false;
	
	ThumbnailBackup.SynchronizeProperties(ThumbnailHandle);
	ThumbnailBackup.Temp = ThumbnailHandle;
	CleanupStudio();
	
	if (bIsPreviewVisible)
	{
		InitializeStudio();
	}
	
	HeaderRow
		.NameContent()
		.MinDesiredWidth(300)
		.MaxDesiredWidth(300)
		[
			StructHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Left)
		[
			SNew(SButton)
			.Text_Lambda([this]()
			{
				return bIsPreviewVisible ? FText::FromString("Hide") : FText::FromString("Preview");
			})
			.HAlign(HAlign_Center)
			.ContentPadding(FMargin(-2, 3.2, 0, 3))
			.ToolTipText_Lambda([this]() -> FText
			{
				return bIsPreviewVisible 
				? FText::FromString("Collapse Copilot properties panel.")
				: FText::FromString("Preview Copilot properties panel.");
			})
			.OnClicked_Lambda([this] ()
			{
				TogglePreviewVisibility();
				return FReply::Handled();
			})
		];
}

void FThumbnailEditor::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Add Thumbnail (always visible)
	TSharedPtr<IPropertyHandle> ThumbnailTextureHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FThumbnail, Thumbnail));
	if (ThumbnailTextureHandle.IsValid())
	{
		ChildBuilder.AddProperty(ThumbnailTextureHandle.ToSharedRef());
	}

	// Add BaseLayer (always visible)
	TSharedPtr<IPropertyHandle> BaseLayerHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FThumbnail, BaseLayer));
	if (BaseLayerHandle.IsValid())
	{
		ChildBuilder.AddProperty(BaseLayerHandle.ToSharedRef());
	}
	
	ChildBuilder.AddCustomRow(FText::FromString("DimensionsPreview"))
	.NameContent()
	.VAlign(VAlign_Center)
	[
		StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FThumbnail, Dimensions))->CreatePropertyNameWidget()
	]
	.ValueContent()
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.MaxWidth(35)
		[
			SNew(SSpinBox<int32>)
			.MinValue(1)
			.MaxValue(10)
			.Value_Lambda([this]() { return ThumbnailBackup.Dimensions.X ;})
			.OnValueChanged_Lambda([this] (int32 NewValue)
			{
				ThumbnailBackup.Dimensions.X = NewValue;
				Refresh();
			})
			.MinDesiredWidth(15.f)
			.Justification(ETextJustify::Center)
		]
		+SHorizontalBox::Slot()
		.Padding(FMargin(2, 0, 0, 0))
		.MaxWidth(35)
		[
			SNew(SSpinBox<int32>)
			.MinValue(1)
			.MaxValue(10)
			.Value_Lambda([this]() { return ThumbnailBackup.Dimensions.Y ;})
			.OnValueChanged_Lambda([this] (int32 NewValue)
			{
				ThumbnailBackup.Dimensions.Y = NewValue;
				Refresh();
			})
			.MinDesiredWidth(15.f)
			.Justification(ETextJustify::Center)
		]
	];
		
	// === Main Preview Row (2 columns layout) ===
	ChildBuilder.AddCustomRow(FText::FromString("ThumbnailPreview"))
		.Visibility(TAttribute<EVisibility>::CreateLambda([this]()
		{
			return bIsPreviewVisible ? EVisibility::Visible : EVisibility::Collapsed;
		}))
		.NameContent()
		.MinDesiredWidth(300.f)
		.MaxDesiredWidth(300.f)
		.HAlign(HAlign_Center)
		[
			// Left column: Parameters
			CreateParametersWidget()
		]
		.ValueContent()
		.MinDesiredWidth(512.f)
		.MaxDesiredWidth(512.f)
		.HAlign(HAlign_Center)
		[
			// Right column: Preview
			CreatePreviewAndControlsWidget()
		];
}

TSharedRef<SWidget> FThumbnailEditor::CreateParametersWidget()
{
	return SNew(SVerticalBox)
		// Asset Name
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Name"))
				.MinDesiredWidth(80.0f)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.MinWidth(200)
			.MaxWidth(200)
			.VAlign(VAlign_Center)
			[
				SNew(SEditableTextBox)
				.Text_Lambda([this]()
				{
					return ThumbnailHandle ? FText::FromString(ThumbnailBackup.AssetName) : FText::FromString("Default");
				})
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type)
				{
					if (ThumbnailHandle)
					{
						ThumbnailBackup.AssetName = NewText.ToString();
					}
				})
				.OnTextChanged_Lambda([this](const FText& NewText)
				{
					if (ThumbnailHandle)
					{
						ThumbnailBackup.AssetName = NewText.ToString();
					}
				})
				.Justification(ETextJustify::Center)
			]
		]

		// Mesh
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Mesh"))
				.MinDesiredWidth(80.0f)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.MinWidth(200)
			.MaxWidth(200)
			.VAlign(VAlign_Center)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UStaticMesh::StaticClass())
				.DisplayThumbnail(true)
				.ThumbnailPool(UThumbnailManager::Get().GetSharedThumbnailPool())
				.DisplayCompactSize(true)
				.ObjectPath_Lambda([this] ()
				{
					return ThumbnailBackup.Mesh ? ThumbnailBackup.Mesh->GetPathName() : FString();
				})
				.OnObjectChanged_Lambda([this] (const FAssetData& AssetData)
				{
					ThumbnailBackup.Mesh = Cast<UStaticMesh>(AssetData.GetAsset());
					if (ThumbnailBackup.Mesh)
					{
						ThumbnailBackup.SkeletalMesh = nullptr;
					}
					Refresh();
				})
			]
		]

		// Skeletal Mesh
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Skeletal Mesh"))
				.MinDesiredWidth(80.0f)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.MinWidth(200)
			.MaxWidth(200)
			.VAlign(VAlign_Center)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(USkeletalMesh::StaticClass())
				.DisplayThumbnail(true)
				.ThumbnailPool(UThumbnailManager::Get().GetSharedThumbnailPool())
				.DisplayCompactSize(true)
				.ObjectPath_Lambda([this] ()
				{
					return ThumbnailBackup.SkeletalMesh ? ThumbnailBackup.SkeletalMesh->GetPathName() : FString();
				})
				.OnObjectChanged_Lambda([this] (const FAssetData& AssetData)
				{
					ThumbnailBackup.SkeletalMesh = Cast<USkeletalMesh>(AssetData.GetAsset());
					if (ThumbnailBackup.SkeletalMesh)
					{
						ThumbnailBackup.Mesh = nullptr;
					}
					Refresh();
				})
			]
		]
		
		// Light Settings
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(0.0f, 15.0f, 0.0f, 10.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Light Settings"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
		]
		// Intensity
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			CreateBrightnessSlider()
		]
		// Attenuation Radius
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			CreateAttenuationSlider()
		]
		// Light Color
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Light Color"))
				.MinDesiredWidth(80.0f)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.MinWidth(200)
			.MaxWidth(200)
			.Padding(5, 0, 0, 0)
			.VAlign(VAlign_Center)
			[
				SNew(SColorBlock)
				.Color_Lambda([this]()
				{
					return ThumbnailHandle ? ThumbnailBackup.SceneColor : FLinearColor::White;
				})
				.ShowBackgroundForAlpha(true)
				.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
				.OnMouseButtonDown_Lambda([this] (const FGeometry&, const FPointerEvent&) -> FReply
				{
					FColorPickerArgs PickerArgs;
					PickerArgs.bUseAlpha = false;
					PickerArgs.bOnlyRefreshOnMouseUp = false;
					PickerArgs.bOnlyRefreshOnOk = false;
					PickerArgs.sRGBOverride = false;
					PickerArgs.DisplayGamma = TAttribute<float>(2.2f);
					PickerArgs.InitialColor = ThumbnailBackup.SceneColor;
					
					PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda([this](FLinearColor NewColor)
					{
						if (ThumbnailHandle)
						{
							ThumbnailBackup.SceneColor = NewColor;
							Refresh();
						}
					});
					OpenColorPicker(PickerArgs);
					return FReply::Handled();
				})
				.Size(FVector2D(50.f, 20.f))
			]
		]
		
		// Create/Update Thumbnail Button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)
			.MaxDesiredWidth(200.0f)
			.HAlign(HAlign_Center)
			[
				SAssignNew(SaveButton, SButton)
				.ButtonColorAndOpacity_Lambda([this]() -> FLinearColor
				{
					return SaveButton.IsValid() && SaveButton->IsHovered()
					? ColorDesign::ButtonHover
					: ColorDesign::ButtonNormal;
				})
				.Text_Lambda([this]()
				{
					return HasExistingThumbnail() ? FText::FromString("Update Thumbnail") : FText::FromString("Create Thumbnail");
				})
				.OnClicked(this, &FThumbnailEditor::OnCreateOrUpdateClicked)
			]
		];
}

TSharedRef<SWidget> FThumbnailEditor::CreatePreviewAndControlsWidget()
{
	return SNew(SVerticalBox)

		// Preview (Copilot)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				ThumbnailCopilot.IsValid() ? ThumbnailCopilot.ToSharedRef() : SNullWidget::NullWidget
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(10.f,10.f)
			[
				CreateResetButton()
			]
		]

		// Dimensions display (bottom)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				if (!ThumbnailHandle) return FText::FromString("512x512");
								
				// Calculate crop size (native resolution, no upscale)
				constexpr int32 SourceSize = AThumbnailStudio::GetSourceSize();
				const int32 MaxDim = FMath::Max(ThumbnailBackup.Dimensions.X, ThumbnailBackup.Dimensions.Y);
				const int32 CropWidth = (SourceSize * ThumbnailBackup.Dimensions.X) / MaxDim;
				const int32 CropHeight = (SourceSize * ThumbnailBackup.Dimensions.Y) / MaxDim;
								
				return FText::FromString(FString::Printf(TEXT("%dx%d"), CropWidth, CropHeight));
			})
		];
}

TSharedRef<SWidget> FThumbnailEditor::CreateBrightnessSlider() const
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Intensity"))
			.MinDesiredWidth(80.0f)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.MinWidth(200)
		.MaxWidth(200)
		.VAlign(VAlign_Center)
		.Padding(5.0f, 0.0f)
		[
			SNew(SSlider)
			.MinValue(0.0f)
			.MaxValue(2.0f)
			.Value_Lambda([this]() { return ThumbnailHandle ? ThumbnailBackup.SceneBrightness : 1.0f; })
			.OnValueChanged_Lambda([this](float Value)
			{
				if (ThumbnailHandle && ThumbnailStudio.IsValid())
				{
					ThumbnailBackup.SceneBrightness = Value;
					Refresh();
				}
			})
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.MinWidth(50.0f)
		.MaxWidth(50.0f)
		.Padding(5.0f, 0.0f)
		[
			SNew(SNumericEntryBox<float>)
			.MinDesiredValueWidth(50.0f)
			.MaxFractionalDigits(3)
			.Value_Lambda([this]() { return ThumbnailHandle ? ThumbnailBackup.SceneBrightness : 1.0f; })
			.OnValueChanged_Lambda([this](float Value)
			{
				if (ThumbnailHandle && ThumbnailStudio.IsValid())
				{
					ThumbnailBackup.SceneBrightness = FMath::Clamp(Value, 0.0f, 2.0f);
					Refresh();
				}
			})
		];
}

TSharedRef<SWidget> FThumbnailEditor::CreateAttenuationSlider() const
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Attenuation"))
			.MinDesiredWidth(80.0f)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.MinWidth(200)
		.MaxWidth(200)
		.VAlign(VAlign_Center)
		.Padding(5.0f, 0.0f)
		[
			SNew(SSlider)
			.MinValue(0.0f)
			.MaxValue(1.0f)
			.Value_Lambda([this]() { return ThumbnailHandle ? ThumbnailBackup.SceneAttenuation : 1.0f; })
			.OnValueChanged_Lambda([this](float Value)
			{
				if (ThumbnailHandle && ThumbnailStudio.IsValid())
				{
					ThumbnailBackup.SceneAttenuation = Value;
					Refresh();
				}
			})
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.MinWidth(50.0f)
		.MaxWidth(50.0f)
		.Padding(5.0f, 0.0f)
		[
			SNew(SNumericEntryBox<float>)
			.MinDesiredValueWidth(50.0f)
			.MaxFractionalDigits(3)
			.Value_Lambda([this]() { return ThumbnailHandle ? ThumbnailBackup.SceneAttenuation : 1.0f; })
			.OnValueChanged_Lambda([this](float Value)
			{
				if (ThumbnailHandle && ThumbnailStudio.IsValid())
				{
					ThumbnailBackup.SceneAttenuation = FMath::Clamp(Value, 0.0f, 1.0f);
					Refresh();
				}
			})
		];
}

FReply FThumbnailEditor::OnCreateOrUpdateClicked()
{
	if (!ThumbnailHandle) return FReply::Handled();

	// Initialize studio if needed
	if (!ThumbnailStudio.IsValid())
	{
		InitializeStudio();
	}

	ThumbnailHandle->SynchronizeProperties(&ThumbnailBackup);
	
	// Save the thumbnail
	SaveThumbnail();
		
	return FReply::Handled();
}

void FThumbnailEditor::OnDefaultParamsClicked(const bool bTransformParams) const
{
	if (!ThumbnailHandle) return;
	
	ThumbnailHandle->ResetParameters(bTransformParams);
	
	if (PropertyUtils.IsValid())
	{	
		ThumbnailHandle->bEditorPreviewVisible = bIsPreviewVisible;
		PropertyUtils->ForceRefresh();
	}
}

void FThumbnailEditor::InitializeStudio()
{
	if (!ThumbnailHandle) return;

	// Spawn studio actor
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;
	
	for (const TActorIterator<AThumbnailStudio> It(World); It;)
	{
		ThumbnailStudio = *It;
		break;
	}
	
	if (!ThumbnailStudio.IsValid())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = MakeUniqueObjectName(World, AThumbnailStudio::StaticClass(), TEXT("ThumbnailStudio"));

		ThumbnailStudio = World->SpawnActor<AThumbnailStudio>(AThumbnailStudio::StaticClass(), FTransform::Identity, SpawnParams);
	}
	
	if (ThumbnailStudio.IsValid())
	{
		ThumbnailStudio->Initialize(&ThumbnailBackup);
		
		if (ThumbnailCopilot.IsValid())
		{
			ThumbnailCopilot->Initialize(ThumbnailStudio, &ThumbnailBackup);
		}
		Refresh();
	}	
}

void FThumbnailEditor::CleanupStudio()
{
	const UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;
	
	for (const TActorIterator<AThumbnailStudio> It(World); It;)
	{
		It->Destroy();
		break;
	}
	ThumbnailStudio = nullptr;
}

void FThumbnailEditor::SaveThumbnail() const
{
	if (!ThumbnailStudio.IsValid() || !ThumbnailHandle) return;

	ThumbnailStudio->SaveRenderTargetToDisk();
}

void FThumbnailEditor::LoadThumbnail()
{
	// Load thumbnail data if it exists
	// This would typically load from disk or a saved asset
}

bool FThumbnailEditor::HasExistingThumbnail() const
{
	return ThumbnailHandle && ThumbnailHandle->Thumbnail != nullptr;
}

void FThumbnailEditor::TogglePreviewVisibility()
{
	bIsPreviewVisible = !bIsPreviewVisible;
	ThumbnailHandle->bEditorPreviewVisible = bIsPreviewVisible;

	if (PropertyUtils.IsValid())
	{
		PropertyUtils->ForceRefresh();
	}
}

void FThumbnailEditor::Refresh() const
{
	if (ThumbnailStudio.IsValid())
	{
		ThumbnailStudio->Refresh();
		
		if (ThumbnailCopilot.IsValid())
		{
			ThumbnailCopilot->UpdateRenderBrush();
		}
	}
}

TSharedRef<SWidget> FThumbnailEditor::CreateResetButton()
{
	return 
	SNew(SComboButton)
	.ButtonStyle(FAppStyle::Get(), "SimpleButton")
	.ContentPadding(FMargin(2.f))
	.ForegroundColor(FSlateColor(FLinearColor::White))
	.ButtonColorAndOpacity(ColorDesign::ButtonNormal)	
	.HasDownArrow(true)
	.ButtonContent()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Reset"))
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
	]
	.MenuContent()
	[
		SNew(SVerticalBox)
		
		// ========== LAST SAVE ==========
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SAssignNew(LastSaveButton, SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor(FLinearColor::White))
			.ButtonColorAndOpacity_Lambda([this]() -> FLinearColor
			{
				return LastSaveButton.IsValid() && LastSaveButton->IsHovered()
					? ColorDesign::ButtonHover
					: ColorDesign::ButtonNormal;
			})
			.OnClicked_Lambda([this]() -> FReply
			{
				// Annuler toutes les modifications (restore from Handle)
				if (PropertyUtils.IsValid())
				{
					ThumbnailHandle->bEditorPreviewVisible = bIsPreviewVisible;
					PropertyUtils->ForceRefresh();
				}
				
				Refresh();
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(FText::FromString("Last Save"))
				.ToolTipText(FText::FromString("Cancel all modifications and restore saved values"))
			]
		]
		
		// ========== DEFAULT TRANSFORM ==========
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SAssignNew(ResetTransformButton, SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor(FLinearColor::White))
			.ButtonColorAndOpacity_Lambda([this]() -> FLinearColor
			{
				return ResetTransformButton.IsValid() && ResetTransformButton->IsHovered()
					? ColorDesign::ButtonHover
					: ColorDesign::ButtonNormal;
			})
			.OnClicked_Lambda([this]() -> FReply
			{
				// Reset uniquement Transform (Camera + Object)
				ThumbnailBackup.ResetParameters(true);
				Refresh();
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(FText::FromString("Default Transform"))
				.ToolTipText(FText::FromString("Reset camera and object transform to default values"))
			]
		]
		
		// ========== DEFAULT SCENE LIGHTING ==========
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SAssignNew(ResetLightButton, SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ForegroundColor(FSlateColor(FLinearColor::White))
			.ButtonColorAndOpacity_Lambda([this]() -> FLinearColor
			{
				return ResetLightButton.IsValid() && ResetLightButton->IsHovered()
					? ColorDesign::ButtonHover
					: ColorDesign::ButtonNormal;
			})
			.OnClicked_Lambda([this]() -> FReply
			{
				// Reset uniquement Scene (Brightness, Attenuation, Color)
				ThumbnailBackup.ResetParameters(false);
				Refresh();
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(FText::FromString("Default Scene Lighting"))
				.ToolTipText(FText::FromString("Reset brightness, attenuation and color to default values"))
			]
		]
	];  
}