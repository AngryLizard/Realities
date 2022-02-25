// The Gateway of Realities: Planes of Existence.

#pragma once

#include <time.h>

#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"
#include "Realities/Utility/Storage/TGOR_Buffer.h"

#include "TGOR_Data.h"
#include "TGOR_UserData.generated.h"

class ATGOR_OnlineController;

using UserLambda = std::function<bool(FTGOR_UserInstance& User)>;
using ConstUserLambda = std::function<bool(const FTGOR_UserInstance& User)>;
using ControllerLambda = std::function<bool(ATGOR_OnlineController* Controller)>;

/**
* TGOR_UserData allows storing user accounts
*/
UCLASS(BlueprintType)
class REALITIES_API UTGOR_UserData : public UTGOR_Data
{
	GENERATED_BODY()
	
public:
	UTGOR_UserData();

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** See whether there is a user with specified key */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool HasUser(int32 Key) const;

	/** Gets the user on a specified key */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		FTGOR_UserInstance& GetUser(int32 Key);
		bool ForEachController(ControllerLambda Func) const;
		bool ForEachUser(ConstUserLambda Func) const;
		bool ForEachUser(UserLambda Func);

	/** Stores a given user with a randomly generated key. Returns the generated key. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		int32 StoreUser(const FTGOR_UserInstance& User);

	/** Removes an inventory. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void RemoveUser(int32 Key);

	/** Clears all users */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void ClearUserTable();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Updates trackers for a given user */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateTrackers(int32 UserKey, float DeltaSeconds);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Updates all user bodies to current dimension state */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateAllUserBodies();

	/** Updates user bodies to current dimension state */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateUserBodies(int32 UserKey);

	/** Add body to a user, return unique body identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		int32 AddUserBody(int32 UserKey, ATGOR_Avatar* Avatar);

	/** Remove body from a user */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void RemoveUserBody(int32 UserKey, int32 Identifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Encrypts a message using a public key */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		static FTGOR_Buffer Encrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey);

	/** Decrypts a message using a private key */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		static FTGOR_Buffer Decrypt(const FTGOR_Buffer& Message, const FTGOR_Buffer& PrivateKey);

	/** Creates private key randomly */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePrivateKey();

	/** Creates public key from private key */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePublicKey(const FTGOR_Buffer& PrivateKey);

	/** Creates a random password buffer */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		static FTGOR_Buffer CreatePassword();
	
private:

	UPROPERTY()
	TMap<int32, FTGOR_UserInstance> UserBase;	
};
