// The Gateway of Realities: Planes of Existence.


#include "TGOR_ElementInstance.h"


FTGOR_ElementInstance::FTGOR_ElementInstance()
	: FTGOR_ElementInstance(0.0f)
{
}

FTGOR_ElementInstance::FTGOR_ElementInstance(float Value)
{
	Temperature = Value;
	Pressure = Value;
	Partition = Value;
	Moisture = Value;
	Dissolution = Value;
	Charge = Value;
	Potency = Value;
}

FTGOR_ElementInstance FTGOR_ElementInstance::operator/(float Factor) const
{
	FTGOR_ElementInstance State;

	State.Temperature = Temperature / Factor;
	State.Pressure = Pressure / Factor;
	State.Partition = Partition / Factor;
	State.Moisture = Moisture / Factor;
	State.Dissolution = Dissolution / Factor;
	State.Charge = Charge / Factor;
	State.Potency = Potency / Factor;

	return(State);
}

FTGOR_ElementInstance FTGOR_ElementInstance::operator*(float Factor) const
{
	FTGOR_ElementInstance State;

	State.Temperature = Temperature * Factor;
	State.Pressure = Pressure * Factor;
	State.Partition = Partition * Factor;
	State.Moisture = Moisture * Factor;
	State.Dissolution = Dissolution *  Factor;
	State.Charge = Charge * Factor;
	State.Potency = Potency * Factor;

	return(State);
}

FTGOR_ElementInstance FTGOR_ElementInstance::operator-(const FTGOR_ElementInstance& State) const
{
	FTGOR_ElementInstance Other;

	Other.Temperature = Temperature - State.Temperature;
	Other.Pressure = Pressure - State.Pressure;
	Other.Partition = Partition - State.Partition;
	Other.Moisture = Moisture - State.Moisture;
	Other.Dissolution = Dissolution - State.Dissolution;
	Other.Charge = Charge - State.Charge;
	Other.Potency = Potency - State.Potency;

	return(Other);
}

FTGOR_ElementInstance FTGOR_ElementInstance::operator+(const FTGOR_ElementInstance& State) const
{
	FTGOR_ElementInstance Other = *this;

	Other.Temperature = Temperature + State.Temperature;
	Other.Pressure = Pressure + State.Pressure;
	Other.Partition = Partition + State.Partition;
	Other.Moisture = Moisture + State.Moisture;
	Other.Dissolution = Dissolution + State.Dissolution;
	Other.Charge = Charge + State.Charge;
	Other.Potency = Potency + State.Potency;

	return(Other);
}


FTGOR_ElementInstance FTGOR_ElementInstance::PosMax(const FTGOR_ElementInstance& State) const
{
	FTGOR_ElementInstance Other;

	Other.Temperature = POS_MAX(Temperature, State.Temperature);
	Other.Pressure = POS_MAX(Pressure, State.Pressure);
	Other.Partition = POS_MAX(Partition, State.Partition);
	Other.Moisture = POS_MAX(Moisture, State.Moisture);
	Other.Dissolution = POS_MAX(Dissolution, State.Dissolution);
	Other.Charge = POS_MAX(Charge, State.Charge);
	Other.Potency = POS_MAX(Potency, State.Potency);

	return(Other);
}

FTGOR_ElementInstance FTGOR_ElementInstance::NegMin(const FTGOR_ElementInstance& State) const
{
	FTGOR_ElementInstance Other;

	Other.Temperature = NEG_MIN(Temperature, State.Temperature);
	Other.Pressure = NEG_MIN(Pressure, State.Pressure);
	Other.Partition = NEG_MIN(Partition, State.Partition);
	Other.Moisture = NEG_MIN(Moisture, State.Moisture);
	Other.Dissolution = NEG_MIN(Dissolution, State.Dissolution);
	Other.Charge = NEG_MIN(Charge, State.Charge);
	Other.Potency = NEG_MIN(Potency, State.Potency);

	return(Other);
}


bool FTGOR_ElementInstance::IsInBetween(const FTGOR_ElementInstance& Min, const FTGOR_ElementInstance& Max) const
{
	return(
		BET_MIN_MAX(Temperature, Min.Temperature, Max.Temperature) &&
		BET_MIN_MAX(Pressure, Min.Pressure, Max.Pressure) &&
		BET_MIN_MAX(Partition, Min.Partition, Max.Partition) &&
		BET_MIN_MAX(Moisture, Min.Moisture, Max.Moisture) &&
		BET_MIN_MAX(Dissolution, Min.Dissolution, Max.Dissolution) &&
		BET_MIN_MAX(Charge, Min.Charge, Max.Charge) &&
		BET_MIN_MAX(Potency, Min.Potency, Max.Potency)
	);
}