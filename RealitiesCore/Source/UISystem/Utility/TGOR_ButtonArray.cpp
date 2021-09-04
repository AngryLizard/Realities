// The Gateway of Realities: Planes of Existence.

#include "TGOR_ButtonArray.h"

#include "UISystem/Utility/TGOR_ButtonEntry.h"
#include "UISystem/Queries/TGOR_SelectionQuery.h"

void UTGOR_ButtonArray::ApplyQuery(UTGOR_Query* Query)
{
	Super::ApplyQuery(Query);

	OpenedQuery = Cast<UTGOR_SelectionQuery>(Query);
	if (IsValid(OpenedQuery))
	{
		OpenedQuery->AssignParent(this);
		TArray<FTGOR_Display> Displays = OpenedQuery->QueryDisplays();
		Display(Displays);

		OpenedQuery->AssignParent(this);

		APlayerController* OwningPlayer = GetOwningPlayer();

		if (*ButtonEntryType)
		{
			// Check current entries for validity, discard all if there is an entry where there shouldn't be
			for (int32 Index = 0; Index < ButtonEntries.Num(); Index++)
			{
				UTGOR_ButtonEntry* Entry = ButtonEntries[Index];
				if ((IsValid(Entry) && (!Entry->IsA(ButtonEntryType) || FilterCreateEntry(Index))) ||
					(!IsValid(Entry) && !FilterCreateEntry(Index)))
				{
					ButtonEntries.Empty();
				}
			}

			// Create new buttons if not enough have been spawned yet
			int32 Num = Displays.Num();
			if (Num != ButtonEntries.Num())
			{
				OnClearEntries();
				const int32 Offset = FMath::Min(ButtonEntries.Num(), Num);
				for (int32 Index = 0; Index < Offset; Index++)
				{
					if (!FilterCreateEntry(Index))
					{
						// Re-add slots that were already created but not removed
						OnCreateEntry(ButtonEntries[Index], Index);
					}
				}

				ButtonEntries.SetNumZeroed(Num);
				for (int32 Index = Offset; Index < Num; Index++)
				{
					if (!FilterCreateEntry(Index))
					{
						// Create Entries on empty slots
						if (IsValid(OwningPlayer))
						{
							ButtonEntries[Index] = CreateWidget<UTGOR_ButtonEntry>(OwningPlayer, ButtonEntryType);
						}
						else
						{
							ButtonEntries[Index] = CreateWidget<UTGOR_ButtonEntry>(this, ButtonEntryType);
						}

						OnCreateEntry(ButtonEntries[Index], Index);
					}
					else
					{
						ButtonEntries[Index] = nullptr;
					}
				}
			}

			// Assign entries in defined order
			for (int32 Index = 0; Index < Num; Index++)
			{
				const int32 EntryIndex = (BackwardsEntry ? (Num - Index - 1) : Index);
				UTGOR_ButtonEntry* Entry = ButtonEntries[EntryIndex];
				if (IsValid(Entry))
				{
					Entry->Assign(Displays[Index], OpenedQuery, Index);
				}
			}
		}
		else
		{
			OnClearEntries();
			ButtonEntries.Reset();
		}

		OnArrayInit.Broadcast();
	}
}

void UTGOR_ButtonArray::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsDesignTime())
	{
		OnClearEntries();

		if (*ButtonEntryType)
		{
			APlayerController* OwningPlayer = GetOwningPlayer();
			for (int32 Index = 0; Index < PreviewEntryCount; Index++)
			{
				if (!FilterCreateEntry(Index))
				{
					UTGOR_ButtonEntry* Entry = CreateWidget<UTGOR_ButtonEntry>(this, ButtonEntryType);
					OnCreateEntry(Entry, Index);
				}
			}
		}
	}
}

void UTGOR_ButtonArray::Commit(int32 Selection)
{
	OnCommit(Selection);
	if (IsValid(OpenedQuery))
	{
		OnButtonCommit.Broadcast(Selection);
	}
}

void UTGOR_ButtonArray::Select(int32 Selection)
{
	OnSelect(Selection);
	if (IsValid(OpenedQuery))
	{
		OnButtonSelect.Broadcast(Selection);
	}
}