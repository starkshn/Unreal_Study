// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMyCharacterAnim.h"
#include "ABMyCharacter.h"

UABMyCharacterAnim::UABMyCharacterAnim()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
}

void UABMyCharacterAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	auto Pawn = TryGetPawnOwner();
	MyCharacater = Cast<AABMyCharacter>(Pawn);
	MyCharacater->RunEvent.BindLambda
	(
		[this](bool value) -> void
		{
			IsRunning = value;
		}
	);

	MyCharacater->VaultHighEvent.BindLambda
	(
		[this](bool value) -> void
		{
			IsVaultingHigh = value;
		}
	);
}

void UABMyCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<AABMyCharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UABMyCharacterAnim::AnimNotify_EndHighVault()
{
	auto Pawn = TryGetPawnOwner();
	auto Character = Cast<AABMyCharacter>(Pawn);
	Character->SetHighVaultEnd();
	IsVaultingHigh = false;
}


//void UABMyCharacterAnim::AnimNotify_EndLowThinVault()
//{
//	ABLOG_S(Warning);
//	CanLowThinVault = false;
//	auto Pawn = TryGetPawnOwner();
//	auto Character = Cast<AABMyCharacter>(Pawn);
//	Character->CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	Character->CharacterMovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
//	Character->EnableInput(PlayerController);
//	Character->LowThinVaulting = false;
//}
//
//void UABMyCharacterAnim::AnimNotify_EndLowThickVault()
//{
//	ABLOG_S(Warning);
//	CanLowThickVault = false;
//	auto Pawn = TryGetPawnOwner();
//	auto Character = Cast<AABMyCharacter>(Pawn);
//	Character->CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	Character->CharacterMovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
//	Character->EnableInput(PlayerController);
//	Character->LowThickVaulting = false;
//}
