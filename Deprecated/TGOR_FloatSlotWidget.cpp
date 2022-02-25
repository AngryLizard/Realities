#include "TGOR_FloatSlotWidget.h"

#include "Base/Inventory/TGOR_FloatStorage.h"

UTGOR_FloatSlotWidget::UTGOR_FloatSlotWidget()
	: Super()
{
}

float UTGOR_FloatSlotWidget::GetFloat() const
{
	auto Storage = GetStorageCasted<UTGOR_FloatStorage>();
	if (Storage)
	{
		return Storage->GetFloat(Index);
	}

	return 0.0f;
}

bool UTGOR_FloatSlotWidget::PutFloat(float Value)
{
	auto Storage = GetStorageCasted<UTGOR_FloatStorage>();
	if (Storage)
	{
		return Storage->ReplaceFloat(Value, Index) != INDEX_NONE;
	}

	return false;
}