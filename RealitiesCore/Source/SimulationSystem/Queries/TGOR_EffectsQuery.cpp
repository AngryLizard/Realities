// The Gateway of Realities: Planes of Existence.


#include "TGOR_EffectsQuery.h"

#include "SimulationSystem/Components/TGOR_StatComponent.h"
#include "SimulationSystem/Content/TGOR_Effect.h"
#include "SimulationSystem/Content/TGOR_Stat.h"

#include "CoreSystem/TGOR_Singleton.h"


UTGOR_EffectsQuery::UTGOR_EffectsQuery()
: Super()
{
	Required = true;
}

TArray<UTGOR_CoreContent*> UTGOR_EffectsQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_EffectOutput& Effect : Effects)
	{
		Output.Emplace(Effect.Effect);
	}
	return Output;
}

void UTGOR_EffectsQuery::AssignComponent(UTGOR_StatComponent* StatComponent)
{
	TArray<FTGOR_StatOutput> Stats = StatComponent->GetStats();

	Effects.Empty();
	for (const FTGOR_StatOutput& Stat : Stats)
	{
		for (UTGOR_Effect* Effect : Stat.Active)
		{
			FTGOR_EffectOutput Output;
			Output.Effect = Effect;
			Output.Value = FMath::Clamp((Stat.Current - Effect->Min) / (Effect->Max - Effect->Min), 0.0f, 1.0f);
			Effects.Emplace(Output);
		}
	}

}