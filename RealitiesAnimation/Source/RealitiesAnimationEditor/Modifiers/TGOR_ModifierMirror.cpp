// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ModifierMirror.h"
#include "ISequencer.h"
#include "LevelSequence.h"
#include "MovieSceneSequence.h"
#include "ILevelSequenceEditorToolkit.h"
#include "../RealitiesAnimationEditor.h"
#include "Tools/ControlRigSnapper.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Sequencer/MovieSceneControlRigParameterTrack.h"
#include "Sequencer/MovieSceneControlRigParameterSection.h"
#include "SequencerKeyCollection.h"
#include "IKeyArea.h"

TWeakPtr<ISequencer> FTGOR_ModifierMirror::GetSequencer() const
{
	TWeakPtr<ISequencer> WeakSequencer = nullptr;

	FRealitiesAnimationEditor& RealitiesAnimationEditor = FModuleManager::LoadModuleChecked<FRealitiesAnimationEditor>("RealitiesAnimationEditor");
	TWeakPtr<ISequencer> CurrentSequencer = RealitiesAnimationEditor.GetSequencer();
	if (CurrentSequencer.IsValid())
	{
		ULevelSequence* LevelSequence = Cast<ULevelSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
		if (LevelSequence)
		{
			IAssetEditorInstance* AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(LevelSequence, false);
			ILevelSequenceEditorToolkit* LevelSequenceEditor = static_cast<ILevelSequenceEditorToolkit*>(AssetEditor);
			WeakSequencer = LevelSequenceEditor ? LevelSequenceEditor->GetSequencer() : nullptr;
		}
	}

	return WeakSequencer;
}

void FTGOR_ModifierMirror::Mirror()
{
	TWeakPtr<ISequencer> Sequencer = GetSequencer();
	if (!Sequencer.IsValid() || !Sequencer.Pin()->GetFocusedMovieSceneSequence())
	{
		return;
	}

	TArray<const IKeyArea*> OutSelectedKeyAreas;
	Sequencer.Pin()->GetSelectedKeyAreas(OutSelectedKeyAreas);

	TArray<FMovieSceneChannel*> Channels;
	for (const IKeyArea* Area : OutSelectedKeyAreas)
	{
		const FString Name = Area->GetName().ToString();
		if (Name.EndsWith("Location.X") || Name.EndsWith("Rotation.Y") || Name.EndsWith("Rotation.Z"))
		{
			FMovieSceneFloatChannel* Channel = Area->GetChannel().Cast<FMovieSceneFloatChannel>().Get();
			for (FMovieSceneFloatValue& floatValue : Channel->GetData().GetValues())
			{
				floatValue.Value *= -1;
				floatValue.Tangent.ArriveTangent *= -1;
				floatValue.Tangent.LeaveTangent *= -1;
			}
		}
	}

	Sequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::RefreshAllImmediately);
}

#pragma optimize( "", off )
void FTGOR_ModifierMirror::Loop()
{
	TWeakPtr<ISequencer> Sequencer = GetSequencer();
	if (!Sequencer.IsValid() || !Sequencer.Pin()->GetFocusedMovieSceneSequence())
	{
		return;
	}

	UMovieScene* MovieScene = Sequencer.Pin()->GetFocusedMovieSceneSequence()->GetMovieScene();

	// Make sure range is inclusive
	TRange<FFrameNumber> PlaybackRange = MovieScene->GetPlaybackRange();
	PlaybackRange = TRange<FFrameNumber>(
		TRange<FFrameNumber>::BoundsType::Inclusive(PlaybackRange.GetLowerBoundValue()),
		TRange<FFrameNumber>::BoundsType::Exclusive(PlaybackRange.GetUpperBoundValue()));

	const FFrameRate FrameRate = MovieScene->GetTickResolution() / MovieScene->GetDisplayRate();
	const FFrameNumber PlaybackLength = PlaybackRange.Size<FFrameNumber>() + FrameRate.AsFrameNumber(1.0);
	//const FFrameNumber PlaybackLength = PlaybackRange.Size<FFrameNumber>() + FrameRate.AsFrameNumber(1.0);

	TArray<const IKeyArea*> OutSelectedKeyAreas;
	Sequencer.Pin()->GetSelectedKeyAreas(OutSelectedKeyAreas);

	TMap<FMovieSceneChannel*, TArray<FKeyHandle>> DeleteKeys;

	for (const IKeyArea* KeyArea : OutSelectedKeyAreas)
	{
		FMovieSceneChannel* Channel = KeyArea->ResolveChannel();

		TArray<FFrameNumber> OutKeyTimes;
		KeyArea->GetKeyTimes(OutKeyTimes);

		TArray<FKeyHandle> OutKeyHandles;
		KeyArea->GetKeyHandles(OutKeyHandles);

		// Delete keys outside the range without adding new ones (DeleteFrom adds border keys)
		const int32 Num = OutKeyTimes.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			const FFrameNumber& Time = OutKeyTimes[Index];
			const FKeyHandle& Handle = OutKeyHandles[Index];
			if (!PlaybackRange.Contains(Time))
			{
				DeleteKeys.FindOrAdd(Channel).Emplace(Handle);
			}
			else
			{
				KeyArea->SetKeyTime(KeyArea->DuplicateKey(Handle), Time - PlaybackLength);
				KeyArea->SetKeyTime(KeyArea->DuplicateKey(Handle), Time + PlaybackLength);
			}
		}
	}

	for (const auto& DeletePair : DeleteKeys)
	{
		DeletePair.Key->DeleteKeys(DeletePair.Value);
	}

	Sequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::RefreshAllImmediately);
}

void FTGOR_ModifierMirror::TimeOffsetHalf()
{
	TWeakPtr<ISequencer> Sequencer = GetSequencer();
	if (!Sequencer.IsValid() || !Sequencer.Pin()->GetFocusedMovieSceneSequence())
	{
		return;
	}

	// Loop to make sure we can offset without losing data
	Loop();

	UMovieScene* MovieScene = Sequencer.Pin()->GetFocusedMovieSceneSequence()->GetMovieScene();
	const FFrameNumber OffsetLength = MovieScene->GetPlaybackRange().Size<FFrameNumber>() / 2;

	TArray<const IKeyArea*> OutSelectedKeyAreas;
	Sequencer.Pin()->GetSelectedKeyAreas(OutSelectedKeyAreas);

	for (const IKeyArea* KeyArea : OutSelectedKeyAreas)
	{
		TArray<FFrameNumber> OutKeyTimes;
		KeyArea->GetKeyTimes(OutKeyTimes);

		TArray<FKeyHandle> OutKeyHandles;
		KeyArea->GetKeyHandles(OutKeyHandles);

		// Move all keys
		const int32 Num = OutKeyTimes.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			const FFrameNumber& Time = OutKeyTimes[Index];
			const FKeyHandle& Handle = OutKeyHandles[Index];
			KeyArea->SetKeyTime(Handle, Time + OffsetLength);
		}
	}

	// Loop again to normalise data (technically not necessary, but looks nicer)
	Loop();
}
#pragma optimize( "", on )


bool HasLeft(FString& String)
{
	return String.Contains("_L") || String.Contains("Left");
}

void FTGOR_ModifierMirror::FlipLeftToRight(bool bWithOffet)
{
	TWeakPtr<ISequencer> Sequencer = GetSequencer();
	if (!Sequencer.IsValid() || !Sequencer.Pin()->GetFocusedMovieSceneSequence())
	{
		return;
	}

	// Loop every input to catch non-mirrored properties too
	Loop();

	UMovieScene* MovieScene = Sequencer.Pin()->GetFocusedMovieSceneSequence()->GetMovieScene();
	TRange<FFrameNumber> PlaybackRange = MovieScene->GetPlaybackRange();

	TArray<const IKeyArea*> OutSelectedKeyAreas;
	Sequencer.Pin()->GetSelectedKeyAreas(OutSelectedKeyAreas);

	static TMap<FString, FString> Bindings = TMap<FString, FString>({ {"_L.", "_R."}, {".L.", ".R."}, {"Left", "Right"} });

	// Find transforms to move
	TMap<UMovieSceneControlRigParameterSection*, TMap<FName, FMovieSceneFloatChannel*>> Sections;
	for (const IKeyArea* Area : OutSelectedKeyAreas)
	{
		if (UMovieSceneControlRigParameterSection* Section = Cast<UMovieSceneControlRigParameterSection>(Area->GetOwningSection()))
		{
			// The trailing dot is a hack so we can match the end of string and anything between with . suffix
			const FString Name = Area->GetName().ToString() + ".";
			for (const auto& Binding : Bindings)
			{
				if ((Name).Contains(Binding.Key) && Area->GetChannelTypeName() == TEXT("MovieSceneFloatChannel"))
				{
					FMovieSceneFloatChannel* Channel = Area->GetChannel().Cast<FMovieSceneFloatChannel>().Get();
					Sections.FindOrAdd(Section).Emplace(FName(*Name.Replace(*Binding.Key, *Binding.Value).LeftChop(1)), Channel);
				}
			}
		}
	}

	// Deselect so we can properly select the new keys
	Sequencer.Pin()->EmptySelection();

	// Copy data
	for (const auto& Section : Sections)
	{
		const auto Channels = Section.Key->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
		const auto MetaData = Section.Key->GetChannelProxy().GetMetaData<FMovieSceneFloatChannel>();

		TArray<FName> Select;

		const int32 Num = Channels.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			const FName& Name = MetaData[Index].Name;
			if (FMovieSceneFloatChannel* const * Ptr = Section.Value.Find(Name))
			{
				FMovieSceneFloatChannel* Source = *Ptr;
				FMovieSceneFloatChannel* Target = Channels[Index];

				// Make perfect copy
				TArray<FFrameNumber> Times(Source->GetTimes());
				TArray<FMovieSceneFloatValue> Values(Source->GetValues());
				Target->Set(Times, Values);

				Select.Emplace(Name);
			}
		}

		Sequencer.Pin()->SelectByChannels(Section.Key, Select, false, true);
	}

	// Apply looping/mirror operations
	Mirror();
	if (bWithOffet)
	{
		TimeOffsetHalf();
	}
}
