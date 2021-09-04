// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageMinimal.h"

#include "TGOR_Table.generated.h"

/**
* FTGOR_Table allows for efficient serialisation of value arrays with attached capacities
*/
USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_Table
{
	GENERATED_USTRUCT_BODY()
		FTGOR_Table();

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> Capacities;

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> Values;

	UPROPERTY(BlueprintReadWrite)
		TArray<bool> Mask;

	UPROPERTY(BlueprintReadWrite)
		int32 Default;

	template<typename T>
	static FTGOR_Table Make(const TMap<T, int32>& Ranks, const TMap<T, int32>& CapacityMap, const TMap<T, int32>& ValueMap, int32 DefaultValue = INDEX_NONE)
	{
		FTGOR_Table Table;

		const int32 Num = Ranks.Num();
		Table.Capacities.SetNum(Num);
		Table.Values.SetNum(Num);
		Table.Mask.SetNum(Num);

		for (const auto& Pair : Ranks)
		{
			Table.Capacities[Pair.Value] = CapacityMap[Pair.Key];

			Table.Mask[Pair.Value] = ValueMap.Contains(Pair.Key);
			if(Table.Mask[Pair.Value])
			{
				Table.Values[Pair.Value] = FMath::Max(ValueMap[Pair.Key], 0);
				Table.Mask[Pair.Value] = true;
			}
			else
			{
				Table.Values[Pair.Value] = DefaultValue;
				Table.Mask[Pair.Value] = false;
			}
		}

		Table.Default = DefaultValue;
		return Table;
	}

	template<typename T>
	static FTGOR_Table Make(const TMap<T, int32>& Ranks, const TMap<T, int32>& CapacityMap, int32 DefaultValue = INDEX_NONE)
	{
		return Make(Ranks, CapacityMap, TMap<T, int32>(), DefaultValue);
	}

	template<typename T>
	void Populate(const TMap<T, int32>& Ranks, TMap<T, int32>& ValueMap) const
	{
		ValueMap.Empty();
		for (const auto& Pair : Ranks)
		{
			if (Mask[Pair.Value])
			{
				ValueMap.Emplace(Pair.Key, Values[Pair.Value]);
			}
		}
	}
};

