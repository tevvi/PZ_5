// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Bullet.h"
#include "Animation/AnimInstance.h"
#include "Runtime/Core/Public/Containers/Array.h"
#include "FIrstPerson.generated.h"

UCLASS()
class PZ_5_API AFIrstPerson : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AFIrstPerson();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ThirdPersonCameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Weapon;
	
	float TurnRate;
	float LookUpRate;

	//Animation

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EqiupAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* FireAnimation;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* BulletSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABullet> CurrentBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ABullet>> BulletPack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWeaponIsEquipped;

	int32 BulletIndex = 0;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MyMoveForward(float MoveValue);
	void MyMoveLeft(float MoveLeftValue);
	void MyTurnRate(float TurnRateValue);
	void MyLookUpRate(float LookUpRateValue);

	void OnFire();

	void SwitchWeapon();
	void ToggleWeaponVisibility();
	
	void NextBullet();
	void PreviousBullet();
};
