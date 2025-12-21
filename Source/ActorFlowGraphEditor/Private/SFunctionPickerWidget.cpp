#include "SFunctionPickerWidget.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "ISinglePropertyView.h"

void SFunctionPickerWidget::Construct(const FArguments& InArgs)
{
	OnFunctionPicked = InArgs._OnFunctionPicked;

	PrefixFilterConfig = GetMutableDefault<UFunctionPickerPrefixFilterConfig>();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch = false;
	Args.bShowScrollBar = false;
	Args.bLockable = false;
	Args.bUpdatesFromSelection = false;

	DetailsView = PropertyModule.CreateDetailView(Args);
	DetailsView->SetObject(PrefixFilterConfig);

	DetailsView->OnFinishedChangingProperties().AddLambda(
		[this](const FPropertyChangedEvent&)
		{
			PrefixFilterConfig->SaveConfig();
			UpdatePrefixFilter();
			OnSearchChanged(SearchText);
		}
	);

	BuildFunctionList(InArgs._TargetClass);
	UpdatePrefixFilter();
	FilteredFunctions = AfterPrefixFunctions;
	UpdateCategoryFilter();

	ChildSlot
		[
			SNew(SVerticalBox)

				// Custom Pin
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4)
						[
							SAssignNew(CustomPinTextBox, SEditableTextBox)
								.HintText(FText::FromString(TEXT("Enter Custom Pin Name")))
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.0f)
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Make Custom Pin")))
								.OnClicked(this, &SFunctionPickerWidget::OnCustomPinButtonClicked)
						]
				]

			    //Filter array
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
						[
							DetailsView.ToSharedRef()
						]
				]

				// Search
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4)
				[
					SNew(SSearchBox)
						.OnTextChanged(this, &SFunctionPickerWidget::OnSearchChanged)
				]

				// Function List
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

		if (Func->HasAnyFunctionFlags(FUNC_BlueprintCallable))
		{
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

FReply SFunctionPickerWidget::OnCustomPinButtonClicked()
{
	if (CustomPinTextBox.IsValid())
	{
		FText EnteredText = CustomPinTextBox->GetText();
		if (OnFunctionPicked.IsBound())
		{
			OnFunctionPicked.Execute(FName(*EnteredText.ToString()));
		}
	}

	return FReply::Handled();
}

void SFunctionPickerWidget::OnSearchChanged(const FText& Text)
{
	SearchText = Text;

	if (Text.IsEmpty())
	{
		FilteredFunctions = AfterPrefixFunctions;
		UpdateCategoryFilter();
		return;
	}
 
	const FString Query = Text.ToString();

	FilteredFunctions.Empty();

	for (const TSharedPtr<FFunctionPickerItem>& PickedFunction : AfterPrefixFunctions)
	{
		if (PickedFunction->RowType == EPickerRowType::Category || PickedFunction->Name.ToString().Contains(Query))
		{
			FilteredFunctions.Add(PickedFunction);
		}
	}

	UpdateCategoryFilter();

	ListViewWidget->RequestListRefresh();
}

void SFunctionPickerWidget::UpdatePrefixFilter()
{
	AfterPrefixFunctions.Reset();

	for (const TSharedPtr<FFunctionPickerItem>& Item : AllFunctions)
	{
		if (Item->RowType == EPickerRowType::Category)
		{
			AfterPrefixFunctions.Add(Item);
			continue;
		}
		bool bDoSkipFunction = false;
		for (FString Prefix : PrefixFilterConfig->ExcludedPrefixes)
		{
			FString FunName = Item->Name.ToString();
			if (FunName.StartsWith(Prefix))
			{
				if (FunName.IsValidIndex(Prefix.Len()))
				{
					TCHAR Character = FunName[Prefix.Len()];
					if (FChar::IsUpper(Character))
					{
						bDoSkipFunction = true;
						break;
					}
				}
			}
		}
		if (!bDoSkipFunction)
		{
			AfterPrefixFunctions.Add(Item);
		}
	}

}

void SFunctionPickerWidget::UpdateCategoryFilter()
{
	EPickerRowType PrevRowType = EPickerRowType::Function;
	for (int32 Index = FilteredFunctions.Num() - 1; Index >= 0; --Index)
	{
		if (PrevRowType == EPickerRowType::Category && FilteredFunctions[Index]->RowType == EPickerRowType::Category)
		{
			PrevRowType = EPickerRowType::Category;
			FilteredFunctions.RemoveAt(Index);
			continue;
		}
		PrevRowType = FilteredFunctions[Index]->RowType;
	}
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
