// The Gateway of Realities: Planes of Existence.
#include "TGOR_PhysicsComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"

#include "Net/UnrealNetwork.h"

UTGOR_PhysicsComponent::UTGOR_PhysicsComponent()
:	Super(),
	PendingTime(0.0f),
	SimulationTimestep(0.003f),

	BufferThreshold(),
	AdjustThreshold(BufferThreshold, 0.4f),
	UpdateThreshold(BufferThreshold, 2.0f),
	ReplayThreshold(0.05),

	AuthoritiveSlave(true),
	ReplayParents(true)
{
	AuthorityMode = ETGOR_MovementAuthorityMode::SlaveClient;
	AuthoritiveSlave = (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveServer);
}


void UTGOR_PhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialise buffer always in case we switch netmode on the fly
	SetBufferSize(64);

	// Create first buffer entry on first state
	StoreToBuffer(BufferTop);

	// Get main volume for physics
	SINGLETON_CHK;
	if (!Capture.PhysicsVolume.IsValid())
	{
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();
		if (IsValid(WorldData))
		{
			Capture.PhysicsVolume = WorldData->GetMainVolume();

			// Notify owner
			OnVolumeChanged.Broadcast(nullptr, Capture.PhysicsVolume.Get());
		}
	}

	// Get surroundings info
	const FTGOR_MovementPosition Position = ComputePosition();
	UpdateSurroundings(Position.Linear);

	// Store in local space
	const FTGOR_MovementBase& MovementBase = GetBase();
	const FTGOR_MovementPosition NewParentPostion = MovementBase.ComputeParentPosition();
	const FQuat ParentAngularInv = NewParentPostion.Angular.Inverse();
	Capture.UpVector = ParentAngularInv * Position.Angular.GetUpVector();
}


void UTGOR_PhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SINGLETON_CHK;
	if (SimulationTimestep >= SMALL_NUMBER)
	{
		// Simulate by one tick if not preempted
		PendingTime += DeltaTime;
		if (PendingTime >= SimulationTimestep)
		{
			TickReset(PendingTime);
			PendingTime = Simulate(PendingTime, false);
			MovementUpdate();
		}
	}
}

void UTGOR_PhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_PhysicsComponent, Capture, COND_SimulatedOnly);
}

void UTGOR_PhysicsComponent::OnPositionChange(const FTGOR_MovementPosition& Position)
{
	UpdateSurroundings(Position.Linear);
	Super::OnPositionChange(Position);
}

void UTGOR_PhysicsComponent::OnReparent(const FTGOR_MovementDynamic& Previous)
{
	// Rotate UpVector to new base
	const FTGOR_MovementBase& MovementBase = GetBase();
	const FTGOR_MovementDynamic ParentDynamic = MovementBase.ComputeParentSpace();
	Capture.UpVector = ParentDynamic.Angular.Inverse() * (Previous.Angular * Capture.UpVector);

	Super::OnReparent(Previous);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_PhysicsComponent::UpdateSurroundings(const FVector& Location)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	ATGOR_PhysicsVolume* MainVolume = WorldData->GetMainVolume();

	ATGOR_PhysicsVolume* OldVolume = Capture.PhysicsVolume.Get();

	// Check current volume before querying everything
	if (IsValid(OldVolume) && OldVolume->IsInside(Location))
	{
		Capture.PhysicsVolume = OldVolume->Query(Location);
	}
	else if (IsValid(MainVolume))
	{
		Capture.PhysicsVolume = MainVolume->Query(Location);

		// Find whether we're out of level
		if (Capture.PhysicsVolume == MainVolume && !Capture.PhysicsVolume->IsInside(Location))
		{
			OutOfLevel();
		}

		// Notify owner
		OnVolumeChanged.Broadcast(OldVolume, Capture.PhysicsVolume.Get());
	}

	// If in own gravity field, assume parent instead
	if (Capture.PhysicsVolume.IsValid())
	{
		UTGOR_MobilityComponent* VolumeMovement = Capture.PhysicsVolume->GetOwning();
		if (IsValid(VolumeMovement) && VolumeMovement->HasParent(this))
		{
			Capture.PhysicsVolume = Capture.PhysicsVolume->GetParentVolume();
		}
	}

	if (Capture.PhysicsVolume.IsValid())
	{
		Capture.Surroundings = Capture.PhysicsVolume->ComputeSurroundings(Location);
	}
}

void UTGOR_PhysicsComponent::OutOfLevel()
{
	OnOutOfLevel.Broadcast();
}

void UTGOR_PhysicsComponent::RepNotifyCapture(const FTGOR_MovementCapture& Old)
{
}

const FTGOR_MovementCapture& UTGOR_PhysicsComponent::GetCapture() const
{
	return Capture;
}


FVector UTGOR_PhysicsComponent::ComputePhysicsUpVector() const
{
	const FTGOR_MovementBase& MovementBase = GetBase();
	const FTGOR_MovementPosition ParentPosition = MovementBase.ComputeParentPosition();
	return ParentPosition.Angular * Capture.UpVector;
}


float UTGOR_PhysicsComponent::GetBouyancyRatio() const
{
	const FTGOR_MovementBody& MovementBody = GetBody();
	if (MovementBody.Mass >= SMALL_NUMBER)
	{
		const float Displaced = MovementBody.GetDisplacedMass(Capture.Surroundings);
		return Displaced / MovementBody.Mass;
	}
	return 0.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_PhysicsComponent::Simulate(float Time, bool Replay)
{
	return Time;
}

void UTGOR_PhysicsComponent::TickReset(float Time)
{

}

void UTGOR_PhysicsComponent::MovementUpdate()
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole > ENetRole::ROLE_None)
	{
		if (GetMasterRole() == NetRole)
		{
			BufferSpread();
		}
		else if (GetSlaveRole() == NetRole)
		{
			BufferUpdate();
		}
	}
}

ENetRole UTGOR_PhysicsComponent::GetMasterRole() const
{
	if (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveClient) return ENetRole::ROLE_Authority;
	if (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveServer) return ENetRole::ROLE_AutonomousProxy;
	return ENetRole::ROLE_Authority;
}

ENetRole UTGOR_PhysicsComponent::GetSlaveRole() const
{
	if (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveClient) return ENetRole::ROLE_AutonomousProxy;
	if (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveServer) return ENetRole::ROLE_Authority;
	return ENetRole::ROLE_SimulatedProxy;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_PhysicsComponent::BufferUpdate()
{
	// Store movement update
	const int32 Size = TimeBuffer.Num();
	BufferTop = (BufferTop + 1) % Size;
	StoreToBuffer(BufferTop);

	// Push forward bottom index if we catch up
	if (BufferTop == BufferBottom)
	{
		BufferBottom = (BufferBottom + 1) % Size;
	}
}

void UTGOR_PhysicsComponent::BufferSpread()
{
	// Update buffer
	BufferUpdate();

	// TODO: Technically don't need to keep the buffer updated to save performance
	// Do movement update if difference becomes too big
	if (BufferPeerAdjust(BufferBottom, BufferTop, BufferThreshold))
	{
		BufferBottom = BufferTop;
	}
}

void UTGOR_PhysicsComponent::BufferForce()
{
	// Update buffer
	BufferUpdate();

	// TODO: Technically don't need to keep the buffer updated to save performance
	// Do movement update (same indices forces resend)
	BufferPeerAdjust(BufferTop, BufferTop, BufferThreshold); // BufferThreshold unused
}

/////////////////////

int32 UTGOR_PhysicsComponent::CurrentBufferIndex() const
{
	return BufferTop;
}

void UTGOR_PhysicsComponent::CacheParentBuffer(UTGOR_MobilityComponent* Parent)
{
	// Don't override already existing cache. Just in case it got moved the earlier version is more likely to be correct.
	if (IsValid(Parent) && !ParentCache.Contains(Parent))
	{
		Parent->ResetToComponent();
		ParentCache.Add(Parent, Parent->GetBase());
	}
}

void UTGOR_PhysicsComponent::SimulateParentCache()
{
	// Reset all moved bases (LoadFromBufferExternal populates cache)
	for (const auto& Cache : ParentCache)
	{
		Cache.Key->SetBase(Cache.Value);
		const FTGOR_MovementPosition Position = Cache.Key->ComputePosition();
	}
}

void UTGOR_PhysicsComponent::BufferReplay(int32 Index)
{
	// Cache gets filled by buffer loads
	ParentCache.Empty();

	// Remember previous position for proper lerp
	const FTGOR_MovementBase OldBase = GetBase();

	// Set to beginning of replay
	LoadFromBufferWhole(Index);
	const int32 Size = TimeBuffer.Num();

	// Iterate and simulate all replay states
	float Time = 0.0f;
	int32 CurrentIndex = Index;
	while (CurrentIndex != BufferTop)
	{
		// Compute timestep
		const FTGOR_Time CurrentTimestamp = TimeBuffer[CurrentIndex];
		const int32 NextIndex = (CurrentIndex + 1) % Size;
		const FTGOR_Time NextTimestamp = TimeBuffer[NextIndex];

		// Simulate this timestep
		Time += NextTimestamp - CurrentTimestamp;
		if (Time > SMALL_NUMBER)
		{
			Time = Simulate(Time, true);
		}

		// Set external and update buffer with current state
		CurrentIndex = NextIndex;
		LoadFromBufferExternal(CurrentIndex);
		StoreToBuffer(CurrentIndex);
	}


	// Network lerp, check for cycles as OldBase wasn't verified
	if (!OldBase.HasCycle())
	{
		MovementAdjust(OldBase, GetBase());
	}

	SimulateParentCache();
}

void UTGOR_PhysicsComponent::BufferForward(FTGOR_Time Timestamp)
{
	// Fast forward acknowledged move
	const int32 Size = TimeBuffer.Num();
	while (BufferBottom != BufferTop)
	{
		// Get next entry, see whether we reached timestamp around interval
		const int32 NextIndex = (BufferBottom + 1) % Size;
		if (TimeBuffer[NextIndex] > Timestamp)
		{
			return;
		}

		BufferBottom = NextIndex;
	}
}

/////////////////////

bool UTGOR_PhysicsComponent::BufferPeerAdjust(int32 PrevIndex, int32 NextIndex, const FTGOR_MovementThreshold& Threshold)
{
	// Compare with current state and adjust peer if necessary
	const FTGOR_MovementUpdate Update = CreateMovementUpdate(NextIndex);
	if (NextIndex == PrevIndex || (BufferCompare(Update, PrevIndex, AdjustThreshold) >= 1.0f))
	{
		switch (GetSlaveRole())
		{
		case ENetRole::ROLE_SimulatedProxy: MulticastBufferAdjust(Update); break;
		case ENetRole::ROLE_AutonomousProxy: ClientBufferAdjust(Update); break;
		case ENetRole::ROLE_Authority: ServerBufferAdjust(Update); break;
		}
		return true;
	}
	return false;
}

void UTGOR_PhysicsComponent::BufferReplicate(int32 Index)
{
	const FTGOR_MovementUpdate Update = CreateMovementUpdate(Index);
	switch (GetMasterRole())
	{
	case ENetRole::ROLE_SimulatedProxy: break;
	case ENetRole::ROLE_AutonomousProxy:  ClientForceUpdate(Update); break;
	case ENetRole::ROLE_Authority: ServerForceUpdate(Update); break; // This should never happen
	}
}

/////////////////////

void UTGOR_PhysicsComponent::LoadFromBufferExternal(int32 Index)
{
	// Cache current buffer state so it can be reset again after replay is done
	// (We want to move other actors only temporarily during replay so the surroundings are also moved back in time)
	UTGOR_MobilityComponent* Parent = GetParent();
	if (IsValid(Parent) && ReplayParents)
	{
		CacheParentBuffer(Parent);
		RecordBuffer[Index].Simulate();
	}

	SetBody(BodyBuffer[Index]);
	Capture.Surroundings = CaptureBuffer[Index].Surroundings;
}

void UTGOR_PhysicsComponent::LoadFromBufferWhole(int32 Index)
{
	SetBase(RecordBuffer[Index].GetRoot());
	Capture = CaptureBuffer[Index];

	// SetBase updates surroundings, Load external after always
	LoadFromBufferExternal(Index);
}

void UTGOR_PhysicsComponent::LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha)
{
	TimeBuffer[PrevIndex] = FMath::Lerp(TimeBuffer[PrevIndex], TimeBuffer[NextIndex], Alpha);

	// Never lerp between different bases
	FTGOR_MovementRecord& PrevBuffer = RecordBuffer[PrevIndex];
	const FTGOR_MovementRecord& NextBuffer = RecordBuffer[NextIndex];
	// else: On parent change during the frame we cannot lerp either so just keep it how it is
	// Do not lerp base before parent (!!!) otherwise different parents might be overridden
	PrevBuffer.Lerp(NextBuffer, Alpha);
	BodyBuffer[PrevIndex].Lerp(BodyBuffer[NextIndex], Alpha);
	CaptureBuffer[PrevIndex].Lerp(CaptureBuffer[NextIndex], Alpha);
}

void UTGOR_PhysicsComponent::StoreToBuffer(int32 Index)
{
	check(TimeBuffer.Num() > Index);
	SINGLETON_CHK;

	// Store timestamp
	TimeBuffer[Index] = Singleton->GetGameTimestamp();
	RecordBuffer[Index] = FTGOR_MovementRecord(GetBase());

	// Store state
	BodyBuffer[Index] = GetBody();
	CaptureBuffer[Index] = Capture;
}

void UTGOR_PhysicsComponent::SetBufferSize(int32 Size)
{
	TimeBuffer.SetNum(Size);
	RecordBuffer.SetNum(Size);
	BodyBuffer.SetNum(Size);
	CaptureBuffer.SetNum(Size);

	BufferTop = BufferBottom = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_PhysicsComponent::TemplateAdjust(BufferCompareFunc Compare, BufferOverrideFunc Override)
{
	const int32 Size = TimeBuffer.Num();

	// Create buffer entry for the current timestamp
	BufferUpdate();

	// Lerp bottom index to closest matching buffer frame
	float BottomError = 1.0f;
	TPair<int32, float> Curr, Next(BufferBottom, 1.0f);
	for (;;)
	{
		// Progress chain by looking for closest match in replay queue, 
		// note that due to BufferUpdate we have always at least one step to go and don't overshoot the buffer
		BufferBottom = Curr.Key;
		BottomError = Curr.Value;
		Curr = Next;
		Next.Key = (Next.Key + 1) % Size;
		Next.Value = Compare(Next.Key, UpdateThreshold);//BufferCompare(Update, Next.Key, UpdateThreshold);

		// Can shortcut process if perfect match was found. Pretty unlikely but triggers a division by zero if not caught.
		if (Next.Value < SMALL_NUMBER)
		{
			BufferBottom = Next.Key;
			BottomError = Next.Value;
			break;
		}
		// Look for sandwich (state with higher error to its left and right)
		// that isn't *too* far apart from what we're looking for (difference lower than 1).
		else if (BottomError >= Curr.Value && Next.Value > Curr.Value&& Curr.Value < 1.0f)
		{
			// Lerp buffer depending on closeness
			const float Alpha = 1.0f - (Curr.Value / (BottomError + Curr.Value));
			LerpToBuffer(BufferBottom, Curr.Key, Alpha);

			// Update error estimation for lower (left) half of the sandwich
			BottomError = Compare(BufferBottom, UpdateThreshold);//BufferCompare(Update, BufferBottom, UpdateThreshold);

			// Use upper (right) half of the sandwich if we have gotten worse!
			if (BottomError > Curr.Value)
			{
				// Take sandwich content
				BufferBottom = Curr.Key;
				BottomError = Curr.Value;

				// Lerp buffer again depending on closeness
				const float Beta = 1.0f - (Next.Value / (Curr.Value + Next.Value));
				LerpToBuffer(BufferBottom, Next.Key, Beta);

				// TODO: Technically not necessary, but useful for stats (records final replay error)
				BottomError = Compare(BufferBottom, UpdateThreshold);//BufferCompare(Update, BufferBottom, UpdateThreshold);
			}

			break;
		}

		// Make sure not to overflow
		else if (Next.Key == BufferTop)
		{
			// Since we have no more information we can assume current buffer is the most accurate
			BufferBottom = BufferTop;
			BottomError = Next.Value;
			break;
		}
	}

	if (BottomError > ReplayThreshold)
	{
		// Check whether we reject the update
		if (AuthoritiveSlave && BottomError >= 1.0f)
		{
			BufferReplicate(BufferBottom);
		}
		else
		{

			Override(BufferBottom);//BufferOverride(Update, BufferBottom);
			BufferReplay(BufferBottom);
		}
	}
}

void UTGOR_PhysicsComponent::TemplateUpdate(BufferOverrideFunc Override)
{
	// Here too we don't want to affect bases
	ParentCache.Empty();

	// Store current state to the buffer, apply update and adjust movement
	const int32 Index = CurrentBufferIndex();
	StoreToBuffer(Index);
	Override(Index);//BufferOverride(Update, Index);
	LoadFromBufferWhole(Index);

	// Reset parents
	SimulateParentCache();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PhysicsComponent::ServerForceUpdate_Implementation(FTGOR_MovementUpdate Update)
{
	ERROR("Authoritive client trying to send force update to server", Error);
}

bool UTGOR_PhysicsComponent::ServerForceUpdate_Validate(FTGOR_MovementUpdate Update)
{
	return true;
}

void UTGOR_PhysicsComponent::ClientForceUpdate_Implementation(FTGOR_MovementUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetMasterRole())
	{
		auto Override = [&Update, this](int32 Index) { return BufferOverride(Update, Index); };
		TemplateUpdate(Override);
	}
}


void UTGOR_PhysicsComponent::ServerBufferAdjust_Implementation(FTGOR_MovementUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return BufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return BufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}

bool UTGOR_PhysicsComponent::ServerBufferAdjust_Validate(FTGOR_MovementUpdate Update)
{
	return true;
}

void UTGOR_PhysicsComponent::ClientBufferAdjust_Implementation(FTGOR_MovementUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return BufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return BufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}

void UTGOR_PhysicsComponent::MulticastBufferAdjust_Implementation(FTGOR_MovementUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return BufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return BufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}

/////////////////////

FTGOR_MovementUpdate UTGOR_PhysicsComponent::CreateMovementUpdate(int32 Index) const
{
	FTGOR_MovementUpdate Update;
	Update.Base = RecordBuffer[Index].GetRoot();
	Update.Timestamp = TimeBuffer[Index];
	Update.UpVector = CaptureBuffer[Index].UpVector;
	return Update;
}

float UTGOR_PhysicsComponent::BufferCompare(const FTGOR_MovementUpdate& Update, int32 Index, const FTGOR_MovementThreshold& Threshold) const
{
	// Compare time
	const float TimeDelta = (TimeBuffer[Index] - Update.Timestamp);
	const float TimeCost = (TimeDelta * TimeDelta) / (Threshold.TimeThreshold * Threshold.TimeThreshold);
	if (TimeCost > 1.0f)
	{
		return 1.0f;
	}

	// Compare base
	const FTGOR_MovementBase& BBase = RecordBuffer[Index].GetRoot();
	const float BufferCost = BBase.Compare(Update.Base, Threshold);
	return FMath::Min(TimeCost + BufferCost, 1.0f);
}

void UTGOR_PhysicsComponent::BufferOverride(const FTGOR_MovementUpdate& Update, int32 Index)
{
	// Make sure base has a component
	FTGOR_MovementBase UpdateBase = Update.Base;
	if (!UpdateBase.Component.IsValid())
	{
		UpdateBase.Component = this;
	}
	RecordBuffer[Index].SetRoot(UpdateBase);
	CaptureBuffer[Index].UpVector = Update.UpVector;
}
