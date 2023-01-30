// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMyCharacter.h"
#include "ABMyCharacterAnim.h"
#include "DrawDebugHelpers.h"
#include "ABPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

	if (IsVaulting)
	{
		VaultTick(DeltaTime);
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

//void AABMyCharacter::CheckCanLowVault()
//{
//	FHitResult HitResult;
//	FCollisionQueryParams Params(NAME_None, false, this);
//	Params.AddIgnoredActor(this);
//
//	float CheckRange = 40.f;
//
//	FVector Start = GetActorLocation() - FVector(0.f, 0.f, 25.f);
//	FVector End = Start + GetActorForwardVector() * CheckRange;
//	FVector Location;
//
//	bool bResult = GetWorld()->LineTraceSingleByChannel
//	(
//		OUT HitResult,
//		Start, End,
//		ECollisionChannel::ECC_GameTraceChannel1,
//		Params
//	);
//	FColor DrawColor;
//	if (bResult)
//		DrawColor = FColor::Green;
//	else
//		DrawColor = FColor::Red;
//
//	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1.f, 10, 1.f);
//
//	if (bResult && IsValid(HitResult.GetActor()))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Trace hit actor: %s"), *HitResult.GetActor()->GetName());
//	}
//	float MinHeight = HitResult.Location.Z;
//	float MaxHeight = HitResult.Location.Z + 20.f;
//	
//	// 물체의 높이가 낮은 경우
//	if (bResult)
//	{
//		FVector WallHeightInfo = CheckWallHeight();
//		if (WallHeightInfo != FVector::ZeroVector && CheckMinMax(MinHeight, MaxHeight, WallHeightInfo.Z))
//		{
//			// 물체의 높이가 낮고 두꺼운 경우
//			if (CheckWallThickness())
//			{
//				ABLOG(Warning, TEXT("Low and Thick"));
//				LowThickVaultEvent.ExecuteIfBound(true);
//				LowThickVaulting = true;
//				CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//				CharacterMovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
//				APlayerController* Playercontroller = GetWorld()->GetFirstPlayerController();
//				DisableInput(Playercontroller);
//
//				// FRotationMatrix::MakeFromX(HitResult.Location);
//				LowThickVaultDestDir = WallHeightInfo - HitResult.Location;
//				LowThickVaultDestDir.Normalize();
//			}
//			// 물체의 높이가 낮고 얇은 경우
//			else
//			{
//				ABLOG(Warning, TEXT("Low and Thin"));
//				LowThinVaultEvent.ExecuteIfBound(true);
//				LowThinVaulting = true;
//				CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//				CharacterMovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
//				APlayerController* Playercontroller = GetWorld()->GetFirstPlayerController();
//				DisableInput(Playercontroller);
//			}
//		}
//		else
//		{
//			ABLOG(Warning, TEXT("Can't Low Vault!"))
//		}
//	}
//}
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
	IsVaulting = CanVault();

	if (IsVaulting)
	{
		VaultHighEvent.ExecuteIfBound(true);
	}
	else if (!IsVaulting)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}
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

bool AABMyCharacter::CanVault()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(this);

	FVector ForwardVector = GetActorLocation() + GetActorForwardVector() * 50.f;
	float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	FVector Start = FVector(ForwardVector.X, ForwardVector.Y, ForwardVector.Z + CapsuleHalfHeight);
	FVector End = FVector(ForwardVector.X, ForwardVector.Y, ForwardVector.Z - CapsuleHalfHeight);
	
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
	
	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1.f);

	return CanVaultToHit(HitResult);
}

bool AABMyCharacter::CanVaultToHit(FHitResult& HitResult)
{
	// Vault할 수 있는 높이지
	FVector HitLocation = HitResult.Location;
	FVector HiTTraceEnd = HitResult.TraceEnd;
	float	Height = HitLocation.Z - HiTTraceEnd.Z;

	ABLOG(Warning, TEXT("Object Height : %f"), Height);
	ABLOG(Warning, TEXT("Object InRange : %d"), UKismetMathLibrary::InRange_FloatFloat(Height, MinHighVault, MaxHighVault));
	if (UKismetMathLibrary::InRange_FloatFloat(Height, MinHighVault, MaxHighVault) == false) return false;

	// 표면이 걸을 수 있는 높이? 경사로인지
	if (CheckWalkable(HitResult.Normal.Z, CharacterMovementComp->GetWalkableFloorZ()) == false) return false;

	// 오브젝트의 표면 위에 서있을 수 있는지
	SetEndingLocation(FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + CapsuleComp->GetScaledCapsuleHalfHeight()));
	CheckCapsuleCollision(EndingLocation, CapsuleComp->GetScaledCapsuleHalfHeight(), CapsuleComp->GetScaledCapsuleRadius());
	return true;
}

void AABMyCharacter::CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius)
{
	DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, FQuat::Identity, FColor::Blue, false, 1.f);

	// Set what actors to seek out from it's collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_EngineTraceChannel1));

	// Ignore any specific actors
	TArray<AActor*> ignoreActors;
	// Ignore self or remove this line to not ignore any
	ignoreActors.Init(this, 1);

	// Array of actors that are inside the radius of the sphere
	TArray<AActor*> outActors;

	// Total radius of the sphere
	float radius = 750.0f;
	// Sphere's spawn loccation within the world
	FVector sphereSpawnLocation = GetActorLocation();
	// Class that the sphere should hit against and include in the outActors array (Can be null)
	UClass* seekClass = AABMyCharacter::StaticClass(); // NULL;

	UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), Center, Radius + 30, HalfHeight, traceObjectTypes, seekClass, ignoreActors, outActors);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	DisableInput(PC);
}

void AABMyCharacter::VaultTick(float DeltaTime)
{
	Progress += DeltaTime / VaultSpeed;
	Progress = FMath::Clamp(Progress, 0.f, 1.f);
	SetActorLocation(FMath::Lerp(GetActorLocation(), EndingLocation, Progress));
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

void AABMyCharacter::SetHighVaultEnd()
{
	Progress = 0.f;
	IsVaulting = false;
	/*CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CharacterMovementComp->SetMovementMode(MOVE_Walking);*/
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	EnableInput(PC);
}

