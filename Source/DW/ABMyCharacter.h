// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DW.h"
#include "GameFramework/Character.h"
#include "ABMyCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnRun, bool);
DECLARE_DELEGATE_OneParam(FOnCanLowThinVault, bool);
DECLARE_DELEGATE_OneParam(FOnCanLowThickVault, bool);


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

	struct VecInfo
	{
		float X;
		float Y;
		float Z;
	};

public:
	AABMyCharacter();

public:
	FOnRun				RunEvent;
	FOnCanLowThinVault	LowThinVaultEvent;
	FOnCanLowThickVault	LowThickVaultEvent;

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
	UFUNCTION()
	void	CheckCanLowVault();
	void	LowThinVault(float DeltaTime);
	void	LowThickVault(float DeltaTime);
	FVector CheckWallHeight();
	bool	CheckWallThickness();
	bool	CheckMinMax(float Min, float Max, float Value)
	{
		if (Value <= Max && Value >= Min) return true;
		return false;
	}
	bool	LowThinVaulting = false;
	bool	LowThickVaulting = false;

protected:
	void SetControlMode(ControlMode ControlMode);

private:
	ControlMode CurrentControlMode = ControlMode::GTA;
	FVector		DirectionToMove = FVector::ZeroVector;

	float		ArmLengthTo = 0.0f;
	FRotator	ArmRotationTo = FRotator::ZeroRotator;
	float		ArmLengthSpeed = 0.0f;
	float		ArmRotationSpeed = 0.0f;

	bool		IsRunning = false;

	float		LowThinVaultSpeed = 100.f;
	float		LowThickVaultSpeed = 70.f;

	FVector		LowThickVaultDestPos = FVector::ZeroVector;
};
