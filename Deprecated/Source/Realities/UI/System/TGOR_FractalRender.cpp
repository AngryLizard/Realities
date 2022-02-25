// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_FractalRender.h"



UTGOR_FractalRender::UTGOR_FractalRender(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeCount = 512;
	NodeLength = 64.0f;
	NodeFade = 0.01f;

	memset(Nodes, 0, NODE_COUNT * sizeof(Node));
	FractalColor = FLinearColor((float)0xFF / 0xFF, (float)0xd2 / 0xFF, (float)0x03 / 0xFF, 0.15f);
	Empty = nullptr;
}

bool UTGOR_FractalRender::Initialize()
{
	bool result = Super::Initialize();
	WidgetSize = FVector2D(0.0f, 0.0f);
	setRandom(&(Nodes[0]));
	return(result);
}

void UTGOR_FractalRender::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	WidgetSize = MyGeometry.GetLocalSize();

	generateNodes();
	entertainNodes();
}


void UTGOR_FractalRender::setNormal(Node* N)
{
	float a = FMath::FRand() * TWOPI;
	FVector2D d(FMath::Cos(a) * NodeLength, FMath::Sin(a) * NodeLength);
	if (!N->C.IsZero()) d += (N->C - N->P);
	if (!N->L.IsZero()) d += (N->L - N->P);
	if (!N->R.IsZero()) d += (N->R - N->P);
	N->N = d.GetSafeNormal() * -1;
}

void UTGOR_FractalRender::disconnect(Node* N)
{
	N->L = N->R = N->C = FVector2D::ZeroVector;
}

void UTGOR_FractalRender::connect(Node* N, Node* M)
{

	if (N->C.IsZero())			N->C = M->P;
	else if (N->L.IsZero())		N->L = M->P;
	else if (N->R.IsZero())		N->R = M->P;

	setNormal(N);

	if (M->C.IsZero())		M->C = N->P;
	else if (M->L.IsZero())	M->L = N->P;
	else if (M->R.IsZero())	M->R = N->P;

	setNormal(M);
}

bool UTGOR_FractalRender::isFree(const Node* N) const
{
	return(!N->B && (N->L.IsZero() || N->R.IsZero() || N->C.IsZero()));
}

bool UTGOR_FractalRender::isOutOfBounds(const Node* N) const
{
	return(N->P.X < 0.0f || N->P.X >= WidgetSize.X || N->P.Y < 0.0f || N->P.Y >= WidgetSize.Y);
}

bool UTGOR_FractalRender::isFriend(const Node* N, const Node* M) const
{
	return (N == M || N->C == M->P || N->L == M->P || N->R == M->P);
}

void UTGOR_FractalRender::setRandom(Node* N)
{
	disconnect(N);
	N->P.X = (WidgetSize.X + WidgetSize.X * (FMath::FRand() - 0.5f) / 2) / 2;
	N->P.Y = (WidgetSize.Y + WidgetSize.Y * (FMath::FRand() - 0.5f) / 2) / 2;
	N->T = NodeFade;
}

UTGOR_FractalRender::Node* UTGOR_FractalRender::getRandomLeaf()
{
	Node* c = nullptr;
	for (int i = NODE_COUNT; i >= 0; i--)
		if ((c = &Nodes[i])->T > 0.0f)
			if (isFree(c))
				return(c);
	return(c);
}

UTGOR_FractalRender::Node* UTGOR_FractalRender::getClosest(Node* N)
{
	float Dist = FLT_MAX;
	Node* r = nullptr;
	Node* c = nullptr;
	for (int i = NODE_COUNT; i >= 0; i--)
		if ((c = &Nodes[i])->T > 0.0f)
			if (isFree(c) && !isFriend(N, c))
			{
				float l = FVector2D::Distance(c->P, N->P);
				if (l < Dist)
					if (FVector2D::DotProduct(N->N, c->N) <= 0.0f)
					{
						Dist = l;
						r = c;
					}
			}
	return(r);
}

void UTGOR_FractalRender::generateNodes()
{
	if (Empty == nullptr) return;
	Node* n = getRandomLeaf();
	if (n == nullptr) return;
	Node* c = getClosest(n);
	float Dist = 2.0f * NodeLength;
	if (c == nullptr || (Dist = FVector2D::Distance(c->P, n->P)) > NodeLength)
	{
		c = Empty;
		Empty = nullptr;
		setNormal(c);
		c->P = n->P + n->N * Dist / 2;
		c->T = NodeFade;
		c->B = false;
	}

	if (isOutOfBounds(c)) { setRandom(c); n->B = true; }
	else					connect(n, c);
}

void UTGOR_FractalRender::entertainNodes()
{
	Node* c = nullptr;
	for (int i = 0; i < NodeCount; i++)
		if ((c = &Nodes[i])->T > 0.0f)
		{
			c->T += NodeFade;
			if (c->T >= 2.0f)
			{
				disconnect(c);
				c->T = 0.0f;
			}
		}
		else
			Empty = c;
}

/*
int32 UTGOR_FractalRender::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FLinearColor color = FractalColor;
	for (int i = 0; i < NodeCount; i++)
	{
		const Node* c = &Nodes[i];

		float T = 1.0f - c->T;
		color.A = (1.0f - ABS(T)) * FractalColor.A;
		
		FSlateDrawElement::MakeLines(OutDrawElements, ++LayerId, AllottedGeometry.ToPaintGeometry(), TArray<FVector2D>({ c->P, c->C }), ESlateDrawEffect::None, color, true);
		FSlateDrawElement::MakeLines(OutDrawElements, ++LayerId, AllottedGeometry.ToPaintGeometry(), TArray<FVector2D>({ c->P, c->L }), ESlateDrawEffect::None, color, true);
		FSlateDrawElement::MakeLines(OutDrawElements, ++LayerId, AllottedGeometry.ToPaintGeometry(), TArray<FVector2D>({ c->P, c->R }), ESlateDrawEffect::None, color, true);
	}

	return(LayerId);
}
*/
