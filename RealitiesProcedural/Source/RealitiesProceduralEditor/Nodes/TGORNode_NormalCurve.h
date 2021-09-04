// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesProcedural/Utility/TGOR_NormalCurve.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

enum class ENormalCurveDirection : uint8
{
	Left,
	Right
};

class FTGORNode_NormalCurve : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	FLinearColor GetColor(const FString& Name) const;
	void ParseValue();
	void WriteValue();

	TOptional<float> GetXOptional(ENormalCurveDirection Dir) const;
	float GetXValue(ENormalCurveDirection Dir) const;
	void OnXChanged(float Value, ENormalCurveDirection Dir);

	TOptional<float> GetYOptional(ENormalCurveDirection Dir) const;
	float GetYValue(ENormalCurveDirection Dir) const;
	void OnYChanged(float Value, ENormalCurveDirection Dir);

	TOptional<float> GetZOptional(ENormalCurveDirection Dir) const;
	float GetZValue(ENormalCurveDirection Dir) const;
	void OnZChanged(float Value, ENormalCurveDirection Dir);

	FVector GetCurve(ENormalCurveDirection Dir) const;

private:
	TSharedPtr<IPropertyHandle> NormalCurveUPropertyHandle;

	FTGOR_NormalCurve Props;
};