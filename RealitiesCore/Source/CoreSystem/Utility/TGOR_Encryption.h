// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "CoreSystem/Storage/TGOR_Buffer.h"

#include "UObject/NoExportTypes.h"
#include "TGOR_Encryption.generated.h"

/**
 * 
 */
UCLASS()
class CORESYSTEM_API UTGOR_Encryption : public UObject
{
	GENERATED_BODY()

public:

	/**  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		static FTGOR_Buffer Encrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		static FTGOR_Buffer Decrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PrivateKey);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePrivateKey();

	/**  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePublicKey(const FTGOR_Buffer& PrivateKey);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePassword();
};