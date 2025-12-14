#include "Utils/Libraries/ToolsLibrary.h"
#include "Utils/Tools/ThumbnailMaker.h"

// ═══════════════════════════════════════════════════════════════════════════════
// ═══ FUNCTIONS (BLUEPRINT API) ═════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════════

UMaterialInstanceDynamic* UToolsLibrary::ExportMaterial(const FThumbnail Thumbnail, UMaterialInterface* Material,
                                                        const FThumbnailMaterialParams& Params, const FLinearColor& Color)
{
	return Thumbnail.ExportMaterial(Material, Params, Color);
}
