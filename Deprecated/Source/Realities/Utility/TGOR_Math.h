// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once
// Parabola from (0,0) to (1,1) (Ax^2 + Bx)
#define CURVEA -3.111f
#define CURVEB 4.111f

// ln(2) / ln(1/2)
#define LNTWO -0.6931f
#define LNHALF 0.6931f

#include "Realities/Utility/Datastructures/TGOR_Matrix3x3.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "TGOR_Math.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Math : public UObject
{
	GENERATED_BODY()
public:
	//	http://stackoverflow.com/questions/30052316/find-next-prime-number-algorithm
	/** Checks if number is prime */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static bool IsPrime(int32 Number);

	/** Gets next number that is prime */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static int32 NextPrime(int32 Number);

	/** Positive modulo */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static int32 PosMod(int32 X, int32 N);

	/** Computes quick mod2 */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static int32 Log2(int32 X);

	/** Safe inverse. Returns 0 when inverting 0 */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (CompactNodeTitle = "1/", Keywords = "C++"))
		static float SafeInv(float X);

	/** Divide safely. Returns 0 when dividing by 0 */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float SafeDiv(float X, float Div);

	/** Interpolates from 0 to 1, slightly overshooting y = 1 and coming back again, creating wobble effect) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float InterpolateParabola(float X);

	/** introduces a clamp to a function */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float ThresholdClamp(float X, float Threshold);

	/** Add pitch to rotator allowing overturning */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static FRotator AddRotation(const FRotator& Rotator, float Pitch, float Yaw);

	/** Get float from byte relations */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float ByteToRatio(uint8 Byte);

	/** Get byte from float relations */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static uint8 RatioToByte(float Ratio);

	/** Get safe ratio between max and x (1.0 if Max is 0.0) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float SafeRatio(float X, float Max);

	/** Lerps three colours with a byte */
	UFUNCTION(BlueprintCallable, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor LerpColours(const FLinearColor& Dark, const FLinearColor& Medium, const FLinearColor& Light, uint8 Byte);

	/** Sets alpha to 1.0 */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor SolidifyColour(const FLinearColor& Colour);

	/** Create linear colour from rgb */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static FLinearColor BuildColour(int32 DwColour);



	/** Get time in seconds (resets every epoch, use for visuals only) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float GetTimeValue(const FTGOR_Time& Time);

	/** Get duration between two timestamps */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float GetDuration(const FTGOR_Time& From, const FTGOR_Time& To);

	/** Get time modulo */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static float GetTimeMod(const FTGOR_Time& Time, float Period = 1.0f, float Speed = 1.0f, float Phase = 0.0f);
	
	/** Increase vector only if limit allows it, clamp otherwise */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static FVector AddLimited(const FVector& Vector, const FVector& Delta, float Limit);

	/** Get class distance (or -1 if not subclasses) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", Meta = (Keywords = "C++"))
		static int32 GetClassDistance(UClass* Base, UClass* Super);

	/** Convert quaternions into rotators */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ToRotator (quat)", CompactNodeTitle = "->"))
		static FRotator Conv_QuatToRotator(FQuat InQuat);

	/** Normalizes a vector (estimate), use this instead of the built-in solution if possible so its behaviour (tolerance, precision, etc) can all be changed at once later. */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "Normalize"))
		static FVector Normalize(const FVector& Vector);

	/** Clamp a vector to a given size (see explanation above). */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ClampToSize"))
		static FVector ClampToSize(const FVector& Vector, float Limit);


	/** Higher order smoothstep. Assumes X in [0, 1] */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ClampToSize"))
		static float SmoothStep(float X, int32 Order);

	/** Binomial coefficient (a; b) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ClampToSize"))
		static float PascalTriangle(int32 A, int32 B);

	/** Integer pow (a; b) */
	UFUNCTION(BlueprintPure, Category = "TGOR Math", meta = (DisplayName = "ClampToSize"))
		static float IntPow(float Base, int32 Exp);

};
