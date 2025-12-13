#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Views/SListView.h"

class UClass;

enum class EPickerRowType
{
	Category,
	Function
};

struct FFunctionPickerItem
{
	EPickerRowType RowType;
	FName Name;
	FString Signature;
};

DECLARE_DELEGATE_OneParam(FOnFunctionPicked, FName);

class SFunctionPickerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFunctionPickerWidget)
		{
		}
		SLATE_EVENT(FSimpleDelegate, OnClose)
		SLATE_EVENT(FOnFunctionPicked, OnFunctionPicked)
		SLATE_ARGUMENT(UClass*, TargetClass)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FFunctionPickerItem>> AllFunctions;

	TArray<TSharedPtr<FFunctionPickerItem>> FilteredFunctions;

	TSharedPtr<SListView<TSharedPtr<FFunctionPickerItem>>> ListViewWidget;

	FOnFunctionPicked OnFunctionPicked;

	TArray<FString> ExcludePrefixes = { TEXT("K2_"), TEXT("GET"), TEXT("Has"), TEXT("Is") };

	void OnSearchChanged(const FText& Text);

	/** List Widget Item */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FFunctionPickerItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const;

	void BuildFunctionList(UClass* TargetClass);

	static FString BuildFunctionSignature(UFunction* Func);
};
