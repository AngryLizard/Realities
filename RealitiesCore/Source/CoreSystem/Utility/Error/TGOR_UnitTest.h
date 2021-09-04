// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "UObject/Object.h"
#include "TGOR_UnitTest.generated.h"

#define TGOR_UNITTEST_INHERIT_BEGIN protected: virtual void UnitTestFunction() { Super::UnitTestFunction(); FString UnitTestFile = GetName(); bool UnitTestAssert = true;
#define TGOR_UNITTEST_BEGIN protected: virtual void UnitTestFunction() { FString UnitTestFile = GetName(); bool UnitTestAssert = true;
#define TGOR_ENSURE_STATIC(N, F) { UnitTestAssert = UTGOR_UnitTest::CallEnsureStatic<std::remove_pointer_t<decltype(this)>>(this, #N, UnitTestFile, "Static", []()->bool F) && UnitTestAssert;}
#define TGOR_ENSURE_SINGLE(N, F) { UnitTestAssert = UTGOR_UnitTest::CallEnsureSingle<std::remove_pointer_t<decltype(this)>>(this, #N, UnitTestFile, "Single", [](decltype(this) A)->bool F) && UnitTestAssert;}
#define TGOR_ENSURE_DOUBLE(N, F) { UnitTestAssert = UTGOR_UnitTest::CallEnsureDouble<std::remove_pointer_t<decltype(this)>>(this, #N, UnitTestFile, "Double", [](decltype(this) A, decltype(this) B)->bool F) && UnitTestAssert;}
#define TGOR_UNITTEST_END UTGOR_Error::Error(UnitTestAssert ? "All unit tests succeeded" : "At least one unit test failed", UnitTestFile, "", "", ETGOR_ErrorMode::Assert); };

#define TGOR_UNITTEST_BODY_BEGINPLAY protected: virtual void BeginningPlay(); public: virtual void BeginPlay() override { Super::BeginPlay(); UnitTestFunction(); BeginningPlay(); };
#define TGOR_UNITTEST_BODY public: virtual void BeginPlay() override { Super::BeginPlay(); UnitTestFunction(); };
/**
 * 
 */
UCLASS()
class CORESYSTEM_API UTGOR_UnitTest : public UObject
{
	GENERATED_BODY()

public:

	template<typename U, typename T>
	static bool CallEnsureStatic(UObject* Outer, FString Name, FString File, FString Line, T Func)
	{
		if (!Func())
		{
			UTGOR_Error::Error("Unit test failed", Name, File, Line, ETGOR_ErrorMode::Assert);
			return(false);
		}
		return(true);
	}

	template<typename U, typename T>
	static bool CallEnsureSingle(UObject* Outer, FString Name, FString File, FString Line, T Func)
	{
		U* A = NewObject<U>(Outer);

		if (!Func(A))
		{
			UTGOR_Error::Error("Unit test failed", Name, File, Line, ETGOR_ErrorMode::Assert);
			return(false);
		}
		return(true);
	}

	template<typename U, typename T>
	static bool CallEnsureDouble(UObject* Outer, FString Name, FString File, FString Line, T Func)
	{
		U* A = NewObject<U>(Outer);
		U* B = NewObject<U>(Outer);

		if (!Func(A, B))
		{
			UTGOR_Error::Error("Unit test failed", Name, File, Line, ETGOR_ErrorMode::Assert);
			return(false);
		}
		return(true);
	}
};
