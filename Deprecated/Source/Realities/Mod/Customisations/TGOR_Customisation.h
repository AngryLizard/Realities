// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Customisation.generated.h"

#define PAYLOAD_CHK if(Payload.Num() != PayloadSize) ERROR("Payload has wrong size", Error)
#define PAYLOAD_RETCHK(R) if(Payload.Num() != PayloadSize) ERRET("Payload has wrong size", Error, R)

#define INIT_TO_PAYLOAD Payload = CreatePayload(); int32 Offset = 0;
#define INIT_FROM_PAYLOAD PAYLOAD_CHK int32 Offset = 0;

#define CONTENT_TO_PAYLOAD(N) {\
	if(Singleton && Content) { UTGOR_ContentManager* ContentManager = Singleton->GetContentManager(); \
	WORD_TO_PAYLOAD((int16)ContentManager->GetTIndex<UTGOR_##N>(Content) + 1)} \
	else WORD_TO_PAYLOAD(0); }

#define CONTENT_FROM_PAYLOAD(N) {\
	SINGLETON_CHK int16 Index = 0; WORD_FROM_PAYLOAD(Index); Index--; \
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager(); \
	Content = ContentManager->GetTFromIndex<UTGOR_##N>(Index); }

#define BYTE_TO_PAYLOAD(N) {ByteToPayload(Payload, Offset, N); Offset += 1;}
#define BYTE_FROM_PAYLOAD(N) {N = ByteFromPayload(Payload, Offset); Offset += 1;}

#define WORD_TO_PAYLOAD(N) {WordToPayload(Payload, Offset, N); Offset += 2;}
#define WORD_FROM_PAYLOAD(N) {N = WordFromPayload(Payload, Offset); Offset += 2;}

#define FLOAT_TO_PAYLOAD(N) {FloatToPayload(Payload, Offset, N); Offset += 4;}
#define FLOAT_FROM_PAYLOAD(N) {N = FloatFromPayload(Payload, Offset); Offset += 4;}

#define NORMAL_TO_PAYLOAD(N) {NormalToPayload(Payload, Offset, N); Offset += 1;}
#define NORMAL_FROM_PAYLOAD(N) {N = NormalFromPayload(Payload, Offset); Offset += 1;}

class UTGOR_BodypartCustomisationWidget;
class UTGOR_ModularSkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Customisation : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Customisation();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Customisation widget that spawns for this customisation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Customisation")
		TSubclassOf<UTGOR_BodypartCustomisationWidget> Widget;

	/** Size of the payload in bytes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Customisation")
		int32 PayloadSize;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Validate a customisation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		virtual bool Validate(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart);

	/** Apply a customisation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance);

	/** Query a customisation for default values */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const;

	/** Write payload to package */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Write(UPARAM(ref) FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload);

	/** Read payload from package */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		virtual bool Read(UPARAM(ref) FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const;

	/** Create payload with correct size */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		TArray<uint8> CreatePayload() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Set byte to payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ByteToPayload(TArray<uint8>& Payload, int32 Offset, uint8 Value) const;

	/** Set word to payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void WordToPayload(TArray<uint8>& Payload, int32 Offset, int32 Value) const;

	/** Set float to payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void FloatToPayload(TArray<uint8>& Payload, int32 Offset, float Value) const;

	/** Set [-1..1] to payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void NormalToPayload(TArray<uint8>& Payload, int32 Offset, float Value) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get byte from payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		uint8 ByteFromPayload(const TArray<uint8>& Payload, int32 Offset) const;

	/** Get word from payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		int32 WordFromPayload(const TArray<uint8>& Payload, int32 Offset) const;

	/** Get float from payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		float FloatFromPayload(const TArray<uint8>& Payload, int32 Offset) const;

	/** Get [-1..1] from payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		float NormalFromPayload(const TArray<uint8>& Payload, int32 Offset) const;
	

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Set data to payload */
	void DataToPayload(TArray<uint8>& Payload, int32 Offset, const void* Src, int32 Length) const;

	/** Get data from payload */
	void DataFromPayload(const TArray<uint8>& Payload, int32 Offset, void* Dst, int32 Length) const;
};
