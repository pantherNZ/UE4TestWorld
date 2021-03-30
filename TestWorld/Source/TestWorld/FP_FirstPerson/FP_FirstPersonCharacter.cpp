 // Copyright Epic Games, Inc. All Rights Reserved.

#include "FP_FirstPersonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "..//Utility.h"
#include "GameFramework/CharacterMovementComponent.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFP_FirstPersonCharacter

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	SetActorTickEnabled( true );

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	TraceDistance = 1000.0f;
	ZoomSpeed = 1.0f;
	ObjectRotateSpeedYaw = 0.1f;
	ObjectRotateSpeedPitch = 0.1f;
	SprintSpeedMultiplier = 1.0f;
	NoclipHeightMultiplier = 1.0f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);				// Set so only owner can see mesh
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);	// Attach mesh to FirstPersonCameraComponent
	Mesh1P->bCastDynamicShadow = false;			// Disallow mesh to cast dynamic shadows
	Mesh1P->CastShadow = false;				// Disallow mesh to cast other shadows

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// Only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	FP_Gun->CastShadow = false;			// Disallow mesh to cast other shadows
	FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFP_FirstPersonCharacter::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	check( PlayerInputComponent );


	DECLARE_DELEGATE_OneParam( FPressedParamDelegate, bool );
	PlayerInputComponent->BindAction< FPressedParamDelegate>( "Mouse1", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouse1, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate>( "Mouse1", IE_Released, this, &AFP_FirstPersonCharacter::OnMouse1, false );
	PlayerInputComponent->BindAction( "Mouse2", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouse2 );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "MouseWheelDown", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouseWheel, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "MouseWheelUp", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouseWheel, false );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "RotateTarget", IE_Pressed, this, &AFP_FirstPersonCharacter::OnRotateTarget, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "RotateTarget", IE_Released, this, &AFP_FirstPersonCharacter::OnRotateTarget, false );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Sprint", IE_Pressed, this, &AFP_FirstPersonCharacter::OnSprint, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Sprint", IE_Released, this, &AFP_FirstPersonCharacter::OnSprint, false );
	PlayerInputComponent->BindAction( "Noclip", IE_Pressed, this, &AFP_FirstPersonCharacter::OnNoclip );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Jump", IE_Pressed, this, &AFP_FirstPersonCharacter::OnJumped, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Jump", IE_Released, this, &AFP_FirstPersonCharacter::OnJumped, false );

	PlayerInputComponent->BindAxis( "MoveForward", this, &AFP_FirstPersonCharacter::MoveForward );
	PlayerInputComponent->BindAxis( "MoveRight", this, &AFP_FirstPersonCharacter::MoveRight );
	PlayerInputComponent->BindAxis( "Turn", this, &AFP_FirstPersonCharacter::Turn );
	PlayerInputComponent->BindAxis( "LookUp", this, &AFP_FirstPersonCharacter::LookUp );
	PlayerInputComponent->BindAxis( "Jump", this, &AFP_FirstPersonCharacter::OnJumped );
}

void AFP_FirstPersonCharacter::OnMouse1( bool pressed )
{
	if( !pressed && TargetActor )
	{
		ReflexOutput( FString( TEXT( "Mouse1 Released" ) ) );
		Cast< UPrimitiveComponent >( TargetActor->GetRootComponent() )->SetSimulatePhysics( true );
		TargetActor = nullptr;
	}
	else if( pressed && !TargetActor )
	{
		// Play a sound if there is one
		if( FireSound != nullptr )
		{
			UGameplayStatics::PlaySoundAtLocation( this, FireSound, GetActorLocation() );
		}

		// Try and play a firing animation if specified
		if( FireAnimation != nullptr )
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if( AnimInstance != nullptr )
			{
				AnimInstance->Montage_Play( FireAnimation, 1.f );
			}
		}

		FVector ShootDir = FVector::ZeroVector;
		FVector StartTrace = FVector::ZeroVector;

		if( PlayerController )
		{
			// Calculate the direction of fire and the start location for trace
			FRotator CamRot;
			PlayerController->GetPlayerViewPoint( StartTrace, CamRot );
			ShootDir = CamRot.Vector();

			// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
			StartTrace = StartTrace + ShootDir * ( ( GetActorLocation() - StartTrace ) | ShootDir );
		}

		// Calculate endpoint of trace
		const FVector EndTrace = StartTrace + ShootDir * TraceDistance;
		const FHitResult Impact = WeaponTrace( StartTrace, EndTrace );
		AActor* DamagedActor = Impact.GetActor();
		UPrimitiveComponent* DamagedComponent = Impact.GetComponent();

		// If we hit an actor, with a component that is simulating physics, apply an impulse
		if( DamagedActor && DamagedActor && DamagedComponent && DamagedActor->IsRootComponentMovable() )
		{
			ReflexOutput( ReflexFormat( "Mouse1 Pressed, target: {0}", DamagedActor->GetName() ) );

			TargetActor = DamagedActor;
			DamagedComponent->SetSimulatePhysics( false );
			TargetLocationOffset = FVector::ZeroVector;
			TargetDistance = ( TargetActor->GetTransform().GetLocation() - GetTransform().GetLocation() ).Size();
			TargetLocationOffset = PlayerController->GetControlRotation().UnrotateVector( TargetActor->GetActorLocation() - GetTargetLockLocation() );
			targetRotationOffset = TargetActor->GetActorRotation() - GetActorRotation();
		}
	}
}

FHitResult AFP_FirstPersonCharacter::WeaponTrace( const FVector& StartTrace, const FVector& EndTrace ) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams( SCENE_QUERY_STAT( WeaponTrace ), true, GetInstigator() );
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit( ForceInit );
	GetWorld()->LineTraceSingleByChannel( Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams );

	return Hit;
}

void AFP_FirstPersonCharacter::OnMouse2()
{
	if( TargetActor )
		ReflexOutput( FString( TEXT( "Mouse2 Pressed - Freeze target" ) ) );

	TargetActor = nullptr;
}

void AFP_FirstPersonCharacter::OnRotateTarget( bool pressed )
{
	Flags[RotatingTarget] = pressed;
	
	ReflexOutput( FString( Flags[RotatingTarget] ? TEXT( "Rotate target Enabled" ) : TEXT( "Rotate target Disabled" ) ) );
}

void AFP_FirstPersonCharacter::OnSprint( bool pressed )
{
	Flags[Sprint] = pressed;
	GetCharacterMovement()->MaxWalkSpeed *= ( pressed ? SprintSpeedMultiplier : 1.0f / SprintSpeedMultiplier );
	GetCharacterMovement()->MaxFlySpeed *= ( pressed ? SprintSpeedMultiplier : 1.0f / SprintSpeedMultiplier );

	ReflexOutput( FString( Flags[Sprint] ? TEXT( "Shift Enabled" ) : TEXT( "Shift Disabled" ) ) );
}

void AFP_FirstPersonCharacter::OnNoclip()
{
	GetCharacterMovement()->SetMovementMode( !GetCharacterMovement()->IsFlying() ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_Walking );
	GetCapsuleComponent()->SetCollisionEnabled( GetCharacterMovement()->IsFlying() ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics );
	//GetCharacterMovement()->GravityScale = Flags[Noclip] ? 0.0f : 1.0f;
	GetCharacterMovement()->bCheatFlying = GetCharacterMovement()->IsFlying();

	ReflexOutput( FString( GetCharacterMovement()->IsFlying() ? TEXT( "Noclip Enabled" ) : TEXT( "Noclip Disabled" ) ) );
}

void AFP_FirstPersonCharacter::OnJumped( bool pressed )
{
	if( !GetCharacterMovement()->IsFlying() )
		pressed ? Jump() : StopJumping();
}

void AFP_FirstPersonCharacter::OnJumped( float Rate )
{
	if( GetCharacterMovement()->IsFlying() )
		AddMovementInput( GetActorUpVector(), Rate * NoclipHeightMultiplier );
}

void AFP_FirstPersonCharacter::OnMouseWheel( bool wheel_down )
{
	TargetDistance = FMath::Max( 0.0f, TargetDistance + GetWorld()->GetDeltaSeconds() * ZoomSpeed * ( wheel_down ? -1.0f : 1.0f ) );
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if( Value != 0.0f )
	{
		const auto direction = GetCharacterMovement()->IsFlying() ? FRotationMatrix( PlayerController->GetControlRotation() ).GetUnitAxis( EAxis::X ) : GetActorForwardVector();
		AddMovementInput( direction, Value * ( Flags.test( Sprint ) ? SprintSpeedMultiplier : 1.0f ) );
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if( Value != 0.0f )
	{
		const auto direction = GetCharacterMovement()->IsFlying() ? FRotationMatrix( PlayerController->GetControlRotation() ).GetUnitAxis( EAxis::Y ) : GetActorRightVector();
		AddMovementInput( direction, Value * ( Flags.test( Sprint ) ? SprintSpeedMultiplier : 1.0f ) );
	}
}

void AFP_FirstPersonCharacter::Turn( float Rate )
{
	if( Flags.test( RotatingTarget ) && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( z, Rate * ObjectRotateSpeedYaw ) * ( GetActorRotation() + targetRotationOffset ).Quaternion();
		targetRotationOffset = target_rotation.Rotator() - GetActorRotation();
	}
	else
	{
		AddControllerYawInput( Rate );
	}
}

void AFP_FirstPersonCharacter::LookUp( float Rate )
{
	if( Flags.test( RotatingTarget ) && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( y, Rate * ObjectRotateSpeedPitch ) * ( GetActorRotation() + targetRotationOffset ).Quaternion();
		targetRotationOffset = target_rotation.Rotator() - GetActorRotation();
	}
	else
	{
		AddControllerPitchInput( Rate );
	}
}

FVector AFP_FirstPersonCharacter::GetTargetLockLocation() const
{
	const auto direction = FRotationMatrix( PlayerController->GetControlRotation() ).GetUnitAxis( EAxis::X );
	const auto target_loc = GetTransform().GetLocation() + direction * TargetDistance + PlayerController->GetControlRotation().RotateVector( TargetLocationOffset );
	return target_loc;
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast< APlayerController >( GetController() );
	check( PlayerController );
}

void AFP_FirstPersonCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( TargetActor )
	{
		TargetActor->SetActorLocation( GetTargetLockLocation(), true );
		TargetActor->SetActorRotation( GetActorRotation() + targetRotationOffset );
	}
}