// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMyCharacter.h"
#include "ABMyCharacterAnim.h"
#include "DrawDebugHelpers.h"
#include "ABPlayerController.h"

AABMyCharacter::AABMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	CapsuleComp = GetCapsuleComponent();
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	CharacterMovementComp = GetCharacterMovement();

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MY_Mesh(TEXT("SkeletalMesh'/Game/Character/MyCharacter.MyCharacter'"));
	if (MY_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MY_Mesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> MY_Anim(TEXT("AnimBlueprint'/Game/BlueprintClass/BP_MyCharacterAnim.BP_MyCharacterAnim_C'"));
	if (MY_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MY_Anim.Class);
	}

	SetControlMode(ControlMode::GTA);

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
}

void AABMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AABMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Vaulting)
	{
		FVector pos = GetActorLocation();
		pos += EndLocation * 200.f * DeltaTime;
		SetActorLocation(pos);
		return;
	}

	switch (CurrentControlMode)
	{
		case ControlMode::DAIBLO:
		{
			FRotator r = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
			SpringArm->SetRelativeRotation(r);
		}
		break;
	}

	switch (CurrentControlMode)
	{
		case ControlMode::DAIBLO:
		{
			if (DirectionToMove.SizeSquared() > 0.f)
			{
				GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
				{
					AddMovementInput(DirectionToMove);
				}
			}
		}
		break;
		case ControlMode::GTA:
		{

		}
		break;
	}
}

void AABMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABMyCharacter::Turn);


	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABMyCharacter::ViewChange); 
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AABMyCharacter::Jump); PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Pressed, this, &AABMyCharacter::PressedRun);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Released, this, &AABMyCharacter::ReleasedRun);
}

void AABMyCharacter::CheckCanVault()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(this);

	float CheckRange = 50.f;

	FVector Start = GetActorLocation() - FVector(0.f, 0.f, 25.f);
	FVector End = Start + GetActorForwardVector() * CheckRange;
	FVector Location;

	bool bResult = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		Start, End,
		ECollisionChannel::ECC_GameTraceChannel1,
		Params
	);
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1.f, 10, 1.f);

	if (bResult && IsValid(HitResult.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit actor: %s"), *HitResult.GetActor()->GetName());
	}
	// #########################################

	FHitResult HitResultHead;
	FCollisionQueryParams Params2(NAME_None, false, this);
	Params2.AddIgnoredActor(this);
	FVector StartHead = GetActorLocation() + FVector(0.f, 0.f, 40.f);
	FVector EndHead = StartHead + GetActorForwardVector() * CheckRange;
	bool bResultHead = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResultHead,
		StartHead, EndHead,
		ECollisionChannel::ECC_GameTraceChannel1,
		Params2
	);
	FColor DrawColorHead;
	if (bResultHead)
		DrawColorHead = FColor::Green;
	else
		DrawColorHead = FColor::Red;

	DrawDebugLine(GetWorld(), StartHead, EndHead, DrawColorHead, false, 1.f, 10, 1.f);


	// 물체의 높이가 낮은 경우
	if (bResult && bResultHead == false)
	{
		if (CheckThickness())
		{
			// 물체의 두깨가 두꺼운 경우
			VaultEvent.ExecuteIfBound(true);
			Vaulting = true;
			ABLOG_S(Warning);
			CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CharacterMovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
			ABLOG(Warning, TEXT("Location : %s"), *HitResult.Location.ToString());
			ABLOG(Warning, TEXT("Normal : %s"), *HitResult.Normal.ToString());
			APlayerController* Playercontroller = GetWorld()->GetFirstPlayerController();
			DisableInput(Playercontroller);
		}
		else
		{
			// 물체의 두깨가 얇은 경우
			//VaultEvent.ExecuteIfBound(false);
			//ABLOG_S(Warning);
		}
	}
	// 물체의 높이가 높은 경우
	else if (bResult && bResultHead)
	{
		if (CheckThickness())
		{
			// 두꺼운 경우

		}
		else
		{
			// 얇은 경우
		}
	}
	// 물체의 높이가 높은데 곰ㅇ중에 떠있는 경우
	else if (bResultHead)
	{
		
	}
}

bool AABMyCharacter::CheckThickness()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(this);
	float CheckRange = 130.f;

	FVector Start = GetActorLocation() + GetActorForwardVector() * CheckRange;
	Start.Z += CapsuleComp->GetScaledCapsuleHalfHeight();
	FVector End = GetActorLocation() + GetActorForwardVector() * CheckRange;
	End.Z -= CapsuleComp->GetScaledCapsuleHalfHeight();

	bool bResult = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		Start, End,
		ECollisionChannel::ECC_GameTraceChannel1,
		Params
	);
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1.f, 10, 1.f);

	ABLOG(Warning, TEXT("%s"), bResult ? TEXT("Thick!") : TEXT("Thin!"));
	if (bResult)
	{
		EndLocation = HitResult.Location;
		EndLocation = EndLocation.GetSafeNormal();
	}
	return bResult;
}

void AABMyCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			GetController()->SetControlRotation(GetActorRotation());
			SetControlMode(ControlMode::DAIBLO);
		}
		break;
		case ControlMode::DAIBLO:
		{
			GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
			SetControlMode(ControlMode::GTA);
		}
		break;
	}
}

void AABMyCharacter::Jump()
{
	CheckCanVault();
}

void AABMyCharacter::PressedRun()
{
	RunEvent.ExecuteIfBound(true);
	IsRunning = true;
}

void AABMyCharacter::ReleasedRun()
{
	RunEvent.ExecuteIfBound(false);
	IsRunning = false;
}

void AABMyCharacter::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			if (IsRunning)
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue);
			}
			else
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue / 2.f);
			}
		}
		break;
		case ControlMode::DAIBLO:
		{
			if (IsRunning)
			{
				DirectionToMove.X = NewAxisValue;
			}
			else
			{
				DirectionToMove.X = NewAxisValue / 2.f;
			}
		}
		break;
	}
}

void AABMyCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			if (IsRunning)
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
			}
			else
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue / 2.f);
			}
		}
		break;
		case ControlMode::DAIBLO:
		{
			if (IsRunning)
			{
				DirectionToMove.Y = NewAxisValue;
			}
			else
			{
				DirectionToMove.Y = NewAxisValue / 2.f;
			}
		}
		break;
	}
}

void AABMyCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AABMyCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AABMyCharacter::SetControlMode(ControlMode ControlMode)
{
	CurrentControlMode = ControlMode;

	switch (CurrentControlMode)
	{
		case ControlMode::DAIBLO:
		{
			ArmLengthTo = 800.f;
			ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
			SpringArm->bUsePawnControlRotation = false;
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritRoll = false;
			SpringArm->bInheritYaw = false;
			SpringArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		}
		break;
	case ControlMode::GTA:
	{
		ArmLengthTo = 450.f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	}
	break;
	}
}

