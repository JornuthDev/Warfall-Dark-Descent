#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SCompoundWidget.h"
#include "IPropertyTypeCustomization.h"

#include "ThumbnailMaker.generated.h"

class UPointLightComponent;
// Forward declarations
class AThumbnailStudio;
class SThumbnailCopilot;

// ==========================================================================
// FTHUMBNAILMATERIALPARAMS - Material parameter names structure
// ==========================================================================

USTRUCT(BlueprintType)
struct FThumbnailMaterialParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IconParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BaseLayerParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UsingLayerParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ColorParameter;

	FThumbnailMaterialParams()
		: IconParameter("Icon")
		, BaseLayerParameter("BaseLayer")
		, UsingLayerParameter("UsingLayer")
		, ColorParameter("Color")
	{}
};

// ==========================================================================
// FTHUMBNAIL - Central thumbnail data structure
// ==========================================================================

USTRUCT(BlueprintType)
struct FThumbnail
{
	GENERATED_BODY()

	// === Textures (Blueprint Visible) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BaseLayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Dimensions;

	// === References (non-serialized, Editor only) ===
	TWeakObjectPtr<AThumbnailStudio> ThumbnailStudio;
	FThumbnail* Temp;
	TSharedPtr<SThumbnailCopilot> ThumbnailCopilot;

	// === Asset Info (Editor only) ===
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	FString AssetName;

	// === Meshes (Editor only) ===
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	USkeletalMesh* SkeletalMesh;

	// === Object Parameters (Editor only) ===
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=0.01, ClampMax=10.0))
	float ObjectScale;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float ObjectOffsetX;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float ObjectOffsetY;

	// === Camera Orbital Parameters (Editor only) ===
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float CameraYaw;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=-85.0, ClampMax=85.0))
	float CameraPitch;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float CameraRoll;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=100.0, ClampMax=800.0))
	float CameraDistance;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=0.1, ClampMax=5.0))
	float CameraSpeed;

	// === Scene Parameters (Editor only) ===
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=0.0, ClampMax=2.0))
	float SceneBrightness;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=0.0, ClampMax=2.0))
	float SceneAttenuation;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true", ClampMin=0.0, ClampMax=2.0))
	FLinearColor SceneColor;

	// === Editor State (non-serialized) ===
	bool bEditorPreviewVisible;

	// === Delegates ===
	DECLARE_DELEGATE_OneParam(FThumbnailDelegate, FName);

	FThumbnailDelegate OnThumbnailOpened;
	FThumbnailDelegate OnThumbnailChanged;

	// === Constructor ===
	FThumbnail()
		: Thumbnail(nullptr)
		, BaseLayer(nullptr)
		, Dimensions(1, 1)
		, Temp(nullptr)
		, AssetName("Default")
		, Mesh(nullptr)
		, SkeletalMesh(nullptr)
		, ObjectScale(1.0f)
		, ObjectOffsetX(0.0f)
		, ObjectOffsetY(0.0f)
		, CameraYaw(0.0f)
		, CameraPitch(0.0f)
		, CameraRoll(0.0f)
		, CameraDistance(300.0f)
		, CameraSpeed(1.0f)
		, SceneBrightness(1.0f)
		, SceneAttenuation(1.0f)
		, SceneColor(FLinearColor::White)
		, bEditorPreviewVisible(false)
	{}

	// === Methods ===

	/** Create a dynamic material instance for rendering with parameters */
	UMaterialInstanceDynamic* ExportMaterial(
		UMaterialInterface* ParentMaterial,
		const FThumbnailMaterialParams& Params,
		const FLinearColor& Color = FLinearColor::Transparent) const;

	/** Synchronize properties from another thumbnail */
	DARKSCRIPT_API void SynchronizeProperties(const FThumbnail* Source);
	
	/** Check if this thumbnail has been modified compared to default */
	bool HasChanged(const bool bIsTransformParams = true) const;
	
	/** Pass all parameters to default value */
	DARKSCRIPT_API void ResetParameters(const bool bIsTransformParams);

	/** Notify that thumbnail was opened */
	void OnThumbnailOpened_Handle() const;

	/** Notify that thumbnail was changed */
	void OnThumbnailChanged_Handle(const FName& Tag) const;

	/** Assignment operator */
	DARKSCRIPT_API FThumbnail& operator=(const FThumbnail& Other);
};

// ==========================================================================
// ATHUMBNAILSTUDIO - Scene capture actor
// ==========================================================================

UCLASS(NotBlueprintable)
class DARKSCRIPT_API AThumbnailStudio : public AEditorUtilityActor
{
	GENERATED_BODY()

public:
	AThumbnailStudio();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// === Public Methods ===

	/** Initialize the studio with thumbnail data */
	void Initialize(FThumbnail* InThumbnailData);

	/** Clear and reset the studio */
	void Clear();

	/** Update the mesh being rendered */
	void UpdateMesh() const;

	/** Update camera transform based on orbital parameters */
	void UpdateCameraTransform() const;

	/** Update object scale */
	void UpdateObjectScale() const;

	/** Update scene lighting parameters */
	void UpdateSceneParameters() const;

	/** Refresh the entire render */
	void Refresh() const;

	/** Create or update the render target */
	UTextureRenderTarget2D* CreateRenderTarget();

	/** Get the current render target */
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	/** Save the render target to disk as texture asset */
	void SaveRenderTargetToDisk() const;

	/** Get reference to thumbnail data */
	FThumbnail* GetThumbnailData() const { return ThumbnailData; }

	/** Get the source render size (always 512x512) */
	static constexpr int32 GetSourceSize() { return 512; }

private:
	// === Components ===

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* MeshObject;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* SkeletalObject;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USceneCaptureComponent2D* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TArray<UPointLightComponent*> StudioLights;
	
	// === Properties ===

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY()
	UMaterialInterface* RenderMaterial;

	/** Reference to thumbnail data being edited */
	FThumbnail* ThumbnailData;

	/** Default camera transform for reset */
	FTransform CameraDefaultTransform;
};

// ==========================================================================
// STHUMBNAILCOPILOT - Slate preview widget
// ==========================================================================

class DARKSCRIPT_API SThumbnailCopilot : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SThumbnailCopilot)
		: _Content()
		, _DesiredPreviewSize(512.0f)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ARGUMENT(float, DesiredPreviewSize)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Initialize with studio and thumbnail data */
	void Initialize(TWeakObjectPtr<AThumbnailStudio> InStudio, FThumbnail* InThumbnailData);

	/** Clear and invalidate the widget */
	void ClearAndInvalidate();

	/** Update the render brush */
	void UpdateRenderBrush();

	// === SWidget overrides ===
	virtual bool SupportsKeyboardFocus() const override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
private:
	/** Process camera movement based on key states */
	void ProcessCameraMovement(float DeltaTime);

	/** Apply camera changes and refresh */
	void ApplyCameraChanges();
	
	TSharedRef<SWidget> CreateDPIWidget() const;

	// === References ===
	TWeakObjectPtr<AThumbnailStudio> ThumbnailStudio;
	FThumbnail* ThumbnailData = nullptr;

	// === UI Elements ===
	TSharedPtr<SImage> RenderImage;
	FSlateBrush RenderBrush;
	UMaterialInstanceDynamic* AlphaInvertMaterial = nullptr;

	// === Input State ===
	bool bIsPressed = false;
	bool bKeyZ_Pressed = false;
	bool bKeyQ_Pressed = false;
	bool bKeyS_Pressed = false;
	bool bKeyD_Pressed = false;
	bool bKeyA_Pressed = false;
	bool bKeyE_Pressed = false;
	bool bAnyKeyPressed = false;
	bool bHasFocus = false;
	
	// === Settings ===
	float DesiredPreviewSize = 0;
	bool bMoveCameraMode = true;
};