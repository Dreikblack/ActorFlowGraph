#include "SActorFlowGraphNode.h"
#include "NodeFactory.h"
#include "SGraphPin.h"
#include "IDocumentation.h"
#include "TutorialMetaData.h"
#include "SLevelOfDetailBranchNode.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "SActorFlowGraphNode"

void SActorFlowGraphNode::Construct(const FArguments& InArgs, UActorFlowEdGraphNode* InNode)
{
	GraphNode = InNode;

	//ActorFlowGraphNode->OnSignalModeChanged.BindRaw(this, &SActorFlowGraphNode::UpdateGraphNode);
	//ActorFlowGraphNode->OnReconstructNodeCompleted.BindRaw(this, &SActorFlowGraphNode::UpdateGraphNode);

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

SActorFlowGraphNode::~SActorFlowGraphNode()
{
}

void SActorFlowGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	SetupErrorReporting();


	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	TSharedRef<SOverlay> DefaultTitleAreaWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.Node.TitleGloss"))
				.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.ColorSpill"))
				.Padding(TitleBorderMargin)
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Top)
								.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
								.AutoWidth()
								[
									CreateIconWidget()
								]
								+ SHorizontalBox::Slot()
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											CreateTitleWidget(NodeTitle)
										]
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											NodeTitle.ToSharedRef()
										]
								]
						]
					+ SHorizontalBox::Slot()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Center)
						.Padding(0, 0, 5, 0)
						.AutoWidth()
						[
							CreateTitleRightWidget()
						]
				]
		]
	+ SOverlay::Slot()
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
				.Visibility(EVisibility::HitTestInvisible)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.TitleHighlight"))
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleIconColor)
				[
					SNew(SSpacer)
						.Size(FVector2D(20, 20))
				]
		];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

	const TSharedRef<SWidget> TitleAreaWidget =
		SNew(SLevelOfDetailBranchNode)
		.UseLowDetailSlot(this, &SActorFlowGraphNode::UseLowDetailNodeTitles)
		.LowDetail()
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.ColorSpill"))
				.Padding(FMargin(75.0f, 22.0f)) 
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
		]
		.HighDetail()
		[
			DefaultTitleAreaWidget
		];

	if (!SWidget::GetToolTip().IsValid())
	{
		TSharedRef<SToolTip> DefaultToolTip = IDocumentation::Get()->CreateToolTip(TAttribute< FText >(this, &SGraphNode::GetNodeTooltip), NULL, GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName());
		SetToolTip(DefaultToolTip);
	}

	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("Graphnode"));
	PopulateMetaTag(&TagMeta);

	//Pin part (Separated by Actor/Component Blocks
	TSharedPtr<SVerticalBox> MainVerticalBox;
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(MainVerticalBox, SVerticalBox)
		];

	MainVerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.Padding(Settings->GetNonPinNodeBodyPadding())
		[
			TitleAreaWidget
		];

	UActorFlowEdGraphNode* Node = CastChecked<UActorFlowEdGraphNode>(GraphNode);


	TMap<FName, TArray<UEdGraphPin*>> pinMap;
	for (UEdGraphPin* pin : Node->Pins)
	{
		pinMap.FindOrAdd(pin->GetPrimaryTerminalType().TerminalSubCategory).Add(pin);
	}
	if (pinMap.Contains(Node->ActorName))
	{
		AddPinGroup(MainVerticalBox, Node->ActorName.ToString(), pinMap[Node->ActorName]);
	}
	for (auto& item : pinMap)
	{
		if (item.Key != Node->ActorName)
		{
			AddPinGroup(MainVerticalBox, item.Key.ToString(), pinMap[item.Key]);
		}
	}

}

void SActorFlowGraphNode::AddPinGroup(TSharedPtr<SVerticalBox> Parent,
	const FString& Title,
	const TArray<UEdGraphPin*>& Pins)
{
	Parent->AddSlot()
		.AutoHeight()
		.Padding(0, 6)
		[
			SNew(STextBlock)
				.Text(FText::FromString(Title))
				.Justification(ETextJustify::Center)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
		];

	Parent->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							BuildPinGroupWidget(Pins)
						]
				]
		];
}

TSharedRef<SWidget> SActorFlowGraphNode::BuildPinGroupWidget(const TArray<UEdGraphPin*>& Pins)
{
	TSharedRef<SVerticalBox> LeftPinsBox = SNew(SVerticalBox);
	TSharedRef<SVerticalBox> RightPinsBox = SNew(SVerticalBox);

	for (UEdGraphPin* Pin : Pins)
	{
		TSharedPtr<SGraphPin> PinWidget = FNodeFactory::CreatePinWidget(Pin);
		AddPin(PinWidget.ToSharedRef());

		if (Pin->Direction == EGPD_Input)
		{
			LeftPinsBox->AddSlot()
				.AutoHeight()
				[
					PinWidget.ToSharedRef()
				];
		}
		else
		{
			RightPinsBox->AddSlot()
				.AutoHeight()
				[
					PinWidget.ToSharedRef()
				];
		}
	}

	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			LeftPinsBox
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1.f) // empty center
		[
			SNew(SSpacer)
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			RightPinsBox
		];
}

void SActorFlowGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		InputPins.Add(PinToAdd);
	}
	else
	{
		OutputPins.Add(PinToAdd);
	}
}

#undef LOCTEXT_NAMESPACE