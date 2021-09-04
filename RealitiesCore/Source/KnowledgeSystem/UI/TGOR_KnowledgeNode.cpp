// The Gateway of Realities: Planes of Existence.


#include "TGOR_KnowledgeNode.h"

#include "KnowledgeSystem/Content/TGOR_Knowledge.h"

#include <type_traits>

/*
	We use bitmasks to encode edge directions where 1's encode a direction being "on".
	To save drawcalls and memory, directions can be flipped. Using the previous state we shift all directions to their mirrored state and combine them to a bitmask.
*/

int constexpr FlipX(int i)
{
	return 
		((i & 0b001001) << 0) | // Up and down (no change)
		((i & 0b000010) << 4) | // Top right (-> top left)
		((i & 0b000100) << 2) | // Bottom right (-> bottom left)
		((i & 0b010000) >> 2) | // Bottom left (-> bottom right)
		((i & 0b100000) >> 4); // Top left (-> top right)
};

int constexpr FlipY(int i)
{
	return
		((i & 0b000001) << 3) | // Up (-> down)
		((i & 0b000010) << 1) | // Top right (-> bottom right)
		((i & 0b000100) >> 1) | // Bottom right (-> top right)
		((i & 0b001000) >> 3) | // Bottom (-> up)
		((i & 0b010000) << 1) | // Bottom left (-> top left)
		((i & 0b100000) >> 1); // Top left (-> bottom left)
};

int constexpr FlipXY(int i)
{
	return FlipX(FlipY(i));
};

template<int I>
struct static_flipmap;

template <>
struct static_flipmap<0>
{
	static const int NUM = 1;
	static const int INDEX = 0;
	static const bool FLIPX = false;
	static const bool FLIPY = false;
};

template<int I>
struct static_flipmap
{
	// Bitmask encoding active directions
	static const int MASK = I;

	using PREV = static_flipmap<I-1>;
	using PREV_FLIPX = static_flipmap<FlipX(I)>;
	using PREV_FLIPY = static_flipmap<FlipY(I)>;
	using PREV_FLIPXY = static_flipmap<FlipXY(I)>;

	// We conditionally check whether a flipped version of this bitmask already exists in earlier iterations so we can index that one instead.
	using COND_FLIPX = typename std::conditional<(FlipX(I) < I), PREV_FLIPX, PREV>::type;
	using COND_FLIPY = typename std::conditional<(FlipY(I) < I), PREV_FLIPY, COND_FLIPX>::type;
	using COND_FLIPXY = typename std::conditional<(FlipXY(I) < I), PREV_FLIPXY, COND_FLIPY>::type;

	static const bool HAS_FLIP = (FlipX(I) < I || FlipY(I) < I || FlipXY(I) < I);

	// Increment index if this is a new unique entry, use lookup table otherwise
	static const int NUM = PREV::NUM + (HAS_FLIP ? 0 : 1);
	static const int INDEX = HAS_FLIP ? COND_FLIPXY::INDEX : (NUM-1);

	// Display whether or not this entry is a flip from an already existing one
	static const bool FLIPX = (FlipXY(I) < I) ? !COND_FLIPXY::FLIPX : ((FlipY(I) < I) ? COND_FLIPY::FLIPX : ((FlipX(I) < I) ? !COND_FLIPX::FLIPX : false));
	static const bool FLIPY = (FlipXY(I) < I) ? !COND_FLIPXY::FLIPY : ((FlipY(I) < I) ? !COND_FLIPY::FLIPY : ((FlipX(I) < I) ? COND_FLIPX::FLIPY : false));
};

// Holds direction id and flip directions
struct FlipmapData
{
	int Index;
	bool FlipX;
	bool FlipY;
};

// We use a dummy variable to propagate so all entries are computed
template <int N, int I = N - 1>
struct FlipmapTable : public FlipmapTable<N, I - 1>
{
protected: static const FlipmapData Dummy;
};

template <int N>
struct FlipmapTable<N, 0>
{
protected: 	static const FlipmapData Dummy;
public: 	static FlipmapData Data[N];
};

// We use recursion to also compute and store values of the flipmap in a static array at compile tiem
template <int N> FlipmapData FlipmapTable<N, 0>::Data[N];
template <int N> const FlipmapData FlipmapTable<N, 0>::Dummy = FlipmapData();
template <int N, int I> const FlipmapData FlipmapTable<N, I>::Dummy = FlipmapTable<N, 0>::Data[I] = FlipmapData({ static_flipmap<I>::INDEX + FlipmapTable<N, I - 1>::Dummy.Index * 0, static_flipmap<I>::FLIPX, static_flipmap<I>::FLIPY });
template struct FlipmapTable<64>;

UTGOR_KnowledgeNode::UTGOR_KnowledgeNode()
:	Super()
{
}

void UTGOR_KnowledgeNode::Reset(UTGOR_Knowledge* Content, bool IsTrail)
{
	Knowledge = Content;
	SetTrailMode(IsTrail);

	DirectionMask.Reset();
	ResetDirections(0, false, false);
}

void UTGOR_KnowledgeNode::AddDirection(int32 Direction, UTGOR_Knowledge* Other)
{
	DirectionMask.Add(Direction, Other);
	UpdateDirections();
}

void UTGOR_KnowledgeNode::UpdateDirections()
{
	// Generate bitmask
	int32 Mask = 0;
	for (const auto& Pair : DirectionMask)
	{
		if (0 <= Pair.Key && Pair.Key < 6 && HasConnectionTo(Pair.Value))
		{
			Mask |= (1 << Pair.Key);
		}
	}

	// Map to direction index
	if (Mask < 64)
	{
		const FlipmapData* Mapping = FlipmapTable<64>::Data;
		ResetDirections(Mapping[Mask].Index, Mapping[Mask].FlipX, Mapping[Mask].FlipY);
	}

}

void UTGOR_KnowledgeNode::PrintFlipmap()
{
	const FlipmapData* Mapping = FlipmapTable<64>::Data;
	for (int i = 0; i < 64; i++)
	{
		RPORT(FString::FromInt(i) + ": " + FString::FromInt(Mapping[i].Index) + " - " + (Mapping[i].FlipX ? "FlipX" : "") + " . " + (Mapping[i].FlipY ? "FlipY" : ""));
	}
}
