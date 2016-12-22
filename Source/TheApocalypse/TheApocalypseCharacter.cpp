// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheApocalypse.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TheApocalypseCharacter.h"


//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATheApocalypseCharacter::ATheApocalypseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

												   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
												   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATheApocalypseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATheApocalypseCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATheApocalypseCharacter::Crouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATheApocalypseCharacter::StopCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheApocalypseCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheApocalypseCharacter::StopSprint);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATheApocalypseCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATheApocalypseCharacter::StopAiming);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATheApocalypseCharacter::Shoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATheApocalypseCharacter::StopShoot);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATheApocalypseCharacter::Reload);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheApocalypseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheApocalypseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheApocalypseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheApocalypseCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATheApocalypseCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATheApocalypseCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATheApocalypseCharacter::OnResetVR);
}

void ATheApocalypseCharacter::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("%f"),reloadTime)

	if (bIsJumping) {
		jumpTime += DeltaTime;
	}

	if (jumpTime > 0.7) {
		bIsJumping = false;
		jumpTime = 0.f;
	}

	if (ammo <= 0) {
		Reload();
	}

	if (bIsReloading) {
		reloadTime += DeltaTime;
	}

	if (reloadTime >= 2.1f) {
		reloadTime = 0;
		bIsReloading = false;
		bCanShoot = true;
	}



}

void ATheApocalypseCharacter::Shoot(){

	bIsFiring = true;
	PlaySound(FireSound);
}

void ATheApocalypseCharacter::StopShoot(){

	bIsFiring = false;
	
}

UAudioComponent* ATheApocalypseCharacter::PlaySound(USoundCue* SoundToPlay){
	UAudioComponent* AC = nullptr;
	if (SoundToPlay) {
		AC = UGameplayStatics::SpawnSoundAttached(SoundToPlay,
												GetWorld()->GetFirstPlayerController()->GetRootComponent());
	}

	return AC;
}

void ATheApocalypseCharacter::Jump() {

	if (jumpTime == 0.f) {
		bIsJumping = true;
	}

}

void ATheApocalypseCharacter::Crouching() {
	bIsCrouching = true;

}

void ATheApocalypseCharacter::StopCrouch() {
	bIsCrouching = false;
}

void ATheApocalypseCharacter::Sprint() {
	bIsSprinting = true;
	GetCameraBoom()->TargetArmLength = 200.f;
	GetCharacterMovement()->MaxWalkSpeed = 700;
}

void ATheApocalypseCharacter::StopSprint() {
	bIsSprinting = false;
	GetCameraBoom()->TargetArmLength = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = 300;
}

void ATheApocalypseCharacter::Aim() {
	GetCameraBoom()->TargetArmLength = 100.f;
	GetCharacterMovement()->MaxWalkSpeed = 300;
	bIsAiming = true;
	bIsSprinting = false;
}

void ATheApocalypseCharacter::StopAiming() {
	GetCameraBoom()->TargetArmLength = 300.f;
	bIsAiming = false;
}

FRotator ATheApocalypseCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


void ATheApocalypseCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATheApocalypseCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATheApocalypseCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATheApocalypseCharacter::Reload(){
	if (ammo < maxAmmo) {
		bIsReloading = true;
		ammo = maxAmmo;
		bCanShoot = false;
		PlaySound(ReloadSound);
	}
}

void ATheApocalypseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheApocalypseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheApocalypseCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATheApocalypseCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}
