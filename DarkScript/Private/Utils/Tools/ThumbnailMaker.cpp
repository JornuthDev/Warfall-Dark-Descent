#include "Utils/Tools/ThumbnailMaker.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Engine/Texture2D.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionPower.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "UObject/SavePackage.h"
#include "Utils/Helpers/SystemsHelper.h"
#include "Widgets/Colors/SColorBlock.h"

// ==========================================================================
// FTHUMBNAIL - Implementation
// ==========================================================================

UMaterialInstanceDynamic* FThumbnail::ExportMaterial(
	UMaterialInterface* ParentMaterial,
	const FThumbnailMaterialParams& Params,
	const FLinearColor& Color) const
{
	if (!ParentMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExportMaterial: ParentMaterial is null"));
		return nullptr;
	}

	if (!Thumbnail)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExportMaterial: Thumbnail texture is null"));
		return nullptr;
	}

	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
	if (!Material)
	{
		UE_LOG(LogTemp, Error, TEXT("ExportMaterial: Failed to create dynamic material instance"));
		return nullptr;
	}

	// Set thumbnail texture if parameter is valid
	if (!Params.IconParameter.IsNone())
	{
		Material->SetTextureParameterValue(Params.IconParameter, Thumbnail);
	}

	// Set base layer usage if parameter is valid
	if (!Params.UsingLayerParameter.IsNone())
	{
		Material->SetScalarParameterValue(Params.UsingLayerParameter, BaseLayer ? 1.0f : 0.0f);
	}

	// Set base layer texture if parameter is valid and texture exists
	if (!Params.BaseLayerParameter.IsNone() && BaseLayer)
	{
		Material->SetTextureParameterValue(Params.BaseLayerParameter, BaseLayer);
	}

	// Set color if parameter is valid
	if (!Params.ColorParameter.IsNone())
	{
		Material->SetVectorParameterValue(Params.ColorParameter, Color);
	}

	return Material;
}

void FThumbnail::SynchronizeProperties(const FThumbnail* Source)
{
	if (!Source) return;

	Thumbnail = Source->Thumbnail;
	BaseLayer = Source->BaseLayer;
	AssetName = Source->AssetName;
	Dimensions = Source->Dimensions;
	Mesh = Source->Mesh;
	SkeletalMesh = Source->SkeletalMesh;
	ObjectScale = Source->ObjectScale;
	ObjectOffsetX = Source->ObjectOffsetX;
	ObjectOffsetY = Source->ObjectOffsetY;
	CameraYaw = Source->CameraYaw;
	CameraPitch = Source->CameraPitch;
	CameraRoll = Source->CameraRoll;
	CameraDistance = Source->CameraDistance;
	CameraSpeed = Source->CameraSpeed;
	SceneBrightness = Source->SceneBrightness;
	SceneAttenuation = Source->SceneAttenuation;
	SceneColor = Source->SceneColor;
}

bool FThumbnail::HasChanged(const bool bIsTransformParams) const
{
	const FThumbnail Default = FThumbnail();
	
	if (bIsTransformParams)
	{
		return  ObjectScale != Default.ObjectScale ||
				ObjectOffsetX != Default.ObjectOffsetX ||
				ObjectOffsetY != Default.ObjectOffsetY ||
			    CameraYaw != Default.CameraYaw ||
			    CameraPitch != Default.CameraPitch ||
			    CameraRoll != Default.CameraRoll ||	
			    CameraDistance != Default.CameraDistance ||
			    CameraSpeed != Default.CameraSpeed;
	}
	return  SceneBrightness != Default.SceneBrightness ||
			SceneAttenuation != Default.SceneAttenuation ||
			SceneColor != Default.SceneColor;
}

void FThumbnail::ResetParameters(const bool bIsTransformParams)
{
	const FThumbnail Default = FThumbnail();
	
	if (bIsTransformParams)
	{
		ObjectScale = Default.ObjectScale;
		ObjectOffsetX = Default.ObjectOffsetX;
		ObjectOffsetY = Default.ObjectOffsetY;
		CameraYaw = Default.CameraYaw;
		CameraPitch = Default.CameraPitch;
		CameraRoll = Default.CameraRoll;
		CameraDistance = Default.CameraDistance;
		CameraSpeed = Default.CameraSpeed;
	}
	else
	{
		SceneBrightness = Default.SceneBrightness;
		SceneAttenuation = Default.SceneAttenuation;
		SceneColor = Default.SceneColor;
	}
}

void FThumbnail::OnThumbnailOpened_Handle() const
{
	if (OnThumbnailOpened.IsBound())
	{
		OnThumbnailOpened.Execute(NAME_None);
	}
}

void FThumbnail::OnThumbnailChanged_Handle(const FName& Tag) const
{
	if (OnThumbnailChanged.IsBound())
	{
		OnThumbnailChanged.Execute(Tag);
	}
}

FThumbnail& FThumbnail::operator=(const FThumbnail& Other)
{
	if (this != &Other)
	{
		Thumbnail = Other.Thumbnail;
		BaseLayer = Other.BaseLayer;
		AssetName = Other.AssetName;
		Dimensions = Other.Dimensions;
		Mesh = Other.Mesh;
		SkeletalMesh = Other.SkeletalMesh;
		ObjectScale = Other.ObjectScale;
		ObjectOffsetX = Other.ObjectOffsetX;
		ObjectOffsetY = Other.ObjectOffsetY;
		CameraYaw = Other.CameraYaw;
		CameraPitch = Other.CameraPitch;
		CameraRoll = Other.CameraRoll;
		CameraDistance = Other.CameraDistance;
		CameraSpeed = Other.CameraSpeed;
		SceneBrightness = Other.SceneBrightness;
		SceneAttenuation = Other.SceneAttenuation;
		SceneColor = Other.SceneColor;
		Temp = Other.Temp;
	}
	return *this;
}

// ==========================================================================
// ATHUMBNAILSTUDIO - Implementation
// ==========================================================================

AThumbnailStudio::AThumbnailStudio()
	: RenderTarget(nullptr)
	, RenderMaterial(nullptr)
	, ThumbnailData(nullptr)
{
	SetActorEnableCollision(false);
	// Create scene root
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	DefaultSceneRoot->SetVisibility(false);
	DefaultSceneRoot->bHiddenInGame = true;
	RootComponent = DefaultSceneRoot;

	// Create static mesh component
	MeshObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshObject"));
	MeshObject->SetupAttachment(DefaultSceneRoot);
	MeshObject->SetRelativeLocation(FVector::ZeroVector);
	MeshObject->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshObject->SetRelativeScale3D(FVector::OneVector);
	MeshObject->SetCastShadow(false);

	// Create skeletal mesh component
	SkeletalObject = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalObject"));
	SkeletalObject->SetupAttachment(DefaultSceneRoot);
	SkeletalObject->SetRelativeLocation(FVector::ZeroVector);
	SkeletalObject->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SkeletalObject->SetRelativeScale3D(FVector::OneVector);
	SkeletalObject->SetCastShadow(false);
	SkeletalObject->SetVisibility(false);

	// Create camera (for Copilot preview - clean background)
	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	Camera->SetupAttachment(DefaultSceneRoot);
	Camera->ProjectionType = ECameraProjectionMode::Perspective;
	Camera->FOVAngle = 60.0f;
	Camera->bCaptureEveryFrame = false;
	Camera->bCaptureOnMovement = false;
	Camera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	Camera->ShowOnlyComponents.Add(MeshObject);
	Camera->ShowOnlyComponents.Add(SkeletalObject);
	Camera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	
	// ========== STUDIO LIGHTING SETUP (Multi-directional) ==========
	
	// Créer 6 lumières pour un éclairage uniforme
	const TArray<FVector> LightPositions = {
		FVector(400, 200, 200),    // Key Light (principale, 45° devant-droite)
		FVector(-200, -300, 100),  // Fill Light (remplissage, gauche)
		FVector(-300, 0, 300)      // Back Light (arrière-haut pour contours)
	};
	
	const TArray<float> LightIntensities = {
		2.0f,   // Key (principale)
		1.0f,   // Fill
		1.5f    // Back
	};
	
	for (int32 i = 0; i < LightPositions.Num(); ++i)
	{
		UPointLightComponent* StudioLight = CreateDefaultSubobject<UPointLightComponent>(
			*FString::Printf(TEXT("StudioLight_%d"), i)
		);
		StudioLight->SetupAttachment(DefaultSceneRoot);
		StudioLight->SetRelativeLocation(LightPositions[i]);
		StudioLight->bUseTemperature = true;
		StudioLight->Intensity = 10.0f * LightIntensities[i];  // Base intensity
		StudioLight->LightColor = FColor::White;
		StudioLight->AttenuationRadius = 2000.0f;
		StudioLight->bUseInverseSquaredFalloff = false;  // Lumière uniforme
		StudioLight->LightFalloffExponent = 2.0f;
		StudioLight->SetCastShadows(false);  // IMPORTANT : Pas d'ombres !
		StudioLights.Add(StudioLight);
	}
	
	// Configure camera show flags
	Camera->ShowFlags.SetAtmosphere(false);           // Pas de ciel
	Camera->ShowFlags.SetFog(false);                  // Pas de brouillard
	Camera->ShowFlags.SetSkyLighting(false);          // Pas de lumière du ciel (IMPORTANT!)
	Camera->ShowFlags.SetDynamicShadows(false);       // Pas d'ombres dynamiques
	Camera->ShowFlags.SetContactShadows(false);       // Pas d'ombres de contact
	Camera->ShowFlags.SetLighting(true);              // Lumière locale uniquement (notre RectLight)
	Camera->ShowFlags.SetMaterials(true);             // Afficher les matériaux
	Camera->ShowFlags.SetPostProcessing(false);       // Pas de post-process (pour éviter artifacts)
	Camera->ShowFlags.SetDirectionalLights(false);
	Camera->ShowFlags.SetSelection(false);
	Camera->ShowFlags.SetSelectionOutline(false);
	Camera->ShowFlags.SetBillboardSprites(false);
	
}

void AThumbnailStudio::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AThumbnailStudio::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

void AThumbnailStudio::Destroyed()
{
	Clear();
	Super::Destroyed();
}

void AThumbnailStudio::Initialize(FThumbnail* InThumbnailData)
{
	ThumbnailData = InThumbnailData;

	if (ThumbnailData)
	{
		// Create render target
		CreateRenderTarget();

		// Update all components
		Refresh();

		// Store default camera transform
		CameraDefaultTransform = Camera->GetComponentTransform();
	}
}

void AThumbnailStudio::Clear()
{
	ThumbnailData = nullptr;

	if (MeshObject)
	{
		MeshObject->SetStaticMesh(nullptr);
		MeshObject->SetVisibility(false);
	}

	if (SkeletalObject)
	{
		SkeletalObject->SetSkeletalMesh(nullptr);
		SkeletalObject->SetVisibility(false);
	}

	if (Camera)
	{
		Camera->TextureTarget = nullptr;
	}

	RenderTarget = nullptr;
}

void AThumbnailStudio::UpdateMesh() const
{
	if (!ThumbnailData) return;

	// Handle static mesh
	if (ThumbnailData->Mesh)
	{
		MeshObject->SetStaticMesh(ThumbnailData->Mesh);
		MeshObject->SetVisibility(true);
		SkeletalObject->SetVisibility(false);

		// Center the mesh by its bounding box
		FBoxSphereBounds Bounds = ThumbnailData->Mesh->GetBounds();
		FVector BoundsCenter = Bounds.Origin;
		FVector MeshLocation = -BoundsCenter;
		MeshLocation.Y = MeshLocation.Y + ThumbnailData->ObjectOffsetX;
		MeshLocation.Z = MeshLocation.Z + ThumbnailData->ObjectOffsetY;
		MeshObject->SetRelativeLocation(MeshLocation);
	}
	// Handle skeletal mesh
	else if (ThumbnailData->SkeletalMesh)
	{
		SkeletalObject->SetSkeletalMesh(ThumbnailData->SkeletalMesh);
		SkeletalObject->SetVisibility(true);
		MeshObject->SetVisibility(false);

		// Center the skeletal mesh by its bounding box
		FBoxSphereBounds Bounds = ThumbnailData->SkeletalMesh->GetBounds();
		FVector BoundsCenter = Bounds.Origin;
		FVector SkeletalLocation = -BoundsCenter;
		SkeletalLocation.Y = SkeletalLocation.Y + ThumbnailData->ObjectOffsetX;
		SkeletalLocation.Z = SkeletalLocation.Z + ThumbnailData->ObjectOffsetY;
		SkeletalObject->SetRelativeLocation(SkeletalLocation);
	}
	else
	{
		MeshObject->SetVisibility(false);
		SkeletalObject->SetVisibility(false);
	}
}

void AThumbnailStudio::UpdateCameraTransform() const
{
	if (!ThumbnailData || !Camera) return;

	// Clamp pitch to avoid gimbal lock
	const float ClampedPitch = FMath::Clamp(ThumbnailData->CameraPitch, -85.0f, 85.0f);
	ThumbnailData->CameraPitch = ClampedPitch;

	// Convert spherical coordinates to cartesian
	const float PitchRad = FMath::DegreesToRadians(ClampedPitch);
	const float YawRad = FMath::DegreesToRadians(ThumbnailData->CameraYaw);

	const float X = ThumbnailData->CameraDistance * FMath::Cos(PitchRad) * FMath::Cos(YawRad);
	const float Y = ThumbnailData->CameraDistance * FMath::Cos(PitchRad) * FMath::Sin(YawRad);
	const float Z = ThumbnailData->CameraDistance * FMath::Sin(PitchRad);

	const FVector CameraLocation(X, Y, Z);
	FRotator CameraRotation = (-CameraLocation).Rotation();
	CameraRotation.Roll = ThumbnailData->CameraRoll;
	
	// Synchronize both cameras
	Camera->SetRelativeLocation(CameraLocation);
	Camera->SetRelativeRotation(CameraRotation);
}

void AThumbnailStudio::UpdateObjectScale() const
{
	if (!ThumbnailData) return;

	const FVector UniformScale(ThumbnailData->ObjectScale);

	if (MeshObject->IsVisible())
	{
		MeshObject->SetRelativeScale3D(UniformScale);
	}

	if (SkeletalObject->IsVisible())
	{
		SkeletalObject->SetRelativeScale3D(UniformScale);
	}
}

void AThumbnailStudio::UpdateSceneParameters() const
{
	if (!ThumbnailData) return;

	// ========== UPDATE STUDIO LIGHTS ==========
	const float BrightnessMultiplier = ThumbnailData->SceneBrightness;
	const float AttenuationMultiplier = ThumbnailData->SceneAttenuation;
	const FLinearColor SceneColor = ThumbnailData->SceneColor;
	
	const TArray<float> BaseLightIntensities = {
		1.5f,   // Front
		0.8f,   // Back
		1.0f,   // Right
		1.0f,   // Left
		1.2f,   // Top
		0.5f    // Bottom
	};
	
	for (int32 i = 0; i < StudioLights.Num(); ++i)
	{
		if (StudioLights[i])
		{
			StudioLights[i]->SetIntensity(10.0f * BaseLightIntensities[i] * BrightnessMultiplier);
			StudioLights[i]->SetAttenuationRadius(2000.f * AttenuationMultiplier);
			StudioLights[i]->MarkRenderStateDirty();
		}
	}
	if (StudioLights.Num() > 0 && StudioLights[0])
	{
		StudioLights[0]->LightColor = SceneColor.ToFColor(true);
		StudioLights[0]->MarkRenderStateDirty();
	}
}

void AThumbnailStudio::Refresh() const
{
	if (!Camera || !RenderTarget) return;

	// Update everything
	UpdateMesh();
	UpdateObjectScale();
	UpdateCameraTransform();
	UpdateSceneParameters();

	// Capture the scene
	Camera->CaptureScene();
}

UTextureRenderTarget2D* AThumbnailStudio::CreateRenderTarget()
{
	if (!ThumbnailData) return nullptr;

	constexpr int32 PreviewSize = GetSourceSize();

	// Create or update main render target (for Copilot preview)
	if (!RenderTarget)
	{
		RenderTarget = NewObject<UTextureRenderTarget2D>();
	}
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->ClearColor = FLinearColor(0, 0, 0, 0);
	RenderTarget->bForceLinearGamma = false;
	RenderTarget->InitAutoFormat(PreviewSize, PreviewSize);
	RenderTarget->UpdateResourceImmediate(true);
	
	// Assign to cameras
	if (Camera)
	{
		Camera->TextureTarget = RenderTarget;
	}

	return RenderTarget;
}
	
void AThumbnailStudio::SaveRenderTargetToDisk() const
{
	if (!RenderTarget || !Camera || !ThumbnailData->Temp) return;

	// ========== SANITIZE ASSET NAME ==========
	FString SanitizedName = ThumbnailData->Temp->AssetName;
	SanitizedName = SanitizedName.Replace(TEXT(" "), TEXT("_"));
	
	FString CleanName;
	for (int32 i = 0; i < SanitizedName.Len(); ++i)
	{
		TCHAR Char = SanitizedName[i];
		if (FChar::IsAlnum(Char) || Char == '_')
		{
			CleanName.AppendChar(Char);
		}
	}
	
	if (CleanName.IsEmpty())
	{
		CleanName = TEXT("Thumbnail_Default");
	}
	
	ThumbnailData->Temp->AssetName = CleanName;

	// ========== CAPTURE SCENE ==========
	Camera->CaptureScene();

	// ========== READ PIXELS FROM RENDER TARGET ==========
	TArray<FColor> SourcePixels;
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get render target resource"));
		return;
	}

	RTResource->ReadPixels(SourcePixels);
	
	// ========== INVERSER L'ALPHA (MAGIC!) ==========
	for (FColor& Pixel : SourcePixels)
	{
		Pixel.A = 255 - Pixel.A;  // Inverse l'alpha !
	}
	
	// ========== CROP TO DIMENSIONS ==========
	constexpr int32 SourceSize = GetSourceSize();
	const int32 MaxDim = FMath::Max(ThumbnailData->Dimensions.X, ThumbnailData->Dimensions.Y);
	const int32 CropWidth = (SourceSize * ThumbnailData->Dimensions.X) / MaxDim;
	const int32 CropHeight = (SourceSize * ThumbnailData->Dimensions.Y) / MaxDim;
	const int32 CropOffsetX = (SourceSize - CropWidth) / 2;
	const int32 CropOffsetY = (SourceSize - CropHeight) / 2;

	// Extract cropped pixels
	TArray<FColor> CroppedPixels;
	CroppedPixels.SetNum(CropWidth * CropHeight);
	
	for (int32 y = 0; y < CropHeight; ++y)
	{
		for (int32 x = 0; x < CropWidth; ++x)
		{
			const int32 SourceX = CropOffsetX + x;
			const int32 SourceY = CropOffsetY + y;
			const int32 SourceIndex = SourceY * SourceSize + SourceX;
			const int32 CropIndex = y * CropWidth + x;
			
			CroppedPixels[CropIndex] = SourcePixels[SourceIndex];
		}
	}

	// ========== CHECK IF ASSET ALREADY EXISTS ==========
	const FString Folder = TEXT("/DarkScript/Data/Thumbnails");
	const FString AssetPath = Folder + TEXT("/") + CleanName;

	if (UTexture2D* ExistingTexture = LoadObject<UTexture2D>(nullptr, *AssetPath))
	{
		// ========== UPDATE EXISTING TEXTURE ==========
		UE_LOG(LogTemp, Log, TEXT("Updating existing texture: %s"), *CleanName);
		
		// Modify the texture (mark for editing)
		ExistingTexture->Modify();
		
		#if WITH_EDITORONLY_DATA
			// Reinitialize source data with new dimensions
			ExistingTexture->Source.Init(CropWidth, CropHeight, 1, 1, TSF_BGRA8);
			
			// Copy cropped pixels
		if (uint8* MipData = ExistingTexture->Source.LockMip(0))
			{
				FMemory::Memcpy(MipData, CroppedPixels.GetData(), CroppedPixels.Num() * sizeof(FColor));
				ExistingTexture->Source.UnlockMip(0);
			}
		#endif
		
		// Update settings
		ExistingTexture->SRGB = true;
		ExistingTexture->CompressionSettings = TC_EditorIcon;
		ExistingTexture->MipGenSettings = TMGS_NoMipmaps;
		ExistingTexture->Filter = TF_Nearest;
		ExistingTexture->LODGroup = TEXTUREGROUP_UI;
		
		// Trigger update
		ExistingTexture->PostEditChange();
		ExistingTexture->UpdateResource();
		// ReSharper disable once CppExpressionWithoutSideEffects
		ExistingTexture->MarkPackageDirty();
		
		// Save the package
		UPackage* Package = ExistingTexture->GetOutermost();
		const FString FilePath = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_None;
		
		if (UPackage::SavePackage(Package, ExistingTexture, *FilePath, SaveArgs))
		{
			ThumbnailData->Temp->Thumbnail = ExistingTexture;
			UE_LOG(LogTemp, Log, TEXT("✅ Updated texture: %s (%dx%d)"), *CleanName, CropWidth, CropHeight);
		}
	}
	else
	{
		// ========== CREATE NEW TEXTURE ==========
		UE_LOG(LogTemp, Log, TEXT("Creating new texture: %s"), *CleanName);
		
		// Create unique asset name (won't conflict)
		FString PackageName;
		FString AssetName;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(AssetPath, TEXT(""), PackageName, AssetName);
		
		// Create package
		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create package"));
			return;
		}
		
		Package->FullyLoad();
		
		// Create texture from pixels
		UTexture2D* NewTexture = NewObject<UTexture2D>(
			Package,
			FName(*AssetName),
			RF_Public | RF_Standalone
		);
		
		if (!NewTexture)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create texture object"));
			return;
		}
		
		#if WITH_EDITORONLY_DATA
			// Initialize source with cropped data
			NewTexture->Source.Init(CropWidth, CropHeight, 1, 1, TSF_BGRA8);
			
			// Copy cropped pixels
		if (uint8* MipData = NewTexture->Source.LockMip(0))
			{
				FMemory::Memcpy(MipData, CroppedPixels.GetData(), CroppedPixels.Num() * sizeof(FColor));
				NewTexture->Source.UnlockMip(0);
			}
		#endif
		
		// Configure texture
		NewTexture->SRGB = true;
		NewTexture->CompressionSettings = TC_EditorIcon;
		NewTexture->MipGenSettings = TMGS_NoMipmaps;
		NewTexture->Filter = TF_Nearest;
		NewTexture->LODGroup = TEXTUREGROUP_UI;
		NewTexture->AddressX = TA_Clamp;
		NewTexture->AddressY = TA_Clamp;
		
		// Update resource
		NewTexture->PostEditChange();
		NewTexture->UpdateResource();
		
		// Notify asset registry
		// ReSharper disable once CppExpressionWithoutSideEffects
		NewTexture->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(NewTexture);
		
		// Save package
		const FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_None;
		
		if (UPackage::SavePackage(Package, NewTexture, *FilePath, SaveArgs))
		{
			ThumbnailData->Temp->Thumbnail = NewTexture;
			UE_LOG(LogTemp, Log, TEXT("✅ Created texture: %s (%dx%d)"), *AssetName, CropWidth, CropHeight);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to save package"));
		}
	}
}

// ==========================================================================
// STHUMBNAILCOPILOT - Implementation
// ==========================================================================

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SThumbnailCopilot::Construct(const FArguments& InArgs)
{
	DesiredPreviewSize = InArgs._DesiredPreviewSize;
	ThumbnailStudio = nullptr;
	ThumbnailData = nullptr;
	AlphaInvertMaterial = nullptr;
	bIsPressed = false;
	bKeyZ_Pressed = false;
	bKeyQ_Pressed = false;
	bKeyS_Pressed = false;
	bKeyD_Pressed = false;
	bKeyA_Pressed = false;
	bKeyE_Pressed = false;
	bAnyKeyPressed = false;
	bHasFocus = false;
	bMoveCameraMode = true;

	RenderBrush.ImageSize = FVector2D(DesiredPreviewSize, DesiredPreviewSize);
	RenderBrush.DrawAs = ESlateBrushDrawType::Image;
	RenderBrush.TintColor = FLinearColor::White;

	ChildSlot
	[
		// WRAPPEZ TOUT DANS UN SBox AVEC OnMouseButtonDown POUR CAPTURER LE FOCUS
		SNew(SBox)
		.WidthOverride(DesiredPreviewSize)
		.HeightOverride(DesiredPreviewSize)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SColorBlock)
				.Color(FLinearColor(0.005, 0.005, 0.005, 1))
			]
			// Render image background
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(DesiredPreviewSize)
				.HeightOverride(DesiredPreviewSize)
				[
					SAssignNew(RenderImage, SImage)
					.Image(&RenderBrush)
				]
			]
            
			// Yellow frame indicator
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride_Lambda([this]()
				{
					if (!ThumbnailData) return DesiredPreviewSize;
					const float MaxDim = FMath::Max(ThumbnailData->Dimensions.X, ThumbnailData->Dimensions.Y);
					return DesiredPreviewSize * ThumbnailData->Dimensions.X / MaxDim;
				})
				.HeightOverride_Lambda([this]()
				{
					if (!ThumbnailData) return DesiredPreviewSize;
					const float MaxDim = FMath::Max(ThumbnailData->Dimensions.X, ThumbnailData->Dimensions.Y);
					return DesiredPreviewSize * ThumbnailData->Dimensions.Y / MaxDim;
				})
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("Border"))
					.BorderBackgroundColor(FLinearColor::Yellow)
					.Padding(0.0f)
				]
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Top)
			.Padding(10)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor_Lambda([this]()
				{
					return bMoveCameraMode ? FLinearColor(0.2f, 0.5f, 1.0f, 0.8f) : FLinearColor(1.0f, 0.5f, 0.2f, 0.8f);
				})
				.Padding(FMargin(10, 5))
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return bMoveCameraMode ? FText::FromString("Camera Mode") : FText::FromString("Object Mode");
					})
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FLinearColor::White)
					.Justification(ETextJustify::Center)
				]
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(10)
			[
				CreateDPIWidget()
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SThumbnailCopilot::Initialize(const TWeakObjectPtr<AThumbnailStudio> InStudio, FThumbnail* InThumbnailData)
{
	ThumbnailStudio = InStudio;
	ThumbnailData = InThumbnailData;

	if (!AlphaInvertMaterial)
	{
		UMaterial* BaseMaterial = NewObject<UMaterial>();
		BaseMaterial->MaterialDomain = MD_UI;
		BaseMaterial->BlendMode = BLEND_Translucent;
		
		UMaterialExpressionTextureSampleParameter2D* TexSample = NewObject<UMaterialExpressionTextureSampleParameter2D>(BaseMaterial);
		TexSample->ParameterName = FName("RenderTexture");
		TexSample->SamplerType = SAMPLERTYPE_Color;
		TexSample->SamplerSource = SSM_FromTextureAsset;
		
		UMaterialExpressionOneMinus* OneMinus = NewObject<UMaterialExpressionOneMinus>(BaseMaterial);
		OneMinus->Input.Expression = TexSample;
		OneMinus->Input.Mask = 1;
		OneMinus->Input.MaskR = 0;
		OneMinus->Input.MaskG = 0;
		OneMinus->Input.MaskB = 0;
		OneMinus->Input.MaskA = 1;
		
		UMaterialExpressionConstant* Gamma = NewObject<UMaterialExpressionConstant>(BaseMaterial);
		Gamma->R = 1.8; // 1/2.2

		UMaterialExpressionPower* ToSRGB = NewObject<UMaterialExpressionPower>(BaseMaterial);
		ToSRGB->Base.Expression = TexSample;
		ToSRGB->Base.Mask = 1;
		ToSRGB->Base.MaskR = 1;
		ToSRGB->Base.MaskG = 1;
		ToSRGB->Base.MaskB = 1;
		ToSRGB->Base.MaskA = 0;
		ToSRGB->Exponent.Expression = Gamma;

		BaseMaterial->GetExpressionCollection().AddExpression(Gamma);
		BaseMaterial->GetExpressionCollection().AddExpression(ToSRGB);
		BaseMaterial->GetExpressionCollection().AddExpression(TexSample);
		BaseMaterial->GetExpressionCollection().AddExpression(OneMinus);
		
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.Expression = ToSRGB;
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.Mask = 1;
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.MaskR = 1;
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.MaskG = 1;
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.MaskB = 1;
		BaseMaterial->GetEditorOnlyData()->EmissiveColor.MaskA = 0;
		
		BaseMaterial->GetEditorOnlyData()->Opacity.Expression = OneMinus;
		BaseMaterial->GetEditorOnlyData()->Opacity.Mask = 1;
		BaseMaterial->GetEditorOnlyData()->Opacity.MaskR = 0;
		BaseMaterial->GetEditorOnlyData()->Opacity.MaskG = 0;
		BaseMaterial->GetEditorOnlyData()->Opacity.MaskB = 0;
		BaseMaterial->GetEditorOnlyData()->Opacity.MaskA = 1;

		
		/*BaseMaterial->GetEditorOnlyData()->Opacity.Expression = OneMinus;*/
		
		BaseMaterial->PreEditChange(nullptr);
		BaseMaterial->PostEditChange();
		
		AlphaInvertMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, nullptr);
	}
	
	if (ThumbnailStudio.IsValid())
	{
		UpdateRenderBrush();
	}
}

void SThumbnailCopilot::ClearAndInvalidate()
{
	ThumbnailStudio = nullptr;
	ThumbnailData = nullptr;

	if (AlphaInvertMaterial)
	{
		AlphaInvertMaterial->ConditionalBeginDestroy();
		AlphaInvertMaterial = nullptr;
	}
	
	RenderBrush.SetResourceObject(nullptr);
	Invalidate(EInvalidateWidget::Layout);
}

void SThumbnailCopilot::UpdateRenderBrush()
{
	if (!ThumbnailStudio.IsValid() || !ThumbnailStudio->GetRenderTarget())
	{
		RenderBrush.SetResourceObject(nullptr);
	}
	if (AlphaInvertMaterial)
	{
		AlphaInvertMaterial->SetTextureParameterValue("RenderTexture", ThumbnailStudio->GetRenderTarget());
		RenderBrush.SetResourceObject(AlphaInvertMaterial);
	}
	else
	{
		RenderBrush.SetResourceObject(ThumbnailStudio->GetRenderTarget());
	}
	Invalidate(EInvalidateWidget::Layout);
}

bool SThumbnailCopilot::SupportsKeyboardFocus() const
{
	return true;
}

FReply SThumbnailCopilot::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (!ThumbnailData) return FReply::Unhandled();

	const FKey Key = InKeyEvent.GetKey();

	bAnyKeyPressed = false;
	
	if (Key == EKeys::SpaceBar)
	{
		bMoveCameraMode = !bMoveCameraMode;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::Z)
	{
		bKeyZ_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::Q)
	{
		bKeyQ_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::S)
	{
		bKeyS_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::D)
	{
		bKeyD_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::A)
	{
		bKeyA_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	if (Key == EKeys::E)
	{
		bKeyE_Pressed = true;
		bAnyKeyPressed = true;
		return FReply::Handled().PreventThrottling();
	}
	

	return FReply::Unhandled();
}

FReply SThumbnailCopilot::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Z)
	{
		bKeyZ_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	if (Key == EKeys::Q)
	{
		bKeyQ_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	if (Key == EKeys::S)
	{
		bKeyS_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	if (Key == EKeys::D)
	{
		bKeyD_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	if (Key == EKeys::A)
	{
		bKeyA_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	if (Key == EKeys::E)
	{
		bKeyE_Pressed = false;
		bAnyKeyPressed = false;
		return FReply::Handled();
	}
	
	bAnyKeyPressed = false;
	return FReply::Unhandled();
}

void SThumbnailCopilot::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bHasFocus = true;
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::Mouse);
	FSlateApplication::Get().SetUserFocus(0, SharedThis(this), EFocusCause::Mouse);
}

void SThumbnailCopilot::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bHasFocus = false;
	if (bAnyKeyPressed)
	{
		bKeyZ_Pressed = false;
		bKeyQ_Pressed = false;
		bKeyS_Pressed = false;
		bKeyD_Pressed = false;
		bKeyA_Pressed = false;
		bKeyE_Pressed = false;
		bAnyKeyPressed = false;
	}
	FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
}

FReply SThumbnailCopilot::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!HasKeyboardFocus())
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::Mouse);
		bHasFocus = true;
	}
	return FReply::Handled();
}

FReply SThumbnailCopilot::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!ThumbnailData || !ThumbnailStudio.IsValid()) return FReply::Unhandled();

	bool bHoldShift = MouseEvent.IsShiftDown();
	const float WheelDelta = MouseEvent.GetWheelDelta();
	const float ZoomSpeed = 20.0f * ThumbnailData->CameraSpeed;
	
	if (bHoldShift)
	{
		ThumbnailData->ObjectScale = FMath::Clamp(
			ThumbnailData->ObjectScale + (WheelDelta * ZoomSpeed * (0.01f / 2)),
			0.f,
			10.f
		);
	}
	else
	{
		// Update distance with clamping
		ThumbnailData->CameraDistance = FMath::Clamp(
			ThumbnailData->CameraDistance - (WheelDelta * ZoomSpeed),
			100.0f,
			800.0f
		);
	}
	
	ApplyCameraChanges();

	return FReply::Handled().PreventThrottling();
}

FReply SThumbnailCopilot::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPressed = true;
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}

	return FReply::Unhandled();
}

FReply SThumbnailCopilot::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsPressed)
	{
		bIsPressed = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SThumbnailCopilot::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	ProcessCameraMovement(InDeltaTime);
}

void SThumbnailCopilot::ProcessCameraMovement(float DeltaTime)
{
	if (!ThumbnailData || !ThumbnailStudio.IsValid()) return;
	
	const float Speed = 50.0f * ThumbnailData->CameraSpeed * DeltaTime;
	bool bChanged = false;
	
	if (bMoveCameraMode)
	{
		// ========== MODE CAMERA ==========
		
		// ZQSD pour orbite
		if (bKeyZ_Pressed)
		{
			ThumbnailData->CameraPitch = FMath::Clamp(ThumbnailData->CameraPitch + Speed, -85.0f, 85.0f);
			bChanged = true;
		}

		if (bKeyS_Pressed)
		{
			ThumbnailData->CameraPitch = FMath::Clamp(ThumbnailData->CameraPitch - Speed, -85.0f, 85.0f);
			bChanged = true;
		}

		if (bKeyQ_Pressed)
		{
			ThumbnailData->CameraYaw += Speed;
			bChanged = true;
		}

		if (bKeyD_Pressed)
		{
			ThumbnailData->CameraYaw -= Speed;
			bChanged = true;
		}
		
		// A/E pour Roll (inclinaison)
		if (bKeyA_Pressed) // Roll gauche
		{
			ThumbnailData->CameraRoll += Speed;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
		if (bKeyE_Pressed) // Roll droite
		{
			ThumbnailData->CameraRoll -= Speed;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
	}
	else
	{
		// ========== MODE OBJECT ==========
		
		// ZQSD pour déplacer l'objet
		if (bKeyZ_Pressed) // Haut
		{
			ThumbnailData->ObjectOffsetY += Speed * 0.5f;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
		if (bKeyS_Pressed) // Bas
		{
			ThumbnailData->ObjectOffsetY -= Speed * 0.5f;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
		if (bKeyQ_Pressed) // Gauche
		{
			ThumbnailData->ObjectOffsetX += Speed * 0.5f;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
		if (bKeyD_Pressed) // Droite
		{
			ThumbnailData->ObjectOffsetX -= Speed * 0.5f;  // NOUVEAU PARAMETRE !
			bChanged = true;
		}
		
		// A/E inutiles en mode Object (ou pour rotation objet si tu veux)
	}

	if (bChanged)
	{
		ApplyCameraChanges();
	}
}

void SThumbnailCopilot::ApplyCameraChanges()
{
	if (ThumbnailStudio.IsValid())
	{
		ThumbnailStudio->Refresh();
		UpdateRenderBrush();
	}
}

TSharedRef<SWidget> SThumbnailCopilot::CreateDPIWidget() const
{
	return 	
	SNew(SBorder)
	.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
	.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.9f))
	.Padding(FMargin(8, 4))
	[
		SNew(SVerticalBox)
		
		// Zoom indicator
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 2)
		[
			SNew(SHorizontalBox)
			
			// Icon Zoom
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			[
				SNew(SImage)
				.Image(GetIcon("Search_Icon"))
				.DesiredSizeOverride(FVector2D(16.f, 16.f))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
			]
			
			// Label Zoom
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Zoom:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
			]
			
			// Value Zoom (multiplicateur)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					if (!ThumbnailData) return FText::FromString("x1.00");
					
					// Convertir distance en multiplicateur (300 = 1.00x)
					const float ZoomMultiplier = 300.0f / ThumbnailData->CameraDistance;
					return FText::FromString(FString::Printf(TEXT("x%.2f"), ZoomMultiplier));
				})
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity_Lambda([this]()
				{
					if (!ThumbnailData) return FLinearColor::White;
					
					const float ZoomMultiplier = 300.0f / ThumbnailData->CameraDistance;
					
					// Couleur selon le zoom
					if (ZoomMultiplier > 1.5f)
						return FLinearColor(1.0f, 1.0f, 0.3f, 1.0f);  // Jaune (zoom in)
					else if (ZoomMultiplier < 0.7f)
						return FLinearColor(0.3f, 0.8f, 1.0f, 1.0f);  // Cyan (zoom out)
					else
						return FLinearColor(0.4f, 1.0f, 0.4f, 1.0f);  // Vert (normal)
				})
			]
		]
		
		// Scale indicator
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 2)
		[
			SNew(SHorizontalBox)
			
			// Icon Scale
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			[
				SNew(SImage)
				.Image(GetIcon("Triangular_Rule_Icon"))
				.DesiredSizeOverride(FVector2D(16.f, 16.f))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
			]
			
			// Label Scale
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 5, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Scale:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
			]
			
			// Value Scale
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					if (!ThumbnailData) return FText::FromString("x1.00");
					return FText::FromString(FString::Printf(TEXT("x%.2f"), ThumbnailData->ObjectScale));
				})
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity_Lambda([this]()
				{
					if (!ThumbnailData) return FLinearColor::White;
					
					// Couleur selon le scale
					if (ThumbnailData->ObjectScale > 2.0f)
						return FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);  // Orange (grand)
					else if (ThumbnailData->ObjectScale < 0.5f)
						return FLinearColor(0.5f, 0.7f, 1.0f, 1.0f);  // Bleu (petit)
					else
						return FLinearColor(0.4f, 1.0f, 0.4f, 1.0f);  // Vert (normal)
				})
			]
		]
	];
}

// ==========================================================================
