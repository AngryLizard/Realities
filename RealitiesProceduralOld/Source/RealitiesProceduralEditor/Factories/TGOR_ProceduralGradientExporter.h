// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Exporters/Exporter.h"
#include "TGOR_ProceduralGradientExporter.generated.h"

UCLASS()
class REALITIESPROCEDURALEDITOR_API UTGOR_ProceduralGradientExporterTGA : public UExporter
{
	GENERATED_UCLASS_BODY()


		//~ Begin UExporter Interface
		virtual bool SupportsObject(UObject* Object) const override;
	virtual bool ExportBinary(UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex = 0, uint32 PortFlags = 0) override;
	//~ End UExporter Interface
};

