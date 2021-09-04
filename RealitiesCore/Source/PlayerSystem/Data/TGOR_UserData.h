// The Gateway of Realities: Planes of Existence.

#pragma once

#include "PlayerSystem/TGOR_UserInstance.h"

#include "CoreSystem/Data/TGOR_Data.h"
#include "TGOR_UserData.generated.h"

class ATGOR_OnlineController;
class UTGOR_TrackedComponent;

using UserLambda = std::function<bool(FTGOR_UserInstance& User)>;
using ConstUserLambda = std::function<bool(const FTGOR_UserInstance& User)>;
using ControllerLambda = std::function<bool(ATGOR_OnlineController* Controller)>;

/**
* TGOR_UserData allows storing user accounts
*/
UCLASS(BlueprintType)
class PLAYERSYSTEM_API UTGOR_UserData : public UTGOR_Data
{
	GENERATED_BODY()
	
public:
	UTGOR_UserData();

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** See whether there is a user with specified key */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool HasUser(int32 Key) const;

	/** Gets the user on a specified key */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		FTGOR_UserInstance& GetUser(int32 Key);
		bool ForEachController(ControllerLambda Func) const;
		bool ForEachUser(ConstUserLambda Func) const;
		bool ForEachUser(UserLambda Func);

	/** Stores a given user with a randomly generated key. Returns the generated key. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		int32 StoreUser(const FTGOR_UserInstance& User);

	/** Removes an inventory. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		void RemoveUser(int32 Key);

	/** Clears all users */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void ClearUserTable();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Updates trackers for a given user */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void UpdateTrackers(int32 UserKey, float DeltaSeconds);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Add body to a user, return unique body identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		int32 AddUserBody(int32 UserKey, UTGOR_TrackedComponent* Component);

	/** Remove body from a user */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void RemoveUserBody(int32 UserKey, int32 Identifier);
	
private:

	UPROPERTY()
	TMap<int32, FTGOR_UserInstance> UserBase;	
};
