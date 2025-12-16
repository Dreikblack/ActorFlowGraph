#include "ActorFlowEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

TSharedPtr<FSlateStyleSet> FActorFlowEditorStyle::StyleSet = nullptr;

FName FActorFlowEditorStyle::GetStyleSetName()
{
	static FName ActorFlowEditorStyle(TEXT("ActorFlowEditorStyle"));
	return ActorFlowEditorStyle;
}

void FActorFlowEditorStyle::Initialize()
{
	StyleSet = MakeShareable(new FSlateStyleSet(TEXT("ActorFlowEditorStyle")));

	const FVector2D Icon16(16.0f, 16.0f);
	const FVector2D Icon64(64.0f, 64.0f);

	// Actor Flow assets
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("ActorFlowGraph"))->GetBaseDir() / TEXT("Resources"));

	StyleSet->Set("ClassIcon.ActorFlowGraphAsset", new IMAGE_BRUSH(TEXT("Icons/ActorFlowAsset_16x"), Icon16));
	StyleSet->Set("ClassThumbnail.ActorFlowGraphAsset", new IMAGE_BRUSH(TEXT("Icons/ActorFlowAsset_64x"), Icon64));

	StyleSet->Set("ClassIcon.ActorFlowGraphRuntime", new IMAGE_BRUSH(TEXT("Icons/ActorFlowAsset_Runtime_16x"), Icon16));
	StyleSet->Set("ClassThumbnail.ActorFlowGraphRuntime", new IMAGE_BRUSH(TEXT("Icons/ActorFlowAsset_Runtime_64x"), Icon64));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

void FActorFlowEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

#undef BORDER_BRUSH
#undef BOX_BRUSH
#undef IMAGE_BRUSH
#undef IMAGE_BRUSH_SVG
