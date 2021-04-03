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
#include "../PhysTool.h"

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	SetActorTickEnabled( true );

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	SprintSpeedMultiplier = 1.0f;
	NoclipHeightMultiplier = 1.0f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;	
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	SetRotationEnabled( true );
	SetMovementEnabled( true );
}

void AFP_FirstPersonCharacter::SetRotationEnabled( bool Enabled )
{
	Flags.set( RotationEnabled, Enabled );
}

void AFP_FirstPersonCharacter::SetMovementEnabled( bool Enabled )
{
	Flags.set( MovementEnabled, Enabled );
}

void AFP_FirstPersonCharacter::SetToolSwapEnabled( bool Enabled )
{
	Flags.set( ToolSwapEnabled, Enabled );
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast< APlayerController >( GetController() );
	check( PlayerController );

	for( int32 i = 0; i < GetCapsuleComponent()->GetNumChildrenComponents(); ++i )
	{
		if( auto* Tool = Cast< UBasePlayerTool >( GetCapsuleComponent()->GetChildComponent( i ) ) )
		{
			Tool->SetPlayerRef( *this );
			Tools.Add( Tool );
		}
	}
	 
	CurrentTool = *Tools.begin();
	CurrentToolIndex = 0;
}

void AFP_FirstPersonCharacter::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	check( PlayerInputComponent );

	DECLARE_DELEGATE_OneParam( FPressedParamDelegate, bool );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Mouse1", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouse1, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Mouse1", IE_Released, this, &AFP_FirstPersonCharacter::OnMouse1, false );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Mouse2", IE_Pressed, this, &AFP_FirstPersonCharacter::OnMouse2, true );
	PlayerInputComponent->BindAction< FPressedParamDelegate >( "Mouse2", IE_Released, this, &AFP_FirstPersonCharacter::OnMouse2, false );
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

	DECLARE_DELEGATE_OneParam( FToolParamDelegate, int32 );
	for( int32 i = 0; i < 10; ++i )
		PlayerInputComponent->BindAction< FToolParamDelegate >( *FString::Format( TEXT( "Tool{0}" ), { i + 1 } ), IE_Released, this, &AFP_FirstPersonCharacter::OnToolChange, i );
}

void AFP_FirstPersonCharacter::OnMouse1( bool pressed )
{
	CurrentTool->OnMouse1( pressed );
}

void AFP_FirstPersonCharacter::OnMouse2( bool Pressed )
{
	CurrentTool->OnMouse2( Pressed );
}

void AFP_FirstPersonCharacter::OnRotateTarget( bool Pressed )
{
	CurrentTool->OnRotateTarget( Pressed );
}

void AFP_FirstPersonCharacter::OnSprint( bool Pressed )
{
	Flags[Sprint] = Pressed;
	GetCharacterMovement()->MaxWalkSpeed *= ( Pressed ? SprintSpeedMultiplier : 1.0f / SprintSpeedMultiplier );
	GetCharacterMovement()->MaxFlySpeed *= ( Pressed ? SprintSpeedMultiplier : 1.0f / SprintSpeedMultiplier );

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

void AFP_FirstPersonCharacter::OnJumped( bool Pressed )
{
	if( !GetCharacterMovement()->IsFlying() && Flags.test( MovementEnabled ) )
		Pressed ? Jump() : StopJumping();
}

void AFP_FirstPersonCharacter::OnJumped( float Rate )
{
	if( GetCharacterMovement()->IsFlying() && Flags.test( MovementEnabled ) )
		AddMovementInput( GetActorUpVector(), Rate * NoclipHeightMultiplier );
}

void AFP_FirstPersonCharacter::OnMouseWheel( bool WheelDown )
{
	CurrentTool->OnMouseWheel( WheelDown );

	if( Flags.test( ToolSwapEnabled ) )
		OnToolChange( ( CurrentToolIndex + ( WheelDown ? -1 : 1 ) ) % Tools.Num() );
}

void AFP_FirstPersonCharacter::OnToolChange( int32 ToolIndex )
{
	if( ToolIndex >= Tools.Num() || ToolIndex == CurrentToolIndex )
		return;

	const auto Prev = CurrentToolIndex;
	CurrentToolIndex = ToolIndex;
	CurrentTool->SetEnabled( false );
	CurrentTool = Tools[CurrentToolIndex];
	CurrentTool->SetEnabled( true );
	UIToolIndexSelected( Prev, CurrentToolIndex );
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if( Value != 0.0f && Flags.test( MovementEnabled ) )
	{
		const auto direction = GetCharacterMovement()->IsFlying() ? FRotationMatrix( PlayerController->GetControlRotation() ).GetUnitAxis( EAxis::X ) : GetActorForwardVector();
		AddMovementInput( direction, Value * ( Flags.test( Sprint ) ? SprintSpeedMultiplier : 1.0f ) );
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if( Value != 0.0f && Flags.test( MovementEnabled ) )
	{
		const auto direction = GetCharacterMovement()->IsFlying() ? FRotationMatrix( PlayerController->GetControlRotation() ).GetUnitAxis( EAxis::Y ) : GetActorRightVector();
		AddMovementInput( direction, Value * ( Flags.test( Sprint ) ? SprintSpeedMultiplier : 1.0f ) );
	}
}

void AFP_FirstPersonCharacter::Turn( float Rate )
{
	CurrentTool->Turn( Rate );

	if( Flags.test( RotationEnabled ) )
	{
		AddControllerYawInput( Rate );
	}
}

void AFP_FirstPersonCharacter::LookUp( float Rate )
{
	CurrentTool->LookUp( Rate );

	if( Flags.test( RotationEnabled ) )
	{
		AddControllerPitchInput( Rate );
	}
}