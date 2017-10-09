// Fill out your copyright notice in the Description page of Project Settings.

#include "FIrstPerson.h"
//#include "GameFramework/InputSettings.h"

// Sets default values
AFIrstPerson::AFIrstPerson()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(45.f, 96.f);

	TurnRate = 45.f;
	LookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.2f;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;


	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;
	

	//SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'
	ConstructorHelpers::FObjectFinder<USkeletalMesh> Load_SkeletalMesh(TEXT("SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (Load_SkeletalMesh.Object)
		this->GetMesh()->SetSkeletalMesh(Load_SkeletalMesh.Object);

	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh());

	//SkeletalMesh'/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun.SK_FPGun'
	ConstructorHelpers::FObjectFinder<USkeletalMesh> Load_Weapon(TEXT("SkeletalMesh'/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (Load_Weapon.Object)
		Weapon->SetSkeletalMesh(Load_Weapon.Object);
	
	BulletSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPoint"));
	BulletSpawnPoint->SetupAttachment(Weapon);
	BulletSpawnPoint->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AFIrstPerson::BeginPlay()
{
	Super::BeginPlay();
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
	bWeaponIsEquipped = true;
	Weapon->SetVisibility(bWeaponIsEquipped);

	if (BulletPack.Num())
		CurrentBullet = BulletPack[BulletIndex];
}

// Called every frame
void AFIrstPerson::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFIrstPerson::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFIrstPerson::OnFire);
	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &AFIrstPerson::SwitchWeapon);

	PlayerInputComponent->BindAction("NextBullet", IE_Pressed, this, &AFIrstPerson::NextBullet);
	PlayerInputComponent->BindAction("PreviousBullet", IE_Pressed, this, &AFIrstPerson::PreviousBullet);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFIrstPerson::MyMoveForward);
	PlayerInputComponent->BindAxis("MoveLeft", this, &AFIrstPerson::MyMoveLeft);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFIrstPerson::MyTurnRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFIrstPerson::MyLookUpRate);

}

void AFIrstPerson::MyMoveForward(float MoveValue)
{
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, MoveValue);
	}
}
void AFIrstPerson::MyMoveLeft(float MoveLeftValue)
{
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, MoveLeftValue);
	}
}
void AFIrstPerson::MyTurnRate(float TurnRateValue)
{
	AddControllerYawInput(TurnRateValue * TurnRate * GetWorld()->GetDeltaSeconds());
}
void AFIrstPerson::MyLookUpRate(float LookUpRateValue)
{
	AddControllerYawInput(LookUpRateValue * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFIrstPerson::OnFire()
{
	if (CurrentBullet && bWeaponIsEquipped)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			const FRotator SpawnRotate = BulletSpawnPoint->GetComponentRotation();
			const FVector SpawnLocation = (BulletSpawnPoint) ? BulletSpawnPoint->GetComponentLocation() : GetActorLocation() + SpawnRotate.RotateVector(FVector(100.0f, 0.0f, 10.0f));

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			World->SpawnActor<ABullet>(CurrentBullet, SpawnLocation, SpawnRotate , ActorSpawnParams);
		}
		if (FireAnimation)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
				AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	
}

void AFIrstPerson::SwitchWeapon()
{
	if (bWeaponIsEquipped == Weapon->IsVisible())
	{ 
		bWeaponIsEquipped = !bWeaponIsEquipped;
		FTimerHandle HandlTime;
		if (EqiupAnimation)
		{
			GetWorldTimerManager().SetTimer(HandlTime, this, &AFIrstPerson::ToggleWeaponVisibility, EqiupAnimation->GetPlayLength(), false);

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
				AnimInstance->Montage_Play(EqiupAnimation, 1.f);
		}
		else 
			GetWorldTimerManager().SetTimer(HandlTime, this, &AFIrstPerson::ToggleWeaponVisibility, 0.1f, false);
	}
}


void AFIrstPerson::ToggleWeaponVisibility()
{
	Weapon->ToggleVisibility();
}

void AFIrstPerson::NextBullet()
{
	if (BulletPack.IsValidIndex(++BulletIndex))
		CurrentBullet = BulletPack[BulletIndex];
	else
		BulletIndex--;
		
}
void AFIrstPerson::PreviousBullet()
{
	if (BulletPack.IsValidIndex(--BulletIndex))
		CurrentBullet = BulletPack[BulletIndex];
	else
		BulletIndex++;
}