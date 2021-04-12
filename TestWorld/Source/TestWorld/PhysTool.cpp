#include "PhysTool.h"

#include "Kismet/KismetMathLibrary.h"
#include "Utility.h"

UPhysTool::UPhysTool()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	RotationLockAngle = 22.5f;
	ZoomSpeed = 1.0f;
	ObjectRotateSpeedYaw = 0.1f;
	ObjectRotateSpeedPitch = 0.1f;
}

void UPhysTool::OnMouse1_Implementation( bool pressed )
{
	if( !pressed && TargetActor )
	{
		ReleaseTarget();
	}
	else if( pressed && !TargetActor )
	{
		// Play a sound if there is one
		//if( FireSound != nullptr )
		//{
		//	UGameplayStatics::PlaySoundAtLocation( this, FireSound, GetActorLocation() );
		//}
		//
		//// Try and play a firing animation if specified
		//if( FireAnimation != nullptr )
		//{
		//	// Get the animation object for the arms mesh
		//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		//	if( AnimInstance != nullptr )
		//	{
		//		AnimInstance->Montage_Play( FireAnimation, 1.f );
		//	}
		//}

		const FHitResult Impact = WeaponTrace();

		if( Impact.GetActor() && Impact.GetComponent() && Impact.GetActor()->IsRootComponentMovable() )
		{
			TargetActor = Impact.GetActor();
			Impact.GetComponent()->SetSimulatePhysics( false );
			TargetLocationOffset = FVector::ZeroVector;
			TargetDistance = ( TargetActor->GetTransform().GetLocation() - Player->GetTransform().GetLocation() ).Size();
			TargetLocationOffset = Player->GetPlayerController()->GetControlRotation().UnrotateVector( TargetActor->GetActorLocation() - GetTargetLockLocation() );
			targetRotationOffset = TargetActor->GetActorRotation() - Player->GetActorRotation();

			ReflexOutput( ReflexFormat( "Mouse1 Pressed, target: {0}", TargetActor->GetName() ) );
		}
	}
}

void UPhysTool::OnMouse2_Implementation( bool Pressed )
{
	if( TargetActor )
		ReflexOutput( FString( TEXT( "Mouse2 Pressed - Freeze target" ) ) );

	TargetActor = nullptr;
}

void UPhysTool::OnRotateTarget_Implementation( bool Pressed )
{
	RotatingTarget = Pressed;
	ReflexOutput( FString( RotatingTarget ? TEXT( "Rotate target Enabled" ) : TEXT( "Rotate target Disabled" ) ) );
}

void UPhysTool::OnRotateTargetAxis_Implementation( bool Pressed )
{
	RotateAxisAligned = Pressed;
	ReflexOutput( FString( RotateAxisAligned ? TEXT( "Rotate target axis aligned Enabled" ) : TEXT( "Rotate target axis aligned Disabled" ) ) );
}

void UPhysTool::OnMouseWheel_Implementation( bool wheel_down )
{
	TargetDistance = FMath::Max( 0.0f, TargetDistance + GetWorld()->GetDeltaSeconds() * ZoomSpeed * ( wheel_down ? -1.0f : 1.0f ) );
}

void UPhysTool::Turn_Implementation( float Rate )
{
	if( RotatingTarget && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - Player->GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( z, Rate * ObjectRotateSpeedYaw ) * ( Player->GetActorRotation() + targetRotationOffset ).Quaternion();
		targetRotationOffset = target_rotation.Rotator() - Player->GetActorRotation();
	}
}

void UPhysTool::LookUp_Implementation( float Rate )
{
	if( RotatingTarget && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - Player->GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( y, -Rate * ObjectRotateSpeedPitch ) * ( Player->GetActorRotation() + targetRotationOffset ).Quaternion();
		targetRotationOffset = target_rotation.Rotator() - Player->GetActorRotation();
	}
}

void UPhysTool::OnEnabledChanged_Implementation( bool Enabled )
{
	if( !Enabled && TargetActor )
	{
		ReleaseTarget();
	}
}

void UPhysTool::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( TargetActor )
	{
		TargetActor->SetActorLocation( GetTargetLockLocation(), true );
		TargetActor->SetActorRotation( GetTargetLockRotation() );
	}

	Player->SetRotationEnabled( !( RotatingTarget && TargetActor ) );
}

FVector UPhysTool::GetTargetLockLocation() const
{
	const auto direction = FRotationMatrix( Player->GetPlayerController()->GetControlRotation() ).GetUnitAxis( EAxis::X );
	const auto target_loc = Player->GetTransform().GetLocation() + direction * TargetDistance + Player->GetPlayerController()->GetControlRotation().RotateVector( TargetLocationOffset );
	return target_loc;
}

FRotator UPhysTool::GetTargetLockRotation()
{
	auto rotation_offset = targetRotationOffset;

	if( RotateAxisAligned )
	{
		auto euler = rotation_offset.Euler();
		euler.X = int( ( euler.X + 360 ) / RotationLockAngle + 0.5f ) * RotationLockAngle;
		euler.Y = int( ( euler.Y + 360 ) / RotationLockAngle + 0.5f ) * RotationLockAngle;
		euler.Z = int( ( euler.Z + 360 ) / RotationLockAngle + 0.5f ) * RotationLockAngle;
		rotation_offset = FRotator::MakeFromEuler( euler );
	}

	return Player->GetActorRotation() + rotation_offset;
}

void UPhysTool::ReleaseTarget()
{
	ReflexOutput( FString( TEXT( "Target Released" ) ) );
	Cast< UPrimitiveComponent >( TargetActor->GetRootComponent() )->SetSimulatePhysics( true );
	TargetActor = nullptr;
}
