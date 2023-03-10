// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DW.h"
#include "Animation/AnimInstance.h"
#include "ABMyCharacterAnim.generated.h"

/**
 * 
 */
UCLASS()
class DW_API UABMyCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABMyCharacterAnim();

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION()
	void AnimNotify_EndHighVault();

//	UFUNCTION()
//	void AnimNotify_EndLowThickVault();

private:
	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsRunning;

	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsSliding;

	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	int32 VaultState;

	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	int32 IsRifle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn, Meta = (AllowPrivateAccess = true))
	int32 IsAiming;

private:
	class AABMyCharacter* MyCharacater;
};
