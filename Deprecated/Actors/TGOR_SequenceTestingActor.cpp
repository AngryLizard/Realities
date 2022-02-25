// The Gateway of Realities: Planes of Existence.

#include "TGOR_SequenceTestingActor.h"
#include "AnimationEditorUtils.h"

#include "LevelSequenceActor.h"
#include "Factories/AnimSequenceFactory.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#include "Misc/MessageDialog.h"
#include "MovieSceneToolHelpers.h"
#include "Toolkits/AssetEditorManager.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AnimSequenceLevelSequenceLink.h"
#include "LevelSequenceAnimSequenceLink.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

#include "LevelSequenceEditor/Private/LevelSequenceEditorToolkit.h"
#include "MovieSceneTools/Private/TrackEditors/SkeletalAnimationTrackEditor.h"

#include "Tracks/MovieSceneSkeletalAnimationTrack.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "TGOR_SequenceTestingActor"

ATGOR_SequenceTestingActor::ATGOR_SequenceTestingActor(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer)
{
	SourceActor = nullptr;
	TargetSkeleton = nullptr;
	SequenceActor = nullptr;

	AnimSeqExportOption = ObjectInitializer.CreateDefaultSubobject<UAnimSeqExportOption>(this, "Options");
}





class SAnimSequenceOptionsWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnimSequenceOptionsWindow)
		: _ExportOptions(nullptr)
		, _WidgetWindow()
		, _FullPath()
	{}

	SLATE_ARGUMENT(UAnimSeqExportOption*, ExportOptions)
		SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_ARGUMENT(FText, FullPath)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnExport()
	{
		bShouldExport = true;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}


	FReply OnCancel()
	{
		bShouldExport = false;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			return OnCancel();
		}

		return FReply::Unhandled();
	}

	bool ShouldExport() const
	{
		return bShouldExport;
	}


	SAnimSequenceOptionsWindow()
		: ExportOptions(nullptr)
		, bShouldExport(false)
	{}

private:

	FReply OnResetToDefaultClick() const;

private:
	UAnimSeqExportOption* ExportOptions;
	TSharedPtr<class IDetailsView> DetailsView;
	TWeakPtr< SWindow > WidgetWindow;
	bool			bShouldExport;
};


void SAnimSequenceOptionsWindow::Construct(const FArguments& InArgs)
{
	ExportOptions = InArgs._ExportOptions;
	WidgetWindow = InArgs._WidgetWindow;

	check(ExportOptions);

	FText CancelText = LOCTEXT("AnimSequenceOptions_Cancel", "Cancel");
	FText CancelTooltipText = LOCTEXT("AnimSequenceOptions_Cancel_ToolTip", "Cancel the current Anim Sequence Creation.");

	TSharedPtr<SBox> HeaderToolBox;
	TSharedPtr<SHorizontalBox> AnimHeaderButtons;
	TSharedPtr<SBox> InspectorBox;
	this->ChildSlot
		[
			SNew(SBox)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2)
		[
			SAssignNew(HeaderToolBox, SBox)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2)
		[
			SNew(SBorder)
			.Padding(FMargin(3))
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
		.Text(LOCTEXT("Export_CurrentFileTitle", "Current File: "))
		]
	+ SHorizontalBox::Slot()
		.Padding(5, 0, 0, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
		.Text(InArgs._FullPath)
		]
		]
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(2)
		[
			SAssignNew(InspectorBox, SBox)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Right)
		.Padding(2)
		[
			SNew(SUniformGridPanel)
			.SlotPadding(2)
		+ SUniformGridPanel::Slot(1, 0)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
		.Text(LOCTEXT("AnimExportOptionsWindow_Export", "Export To Animation Sequence"))
		.OnClicked(this, &SAnimSequenceOptionsWindow::OnExport)
		]
	+ SUniformGridPanel::Slot(2, 0)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
		.Text(CancelText)
		.ToolTipText(CancelTooltipText)
		.OnClicked(this, &SAnimSequenceOptionsWindow::OnCancel)
		]
		]
			]
		];

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	InspectorBox->SetContent(DetailsView->AsShared());

	HeaderToolBox->SetContent(
		SNew(SBorder)
		.Padding(FMargin(3))
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Right)
		[
			SAssignNew(AnimHeaderButtons, SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(2.0f, 0.0f))
		[
			SNew(SButton)
			.Text(LOCTEXT("AnimSequenceOptions_ResetOptions", "Reset to Default"))
		.OnClicked(this, &SAnimSequenceOptionsWindow::OnResetToDefaultClick)
		]
		]
		]
		]
	);

	DetailsView->SetObject(ExportOptions);
}

FReply SAnimSequenceOptionsWindow::OnResetToDefaultClick() const
{
	ExportOptions->ResetToDefault();
	//Refresh the view to make sure the custom UI are updating correctly
	DetailsView->SetObject(ExportOptions, true);
	return FReply::Handled();
}


bool ATGOR_SequenceTestingActor::CreateAnimationSequence(const TArray<UObject*> NewAssets, USkeletalMeshComponent* SkelMeshComp, FGuid Binding, bool bCreateSoftLink)
{
	bool bResult = false;
	if (NewAssets.Num() > 0)
	{
		UAnimSequence* AnimSequence = Cast<UAnimSequence>(NewAssets[0]);
		if (AnimSequence)
		{
			UObject* NewAsset = NewAssets[0];
			TSharedPtr<SWindow> ParentWindow;
			if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
			{
				IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
				ParentWindow = MainFrame.GetParentWindow();
			}

			TSharedRef<SWindow> Window = SNew(SWindow)
				.Title(NSLOCTEXT("UnrealEd", "AnimSeqOpionsTitle", "Animation Sequence Options"))
				.SizingRule(ESizingRule::UserSized)
				.AutoCenter(EAutoCenter::PrimaryWorkArea)
				.ClientSize(FVector2D(500, 445));

			TSharedPtr<SAnimSequenceOptionsWindow> OptionWindow;
			Window->SetContent
			(
				SAssignNew(OptionWindow, SAnimSequenceOptionsWindow)
				.ExportOptions(AnimSeqExportOption)
				.WidgetWindow(Window)
				.FullPath(FText::FromString(NewAssets[0]->GetName()))
			);

			FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);

			const TSharedPtr<ISequencer> ParentSequencer = GetSequencer();
			UMovieScene* MovieScene = ParentSequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
			FMovieSceneSequenceIDRef Template = ParentSequencer->GetFocusedTemplateID();
			FMovieSceneSequenceTransform RootToLocalTransform;
			bResult = MovieSceneToolHelpers::ExportToAnimSequence(AnimSequence, AnimSeqExportOption, MovieScene, ParentSequencer.Get(), SkelMeshComp, Template, RootToLocalTransform);
		}

		if (bResult && bCreateSoftLink)
		{
			TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
			ULevelSequence* LevelSequence = Cast<ULevelSequence>(SequencerPtr->GetFocusedMovieSceneSequence());
			if (LevelSequence && LevelSequence->GetClass()->ImplementsInterface(UInterface_AssetUserData::StaticClass())
				&& AnimSequence->GetClass()->ImplementsInterface(UInterface_AssetUserData::StaticClass()))
			{
				if (IInterface_AssetUserData* AnimAssetUserData = Cast< IInterface_AssetUserData >(AnimSequence))
				{
					UAnimSequenceLevelSequenceLink* AnimLevelLink = AnimAssetUserData->GetAssetUserData< UAnimSequenceLevelSequenceLink >();
					if (!AnimLevelLink)
					{
						AnimLevelLink = NewObject<UAnimSequenceLevelSequenceLink>(AnimSequence, NAME_None, RF_Public | RF_Transactional);
						AnimAssetUserData->AddAssetUserData(AnimLevelLink);
					}

					AnimLevelLink->SetLevelSequence(LevelSequence);
					AnimLevelLink->SkelTrackGuid = Binding;
				}
				if (IInterface_AssetUserData* AssetUserDataInterface = Cast< IInterface_AssetUserData >(LevelSequence))
				{
					bool bAddItem = true;
					ULevelSequenceAnimSequenceLink* LevelAnimLink = AssetUserDataInterface->GetAssetUserData< ULevelSequenceAnimSequenceLink >();
					if (LevelAnimLink)
					{
						for (FLevelSequenceAnimSequenceLinkItem& LevelAnimLinkItem : LevelAnimLink->AnimSequenceLinks)
						{
							if (LevelAnimLinkItem.SkelTrackGuid == Binding)
							{
								bAddItem = false;
								UAnimSequence* OtherAnimSequence = LevelAnimLinkItem.ResolveAnimSequence();

								if (OtherAnimSequence != AnimSequence)
								{
									if (IInterface_AssetUserData* OtherAnimAssetUserData = Cast< IInterface_AssetUserData >(OtherAnimSequence))
									{
										UAnimSequenceLevelSequenceLink* OtherAnimLevelLink = OtherAnimAssetUserData->GetAssetUserData< UAnimSequenceLevelSequenceLink >();
										if (OtherAnimLevelLink)
										{
											OtherAnimAssetUserData->RemoveUserDataOfClass(UAnimSequenceLevelSequenceLink::StaticClass());
										}
									}
								}
								LevelAnimLinkItem.PathToAnimSequence = FSoftObjectPath(AnimSequence);
								LevelAnimLinkItem.bExportCurves = AnimSeqExportOption->bExportCurves;
								LevelAnimLinkItem.bExportTransforms = AnimSeqExportOption->bExportTransforms;
								LevelAnimLinkItem.bRecordInWorldSpace = AnimSeqExportOption->bRecordInWorldSpace;

								break;
							}
						}
					}
					else
					{
						LevelAnimLink = NewObject<ULevelSequenceAnimSequenceLink>(LevelSequence, NAME_None, RF_Public | RF_Transactional);

					}
					if (bAddItem == true)
					{
						FLevelSequenceAnimSequenceLinkItem LevelAnimLinkItem;
						LevelAnimLinkItem.SkelTrackGuid = Binding;
						LevelAnimLinkItem.PathToAnimSequence = FSoftObjectPath(AnimSequence);
						LevelAnimLinkItem.bExportCurves = AnimSeqExportOption->bExportCurves;
						LevelAnimLinkItem.bExportTransforms = AnimSeqExportOption->bExportTransforms;
						LevelAnimLinkItem.bRecordInWorldSpace = AnimSeqExportOption->bRecordInWorldSpace;

						LevelAnimLink->AnimSequenceLinks.Add(LevelAnimLinkItem);
						AssetUserDataInterface->AddAssetUserData(LevelAnimLink);
					}
				}
			}
		}
		// if it contains error, warn them
		if (bResult)
		{
			FText NotificationText;
			if (NewAssets.Num() == 1)
			{
				NotificationText = FText::Format(LOCTEXT("NumAnimSequenceAssetsCreated", "{0} Anim Sequence  assets created."), NewAssets.Num());
			}
			else
			{
				NotificationText = FText::Format(LOCTEXT("AnimSequenceAssetsCreated", "Anim Sequence asset created: '{0}'."), FText::FromString(NewAssets[0]->GetName()));
			}

			FNotificationInfo Info(NotificationText);
			Info.ExpireDuration = 8.0f;
			Info.bUseLargeFont = false;
			Info.Hyperlink = FSimpleDelegate::CreateLambda([NewAssets]()
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAssets(NewAssets);
			});
			Info.HyperlinkText = FText::Format(LOCTEXT("OpenNewPoseAssetHyperlink", "Open {0}"), FText::FromString(NewAssets[0]->GetName()));

			TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
			if (Notification.IsValid())
			{
				Notification->SetCompletionState(SNotificationItem::CS_Success);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToCreateAsset", "Failed to create asset"));
		}
	}
	return bResult;
}

void ATGOR_SequenceTestingActor::TestSequenceBakeToSkeleton()
{
	if (IsValid(SourceActor) && IsValid(TargetSkeleton) && IsValid(SequenceActor))
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		USkeletalMeshComponent* SkelMeshComp = SourceActor->FindComponentByClass<USkeletalMeshComponent>();
		ULevelSequence* Sequence = SequenceActor->GetSequence();
		if (IsValid(SkelMeshComp) && IsValid(Sequence) && IsValid(AssetEditorSubsystem))
		{
			UMovieScene* Scene = Sequence->GetMovieScene();
			const TArray<FMovieSceneBinding>& Bindings = Scene->GetBindings();
			if (Bindings.Num() > 0)
			{
				IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(Sequence, true);
				FLevelSequenceEditorToolkit* LevelSequenceEditor = (FLevelSequenceEditorToolkit*)AssetEditor;
				if (LevelSequenceEditor != nullptr)
				{
					// Get current Sequencer
					Sequencer = TWeakPtr<ISequencer>(LevelSequenceEditor->GetSequencer());


					TArray<TWeakObjectPtr<UObject>> Skels;
					Skels.Add(TargetSkeleton);

					UE_LOG(LogTemp, Warning, TEXT("Sequence bake bro"));
					AnimationEditorUtils::ExecuteNewAnimAsset<UAnimSequenceFactory, UAnimSequence>(Skels, FString("_Sequence"), FAnimAssetCreated::CreateUObject(this, &ATGOR_SequenceTestingActor::CreateAnimationSequence, SkelMeshComp, Bindings[0].GetObjectGuid(), false), false);
				}
			}
		}
	}
}

const TSharedPtr<ISequencer> ATGOR_SequenceTestingActor::GetSequencer() const
{
	return Sequencer.Pin();
}


#undef LOCTEXT_NAMESPACE
