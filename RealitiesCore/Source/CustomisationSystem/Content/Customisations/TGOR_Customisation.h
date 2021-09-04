// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/TGOR_BodypartInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Customisation.generated.h"

#define PAYLOAD_CHK if(Payload.Bytes.Num() != PayloadSize) ERROR("Payload has wrong size", Error)
#define PAYLOAD_RETCHK(R) if(Payload.Bytes.Num() != PayloadSize) ERRET("Payload has wrong size", Error, R)

#define INIT_TO_PAYLOAD Payload = CreatePayload(); int32 Offset = 0;
#define INIT_FROM_PAYLOAD PAYLOAD_CHK int32 Offset = 0;

#define CONTENT_TO_PAYLOAD(N) {\
	SINGLETON_CHK if(N) { UTGOR_ContentManager* ContentManager = Singleton->GetContentManager(); \
	WORD_TO_PAYLOAD((int16)ContentManager->GetTIndex<UTGOR_##N>(N) + 1);} \
	else WORD_TO_PAYLOAD(0); }

#define CONTENT_FROM_PAYLOAD(N) {\
	SINGLETON_CHK int16 Index = 0; WORD_FROM_PAYLOAD(Index); Index--; \
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager(); \
	N = ContentManager->GetTFromIndex<UTGOR_##N>(Index); }

#define BYTE_TO_PAYLOAD(N) {ByteToPayload(Payload, Offset, N); Offset += 1;}
#define BYTE_FROM_PAYLOAD(N) {N = ByteFromPayload(Payload, Offset); Offset += 1;}

#define WORD_TO_PAYLOAD(N) {WordToPayload(Payload, Offset, N); Offset += 2;}
#define WORD_FROM_PAYLOAD(N) {N = WordFromPayload(Payload, Offset); Offset += 2;}

#define FLOAT_TO_PAYLOAD(N) {FloatToPayload(Payload, Offset, N); Offset += 4;}
#define FLOAT_FROM_PAYLOAD(N) {N = FloatFromPayload(Payload, Offset); Offset += 4;}

#define NORMAL_TO_PAYLOAD(N) {NormalToPayload(Payload, Offset, N); Offset += 1;}
#define NORMAL_FROM_PAYLOAD(N) {N = NormalFromPayload(Payload, Offset); Offset += 1;}

class UTGOR_BodypartCustomisationWidget;
class UTGOR_CustomisationComponent;
class UTGOR_Section;


UENUM(BlueprintType)
enum class ETGOR_LayerTypeEnumeration : uint8
{
	Mandatory,
	Optional,
	Multiple
};

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_Customisation : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Customisation();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Customisation widget that spawns for this customisation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		TSubclassOf<UTGOR_BodypartCustomisationWidget> Widget;

	/** Layer type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		ETGOR_LayerTypeEnumeration LayerType;

	/** Whether this customisation trickles down from parent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		bool TrickleDown;

	/** Size of the payload in bytes */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		int32 PayloadSize;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Validate a customisation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual bool Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart);

	/** Apply a customisation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation);

	/** Query a customisation for default values */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const;

	/** Write payload to package */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual void Write(UPARAM(ref) FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload);

	/** Read payload from package */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual bool Read(UPARAM(ref) FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const;

	/** Merge multiple payloads into one, return whether the merge worked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		virtual bool MergePayloads(const TArray<FTGOR_CustomisationPayload>& Payloads, FTGOR_CustomisationPayload& Merge);

	/** Create payload with correct size */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		FTGOR_CustomisationPayload CreatePayload() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Set byte to payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ByteToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, uint8 Value) const;

	/** Set word to payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void WordToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, int32 Value) const;

	/** Set float to payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FloatToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, float Value) const;

	/** Set [-1..1] to payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void NormalToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, float Value) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get byte from payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		uint8 ByteFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const;

	/** Get word from payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		int32 WordFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const;

	/** Get float from payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		float FloatFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const;

	/** Get [-1..1] from payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		float NormalFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Convert from pixel to rendertarget format */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		ETextureRenderTargetFormat GetRendertargetFormat(EPixelFormat PixelFormant) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Set data to payload */
	void DataToPayload(FTGOR_CustomisationPayload& Payload, int32 Offset, const void* Src, int32 Length) const;

	/** Get data from payload */
	void DataFromPayload(const FTGOR_CustomisationPayload& Payload, int32 Offset, void* Dst, int32 Length) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Geometry this customisation is applied to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSubclassOf<UTGOR_Section> SectionInsertion;
	DECL_INSERTION_REQUIRED(SectionInsertion);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
	
};
