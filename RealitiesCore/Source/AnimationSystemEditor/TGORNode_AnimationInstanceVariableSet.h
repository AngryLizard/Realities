// The Gateway of Realities: Planes of Existence.

#pragma once

#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class UTGOR_SubAnimInstance;

class FAnimationInstanceVariableSetDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(class IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface
};
