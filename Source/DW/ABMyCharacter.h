// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DW.h"
#include "GameFramework/Character.h"
#include "ABMyCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnRun, bool);
DECLARE_DELEGATE_OneParam(FOnVaultEvent, int32);

UCLASS()
class DW_API AABMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	enum class ControlMode
	{
		GTA,
		DAIBLO,
	};

	enum class VaultMode
	{
		CantVault,
		HighVaulting,
		LowThinVaulting,
		LowThickVaulting,
	};

public:
	AABMyCharacter();

public:
	FOnRun				RunEvent;
	FOnVaultEvent		VaultEvent;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = Character, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	USkeletalMeshComponent* SK_Mesh;

	UPROPERTY(VisibleAnywhere, Category = Character, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCharacterMovementComponent* CharacterMovementComp;

	UPROPERTY(VisibleAnywhere, Category = Character, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, Category = Character, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UABMyCharacterAnim* MyAnim;

public:
	void ViewChange();
	void Jump();
	void PressedRun();
	void ReleasedRun();

	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

public:
	VaultMode	CanVault();
	VaultMode	CanVaultToHit(FHitResult& HitResult);
	void		CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius);
	VaultMode	CheckThinOrThick();
	float		VaultTick(float DeltaTime);
	bool		CheckWalkable(float NoramlZ, float WalkableFloorZ)
	{
		if (NoramlZ > WalkableFloorZ) return true;
		return false;
	}
	bool	CheckMinMax(float Min, float Max, float Value)
	{
		if (Value <= Max && Value >= Min) return true;
		return false;
	}

public:
	void SetControlMode(ControlMode ControlMode);
	void SetEndingLocation(FVector EndingPos) { EndingLocation = EndingPos; }
	void SetVaultEnd();

private:
	ControlMode CurrentControlMode = ControlMode::GTA;
	FVector		DirectionToMove = FVector::ZeroVector;

	float		ArmLengthTo = 0.0f;
	FRotator	ArmRotationTo = FRotator::ZeroRotator;
	float		ArmLengthSpeed = 0.0f;
	float		ArmRotationSpeed = 0.0f;

	bool		IsRunning = false;

	VaultMode	VaultState = VaultMode::CantVault;
	float		Progress = 0.f;
	float		VaultSpeed = 1.f;
	float		MinHighVault = 100;
	float		MaxHighVault = 170;
	float		MinLowVault = 40;
	float		MaxLowVault = 90;

	float		LowVaultRange = 130.f;
	float		MinLowDepth = 30;
	float		MaxLowDepth = 90;
	float		ForLowVaultCheck = 30.f;
	FVector		StartingLocation = FVector::ZeroVector;
	FVector		EndingLocation = FVector::ZeroVector;
};
