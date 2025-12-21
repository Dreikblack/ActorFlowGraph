#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Views/SListView.h"
#include "SFunctionPickerWidget.generated.h"

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

UCLASS(Config = EditorPerProjectUserSettings)
class UFunctionPickerPrefixFilterConfig : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Function Prefix Filter")
	TArray<FString> ExcludedPrefixes;

	UFunctionPickerPrefixFilterConfig()
	{
		ExcludedPrefixes = { TEXT("K2_"), TEXT("Get"), TEXT("Has"), TEXT("Is") };
	}
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

	TArray<TSharedPtr<FFunctionPickerItem>> AfterPrefixFunctions;

	TArray<TSharedPtr<FFunctionPickerItem>> FilteredFunctions;

	TSharedPtr<SListView<TSharedPtr<FFunctionPickerItem>>> ListViewWidget;

	FOnFunctionPicked OnFunctionPicked;

	UFunctionPickerPrefixFilterConfig* PrefixFilterConfig = nullptr;

	TSharedPtr<ISinglePropertyView> PrefixArrayWidget;

	TSharedPtr<IDetailsView> DetailsView;

	TSharedPtr<SEditableTextBox> CustomPinTextBox;

	FText SearchText;

	void OnSearchChanged(const FText& Text);

	void UpdatePrefixFilter();

	void UpdateCategoryFilter();

	/** List Widget Item */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FFunctionPickerItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const;

	void BuildFunctionList(UClass* TargetClass);

	static FString BuildFunctionSignature(UFunction* Func);

	FReply OnCustomPinButtonClicked();
};
