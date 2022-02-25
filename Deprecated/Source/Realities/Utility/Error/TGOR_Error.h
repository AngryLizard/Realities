// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreGlobals.h"
#include "UObject/Object.h"
#include "TGOR_Error.generated.h"

#define ERRET(T, M, R) {UTGOR_Error::Error(T, FString(__FUNCTION__), FString(__FILE__), FString::FromInt(__LINE__), ETGOR_ErrorMode::M); if(ETGOR_ErrorMode::M >= ETGOR_ErrorMode::Error) return (R);}
#define ERROR(T, M ) {UTGOR_Error::Error(T, FString(__FUNCTION__), FString(__FILE__), FString::FromInt(__LINE__), ETGOR_ErrorMode::M); if(ETGOR_ErrorMode::M >= ETGOR_ErrorMode::Error) return;}
#define EPRNT(T) {UTGOR_Error::Error(T, FString(__FUNCTION__), FString(__FILE__), FString::FromInt(__LINE__), ETGOR_ErrorMode::Warning);}
#define EOUT(T) {UTGOR_Error::Error(T, "[INFO]", "", "", ETGOR_ErrorMode::Info);}

#define RPORT(T) {UTGOR_Error::Report(T);}
#define MPORT(T0, T1) {UTGOR_Error::Report(RP(T0) + ": " + RP(T1));}
#define VPORT(V) MPORT(#V, V)

#define RPTCK(T) {UTGOR_Error::Report(T, 0.0f);}
#define MPTCK(T0, T1) {UTGOR_Error::Report(RP(T0) + ": " + RP(T1), 0.0f);}
#define VPTCK(V) MPTCK(#V , V)

#define RP(T) UTGOR_Error::Print(T)

UENUM(BlueprintType)
enum class ETGOR_ErrorMode : uint8
{
	Info,
	Warning,
	Assert,
	Error,
	Fatal
};

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Error : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "TGOR Error", Meta = (Keywords = "C++"))
		static void Error(FString Message, FString Location, FString File, FString Line, ETGOR_ErrorMode Error);

	UFUNCTION(BlueprintCallable, Category = "TGOR Error", Meta = (Keywords = "C++"))
		static void Report(FString Message, float Time = 20.0f);

	UFUNCTION(BlueprintCallable, Category = "TGOR Error", Meta = (Keywords = "C++"))
		static void DebugTrace(UWorld* World, FVector a, FVector b, FColor Color);


	static FString Print(bool B) { return(B ? "true" : "false"); }
	static FString Print(float N) { return((FMath::IsNaN(N) ? "NaN" : FString::SanitizeFloat(N))); }
	static FString Print(int32 N) { return(FString::FromInt(N)); }
	static FString Print(uint32 N) { return(FString::FromInt(N)); }
	static FString Print(uint8 B) { return(FString::FromInt((int)B)); }
	static FString Print(const FVector& V) { return(V.ToString()); }
	static FString Print(const FVector2D& V) { return(V.ToString()); }
	static FString Print(const FRotator& R) { return(R.ToString()); }
	static FString Print(const FMatrix& M) { return(M.ToString()); }
	static FString Print(const FQuat& Q) { return(Q.ToString()); }
	static FString Print(const FName& N) { return(N.ToString()); }
	static FString Print(const char* S) { return(FString(S)); }
	static FString Print(const FString& S) { return(S); }
	static FString Print(const UObject* O) { return(IsValid(O) ? O->GetName() : "None"); }
};
