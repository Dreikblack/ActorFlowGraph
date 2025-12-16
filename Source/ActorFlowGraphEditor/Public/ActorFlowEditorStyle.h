#pragma once

#include "CoreMinimal.h"

class ACTORFLOWGRAPHEDITOR_API FActorFlowEditorStyle
{
public:
	static TSharedPtr<ISlateStyle> Get()
	{
		return StyleSet;
	}
	static FName GetStyleSetName();

	static void Initialize();
	static void Shutdown();

	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get()->GetBrush(PropertyName, Specifier);
	}

private:
	static TSharedPtr<FSlateStyleSet> StyleSet;
};
