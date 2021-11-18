// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once
// Parabola from (0,0) to (1,1) (Ax^2 + Bx)
#define CURVEA -3.111f
#define CURVEB 4.111f

// ln(2) / ln(1/2)
#define LNTWO -0.6931f
#define LNHALF 0.6931f

#define INV_E 0.36787944117f

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "UObject/Object.h"
#include "TGOR_Math.generated.h"

/**
 * 
 */
UCLASS()
class REALITIESUTILITY_API UTGOR_Math : public UObject
{
	GENERATED_BODY()
public:
	//	http://stackoverflow.com/questions/30052316/find-next-prime-number-algorithm
	/** Checks if number is prime */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static bool IsPrime(int32 Number);

	/** Gets next number that is prime */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static int32 NextPrime(int32 Number);

	/** Positive modulo */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static int32 PosMod(int32 X, int32 N);

	/** Computes quick mod2 */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static int32 Log2(int32 X);

	/** Safe inverse. Returns 0 when inverting 0 */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (CompactNodeTitle = "1/", Keywords = "C++"))
		static float SafeInv(float X);

	/** Divide safely. Returns 0 when dividing by 0 */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float SafeDiv(float X, float Div);

	/** Interpolates from 0 to 1, slightly overshooting y = 1 and coming back again, creating wobble effect) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float InterpolateParabola(float X);

	/** introduces a clamp to a function */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float ThresholdClamp(float X, float Threshold);

	/** Add pitch to rotator allowing overturning */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static FRotator AddRotation(const FRotator& Rotator, float Pitch, float Yaw);

	/** Get float from byte relations */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float ByteToRatio(uint8 Byte);

	/** Get byte from float relations */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static uint8 RatioToByte(float Ratio);

	/** Get safe ratio between max and x (1.0 if Max is 0.0) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float SafeRatio(float X, float Max);

	/** Lerps three colors with a byte */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor LerpColors(const FLinearColor& Dark, const FLinearColor& Medium, const FLinearColor& Light, uint8 Byte);

	/** Sets alpha to 1.0 */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor SolidifyColor(const FLinearColor& Color);

	/** Create linear color from rgb */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor BuildColor(int32 DwColor);



	/** Get time in seconds (resets every epoch, use for visuals only) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float GetTimeValue(const FTGOR_Time& Time);

	/** Get duration between two timestamps */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float GetDuration(const FTGOR_Time& From, const FTGOR_Time& To);

	/** Get time modulo */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static float GetTimeMod(const FTGOR_Time& Time, float Period = 1.0f, float Speed = 1.0f, float Phase = 0.0f);
	
	/** Increase vector only if limit allows it, clamp otherwise */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static FVector AddLimited(const FVector& Vector, const FVector& Delta, float Limit);

	/** Get class distance (or -1 if not subclasses) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", Meta = (Keywords = "C++"))
		static int32 GetClassDistance(UClass* Base, UClass* Super);

	/** Convert quaternions into rotators */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "ToRotator (quat)", CompactNodeTitle = "->"))
		static FRotator Conv_QuatToRotator(FQuat InQuat);

	/** Normalizes a vector (estimate), use this instead of the built-in solution if possible so its behaviour (tolerance, precision, etc) can all be changed at once later. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "Normalize"))
		static FVector Normalize(const FVector& Vector);

	/** Clamp a vector to a given size (see explanation above). */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "ClampToSize"))
		static FVector ClampToSize(const FVector& Vector, float Limit);


	/** Higher order smoothstep. Assumes X in [0, 1] */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static float SmoothStep(float X, int32 Order);

	/** Binomial coefficient (a; b) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static float PascalTriangle(int32 A, int32 B);

	/** Integer pow (a; b) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static float IntPow(float Base, int32 Exp);


	/** Get length of vector projected to [-1, 1]^2. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static float ProjectToBox(const FVector2D& Vector);

	/** Project a segment (S - T) onto a slate from A to B with given normal. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static bool ProjectToSlate(const FVector& Normal, const FVector& A, const FVector& B, const FVector& S, const FVector& T);

	/** Computes the circumcenter of a given triangle */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static bool ComputeCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& Out);

	/** Computes the circumcenter of a given triangle */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (Keywords = "C++"))
		static bool ComputeCircumcenter2D(const FVector2D& A, const FVector2D& B, const FVector2D& C, FVector2D& Out);

};
