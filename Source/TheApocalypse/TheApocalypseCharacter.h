// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "TheApocalypseCharacter.generated.h"

UCLASS(config = Game)
class ATheApocalypseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		class USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		class USoundCue* ReloadSound;

	FTimerHandle shootRate;
public:
	ATheApocalypseCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	float jumpTime;

	float reloadTime;


	UPROPERTY(BlueprintReadOnly)
		bool bIsJumping;

	UPROPERTY(BlueprintReadOnly)
		bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly)
		bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly)
		bool bIsAiming;

	UPROPERTY(BlueprintReadOnly)
		bool bIsFiring;

	UPROPERTY(BlueprintReadOnly)
		bool bIsReloading;

	UPROPERTY(BlueprintReadWrite)
		int32 ammo;

	UPROPERTY(BlueprintReadOnly)
		int32 maxAmmo = 30;

	UPROPERTY(BlueprintReadWrite)
		bool bCanShoot;


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Reload")
		void Reload();

	void Jump();

	void Crouching();

	void StopCrouch();

	void Sprint();

	void StopSprint();

	void Aim();

	void StopAiming();

	void Tick(float DeltaTime) override;

	void Shoot();

	void StopShoot();

	UAudioComponent* PlaySound(USoundCue* SoundToPlay);

public:

	UFUNCTION(BlueprintCallable, Category = "Aim Offset")
		FRotator GetAimOffsets() const;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

