// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesProcedural/Utility/TGOR_NormalCurve.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"


class SCurveEditor;

class FTGORNode_UnitCurve : public IPropertyTypeCustomization, public FCurveOwnerInterface
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	virtual TArray<FRichCurveEditInfoConst> GetCurves() const override;
	virtual TArray<FRichCurveEditInfo> GetCurves() override;
	virtual void ModifyOwner() override;
	virtual TArray<const UObject*> GetOwners() const override;
	virtual void MakeTransactional() override;
	virtual void OnCurveChanged(const TArray<FRichCurveEditInfo>& ChangedCurveEditInfos) override;
	virtual bool IsValidCurve(FRichCurveEditInfo CurveInfo) override;



private:
	TSharedPtr<IPropertyHandle> UnitCurveUPropertyHandle;
	TSharedPtr<SCurveEditor> CurveWidget;

	FTGOR_UnitCurve* UnitCurve;
	UObject* Owner;
};