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
	ABLOG_S(Warning);
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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MY_Mesh(TEXT("SkeletalMesh'/Game/DWFiles/Character/MyCharacter.MyCharacter'"));
	if (MY_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MY_Mesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> MY_Anim(TEXT("AnimBlueprint'/Game/DWFiles/BlueprintClass/BP_MyCharacterAnim.BP_MyCharacterAnim_C'"));
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

	ABLOG(Warning, TEXT("%f"), CapsuleComp->GetScaledCapsuleHalfHeight());
}

void AABMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*DrawDebugCapsule(GetWorld(), GetActorLocation(), CapsuleComp->GetScaledCapsuleHalfHeight(), CapsuleComp->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Cyan, false, 1.f, 1.f);*/

	switch (VaultState)
	{
		case VaultMode::HighVaulting:
		{
			SetActorLocation(FMath::Lerp(StartingLocation, EndingLocation, VaultTick(DeltaTime)));
		}
		break;
		case VaultMode::LowThinVaulting:
		{
			 SetActorLocation(FMath::Lerp(StartingLocation, EndingLocation, VaultTick(DeltaTime)));
		}
		break;
		case VaultMode::LowThickVaulting:
		{
			SetActorLocation(FMath::Lerp(StartingLocation, EndingLocation, VaultTick(DeltaTime)));
		}
		break;
	}

	if (IsSliding)
	{
		SetActorLocation(FMath::Lerp(StartingLocation, EndingLocation, SlidingTick(DeltaTime)), true);
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
	PlayerInputComponent->BindAction(TEXT("Sliding"), EInputEvent::IE_Pressed, this, &AABMyCharacter::Sliding);
	PlayerInputComponent->BindAction(TEXT("Rifle"), EInputEvent::IE_Pressed, this, &AABMyCharacter::Rifle);
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
	VaultState = CanVault();

	switch (VaultState)
	{
		case AABMyCharacter::VaultMode::HighVaulting:
		{
			StartingLocation = GetActorLocation();
			Progress = 0.f;
			VaultEvent.ExecuteIfBound(int32(VaultMode::HighVaulting));
		}
		break;
		case AABMyCharacter::VaultMode::LowThinVaulting:
		{
			StartingLocation = GetActorLocation();
			Progress = 0.f;
			VaultEvent.ExecuteIfBound(int32(VaultMode::LowThinVaulting));
			ABLOG(Warning, TEXT("LowThinVaulting"));
		}
		break;
		case AABMyCharacter::VaultMode::LowThickVaulting:
		{
			StartingLocation = GetActorLocation();
			Progress = 0.f;
			VaultEvent.ExecuteIfBound(int32(VaultMode::LowThickVaulting));
			ABLOG(Warning, TEXT("LowThickVaulting"));
		}
		break;
		case AABMyCharacter::VaultMode::CantVault:
		{
			VaultEvent.ExecuteIfBound(int32(VaultMode::CantVault));
			bPressedJump = true;
			JumpKeyHoldTime = 0.0f;
		}
		break;
	}
}

void AABMyCharacter::Sliding()
{
	if (IsRunning && !CharacterMovementComp->IsFalling())
	{
		ABLOG(Warning, TEXT("Can Slide"));
		Progress = 0.f;
		StartingLocation = GetActorLocation();
		EndingLocation = GetActorLocation() + GetActorForwardVector() * 800.f;
		SlidingEvent.ExecuteIfBound(true);
		IsSliding = true;
		CapsuleComp->SetCapsuleHalfHeight(44.f);
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		ABLOG(Warning, TEXT("Can't Slide"));
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

void AABMyCharacter::Rifle()
{
	RifleEvent.ExecuteIfBound((++IsRifle) % 2);
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

AABMyCharacter::VaultMode AABMyCharacter::CanVault()
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

	if (bResult)
		return CanVaultToHit(HitResult);
	else 
		return VaultMode::CantVault;
}

AABMyCharacter::VaultMode AABMyCharacter::CanVaultToHit(FHitResult& HitResult)
{
	// Vault할 수 있는 높이 인지
	FVector HitLocation = HitResult.Location;
	FVector HiTTraceEnd = HitResult.TraceEnd;
	float	Height = HitLocation.Z - HiTTraceEnd.Z;

	ABLOG(Warning, TEXT("Object Height : %f"), Height);
	ABLOG(Warning, TEXT("Object InRange : %d"), UKismetMathLibrary::InRange_FloatFloat(Height, MinHighVault, MaxHighVault));

	if (UKismetMathLibrary::InRange_FloatFloat(Height, MinHighVault, MaxHighVault) == false && UKismetMathLibrary::InRange_FloatFloat(Height, MinLowVault, MaxLowVault) == false) return VaultMode::CantVault;

	// 표면이 걸을 수 있는 높이? 경사로인지
	if (CheckWalkable(HitResult.Normal.Z, CharacterMovementComp->GetWalkableFloorZ()) == false) return VaultMode::CantVault;

	// 오브젝트의 표면 위에 서있을 수 있는지
	// HighVault일 경우
	if (UKismetMathLibrary::InRange_FloatFloat(Height, MinHighVault, MaxHighVault))
	{
		CheckCapsuleCollision(FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + CapsuleComp->GetScaledCapsuleHalfHeight() + CapsuleComp->GetScaledCapsuleRadius()), CapsuleComp->GetScaledCapsuleHalfHeight(), CapsuleComp->GetScaledCapsuleRadius());
		SetEndingLocation(FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + CapsuleComp->GetScaledCapsuleHalfHeight()));
		return VaultMode::HighVaulting;
	}
	// LowVault일 경우
	if (CheckThinOrThick() == VaultMode::LowThickVaulting)
	{
		SetEndingLocation(FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + CapsuleComp->GetScaledCapsuleHalfHeight()));
		CheckCapsuleCollision(FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + CapsuleComp->GetScaledCapsuleHalfHeight() + CapsuleComp->GetScaledCapsuleRadius()), CapsuleComp->GetScaledCapsuleHalfHeight(), CapsuleComp->GetScaledCapsuleRadius());
	}
	return CheckThinOrThick();
}

void AABMyCharacter::CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius)
{
	DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, FQuat::Identity, FColor::Blue, false, 1.f, 2.f);

	// Set what actors to seek out from it's collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

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

	UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), Center, Radius, HalfHeight, traceObjectTypes, seekClass, ignoreActors, outActors);
	// CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	DisableInput(PC);
}

AABMyCharacter::VaultMode AABMyCharacter::CheckThinOrThick()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(this);

	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - ForLowVaultCheck);
	FVector End = Start + GetActorForwardVector() * LowVaultRange;

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
	// ################################################################

	FHitResult HitResultOtherSide;
	FCollisionQueryParams ParamsOtherSide(NAME_None, false, this);
	ParamsOtherSide.AddIgnoredActor(this);

	FVector StartOtherSide = HitResult.TraceEnd;
	FVector EndOtherSide = StartOtherSide + (GetActorForwardVector() * -1) * LowVaultRange;

	bool bResultOtherSide = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResultOtherSide,
		StartOtherSide, EndOtherSide,
		ECollisionChannel::ECC_GameTraceChannel1,
		ParamsOtherSide
	);
	FColor DrawColorOtherSide;
	if (bResultOtherSide)
		DrawColorOtherSide = FColor::Orange;
	else
		DrawColorOtherSide = FColor::Blue;

	DrawDebugLine(GetWorld(), StartOtherSide, EndOtherSide, DrawColorOtherSide, false, 1.f, 10, 1.f);

	FVector Depth = HitResultOtherSide.Location - HitResult.Location;
	float DepthX = FMath::Abs(Depth.X);

	ABLOG(Warning, TEXT("Low Actor's Depth : %f"), FMath::Abs(Depth.X));

	// Low Actor Thin
	if (DepthX >= 10.f && DepthX <= MinLowDepth)
	{
		FVector OriginalPos = FVector(HitResultOtherSide.Location.X, HitResultOtherSide.Location.Y, HitResultOtherSide.Location.Z + ForLowVaultCheck);

		FVector EndPos = OriginalPos + (GetActorForwardVector() * CapsuleComp->GetScaledCapsuleRadius() * 2);
		
		SetEndingLocation(EndPos);
		CheckCapsuleCollision(EndPos, CapsuleComp->GetScaledCapsuleHalfHeight(), CapsuleComp->GetScaledCapsuleRadius());

		return VaultMode::LowThinVaulting;
	}
	// Low Actor Thick
	else if (DepthX > MinLowDepth || DepthX < 10.f)
	{
		return VaultMode::LowThickVaulting;
	}
	return VaultMode::CantVault;
}

float AABMyCharacter::VaultTick(float DeltaTime)
{
	Progress += DeltaTime / VaultSpeed;
	Progress = FMath::Clamp(Progress, 0.f, 1.f);
	if (Progress >= 1.f)
	{
		if (VaultState != VaultMode::CantVault)
			SetVaultEnd();
			
	}
	return Progress;
}

float AABMyCharacter::SlidingTick(float DeltaTime)
{
	Progress += DeltaTime * SlidingSpeed;
	Progress = FMath::Clamp(Progress, 0.f, 1.f);
	if (Progress >= 1.f)
	{
		if (IsSliding)
			SetSlidingEnd();
	}
	return Progress;
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

void AABMyCharacter::SetVaultEnd()
{
	switch (VaultState)
	{
	case AABMyCharacter::VaultMode::HighVaulting: 
		{
			ABLOG(Warning, TEXT("End High Vault"));
			VaultEvent.ExecuteIfBound(int32(VaultMode::CantVault));
			VaultState = VaultMode::CantVault;
		}
		break;
	case AABMyCharacter::VaultMode::LowThinVaulting:
		{
			ABLOG(Warning, TEXT("End Low Thin Vault"));
			VaultEvent.ExecuteIfBound(int32(VaultMode::CantVault));
			VaultState = VaultMode::CantVault;
		}
		break;
	case AABMyCharacter::VaultMode::LowThickVaulting:
		{
			ABLOG(Warning, TEXT("End Low Thick Vault"));
			VaultEvent.ExecuteIfBound(int32(VaultMode::CantVault));
			VaultState = VaultMode::CantVault;
		}
		break;
	}
	/*CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CharacterMovementComp->SetMovementMode(MOVE_Walking);*/
	// CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	EnableInput(PC);
}

void AABMyCharacter::SetSlidingEnd()
{
	ABLOG(Warning, TEXT("SetSlidingEnd"));
	SlidingEvent.ExecuteIfBound(false);
	IsSliding = false;
	CapsuleComp->SetCapsuleHalfHeight(CapsuleComp->GetScaledCapsuleHalfHeight() * 2);
	CapsuleComp->SetCapsuleHalfHeight(88.f);
}

