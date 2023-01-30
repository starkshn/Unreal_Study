// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DW.h"
#include "GameFramework/Character.h"
#include "ABMyCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnRun, bool);
DECLARE_DELEGATE_OneParam(FOnVaultHigh, bool);

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

public:
	AABMyCharacter();

public:
	FOnRun				RunEvent;
	FOnVaultHigh		VaultHighEvent;

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
	bool	CanVault();
	bool	CanVaultToHit(FHitResult& HitResult);
	void	CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius);
	void	VaultTick(float DeltaTime);
	bool	CheckWalkable(float NoramlZ, float WalkableFloorZ)
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
	void SetHighVaultEnd();

private:
	ControlMode CurrentControlMode = ControlMode::GTA;
	FVector		DirectionToMove = FVector::ZeroVector;

	float		ArmLengthTo = 0.0f;
	FRotator	ArmRotationTo = FRotator::ZeroRotator;
	float		ArmLengthSpeed = 0.0f;
	float		ArmRotationSpeed = 0.0f;

	bool		IsRunning = false;

	bool		IsVaulting = false;
	float		Progress = 0.f;
	float		VaultSpeed = 100.f;
	float		MinHighVault = 100;
	float		MaxHighVault = 170;
	FVector		EndingLocation = FVector::ZeroVector;
};
