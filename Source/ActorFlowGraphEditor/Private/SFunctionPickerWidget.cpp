#include "SFunctionPickerWidget.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"

void SFunctionPickerWidget::Construct(const FArguments& InArgs)
{
	OnFunctionPicked = InArgs._OnFunctionPicked;

	BuildFunctionList(InArgs._TargetClass);
	FilteredFunctions = AllFunctions;

	ChildSlot
		[
			SNew(SVerticalBox)

				// Search
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4)
				[
					SNew(SSearchBox)
						.OnTextChanged(this, &SFunctionPickerWidget::OnSearchChanged)
				]

				// List
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SAssignNew(ListViewWidget, SListView<TSharedPtr<FFunctionPickerItem>>)
						.ListItemsSource(&FilteredFunctions)
						.OnGenerateRow(this, &SFunctionPickerWidget::OnGenerateRow)
						.SelectionMode(ESelectionMode::Single)
						.OnMouseButtonClick_Lambda([this](TSharedPtr<FFunctionPickerItem> Item)
							{
								if (OnFunctionPicked.IsBound() && Item.IsValid() && Item->RowType == EPickerRowType::Function)
								{
									OnFunctionPicked.Execute(Item->Name);
								}
							})
				]
		];
}

void SFunctionPickerWidget::BuildFunctionList(UClass* TargetClass)
{
	if (!TargetClass)
	{
		return;
	}

	AllFunctions.Empty();

	TMap<FName, TArray<UFunction*>> ByCategory;

	for (TFieldIterator<UFunction> It(TargetClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		UFunction* Func = *It;

		if (Func->HasAnyFunctionFlags(FUNC_BlueprintCallable) && Func->HasAnyFunctionFlags(FUNC_Public))
		{
			FString FuncName = Func->GetName();
			bool bDoSkipFunction = false;
			for (FString Prefix : ExcludePrefixes)
			{
				if (FuncName.StartsWith(Prefix)) 
				{
					bDoSkipFunction = true;
					break;
				}
			}
			if (bDoSkipFunction)
			{
				continue;
			}
			FName Category = NAME_None;
			if (Func->HasMetaData(TEXT("Category")))
			{
				Category = FName(*Func->GetMetaData(TEXT("Category")));
			}
			ByCategory.FindOrAdd(Category).Add(Func);
		}

	}

	for (auto& Pair : ByCategory)
	{
		// Category row
		AllFunctions.Add(MakeShared<FFunctionPickerItem>(FFunctionPickerItem{
			EPickerRowType::Category,
			Pair.Key
			}));

		// Function rows
		for (UFunction* Func : Pair.Value)
		{
			AllFunctions.Add(MakeShared<FFunctionPickerItem>(FFunctionPickerItem{
				EPickerRowType::Function,
				Func->GetFName(),
				BuildFunctionSignature(Func)
				}));
		}
	}

	if (ListViewWidget.IsValid())
	{
		ListViewWidget->RequestListRefresh();
	}
}

FString SFunctionPickerWidget::BuildFunctionSignature(UFunction* Func)
{
	FString ReturnType = TEXT("void");

	if (FProperty* Ret = Func->GetReturnProperty())
	{
		ReturnType = Ret->GetCPPType();
	}

	TArray<FString> Params;

	for (TFieldIterator<FProperty> It(Func); It; ++It)
	{
		FProperty* Prop = *It;

		if (Prop->HasAnyPropertyFlags(CPF_Parm) &&
			!Prop->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			Params.Add(
				FString::Printf(TEXT("%s %s"),
					*Prop->GetCPPType(),
					*Prop->GetName())
			);
		}
	}

	return FString::Printf(
		TEXT("%s %s(%s)"),
		*ReturnType,
		*Func->GetName(),
		*FString::Join(Params, TEXT(", "))
	);
}

void SFunctionPickerWidget::OnSearchChanged(const FText& Text)
{
	const FString Query = Text.ToString();

	FilteredFunctions.Empty();

	for (const TSharedPtr<FFunctionPickerItem>& PickedFunction : AllFunctions)
	{
		if (Query.IsEmpty() || PickedFunction->Name.ToString().Contains(Query))
		{
			FilteredFunctions.Add(PickedFunction);
		}
	}

	ListViewWidget->RequestListRefresh();
}

TSharedRef<ITableRow> SFunctionPickerWidget::OnGenerateRow(TSharedPtr<FFunctionPickerItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
	// --- CATEGORY ROW ---
	if (Item->RowType == EPickerRowType::Category)
	{
		return SNew(STableRow<TSharedPtr<FFunctionPickerItem>>, OwnerTable)
			[
				SNew(SBorder)
					.Padding(FMargin(6.f, 4.f))
					.BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
					[
						SNew(STextBlock)
							.Text(FText::FromName(Item->Name))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					]
			];
	}

	// --- FUNCTION ROW ---
	return SNew(STableRow<TSharedPtr<FFunctionPickerItem>>, OwnerTable)
		[
			SNew(SHorizontalBox)

				// Icon
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(2.f)
				[
					SNew(SImage)
						.Image(FAppStyle::GetBrush("GraphEditor.Function_16x"))
				]

				// Signature
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				.Padding(4.f, 2.f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Item->Signature))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
		];
}
