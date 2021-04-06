#include "PhysTool.h"

#include "Kismet/KismetMathLibrary.h"
#include "Utility.h"

UPhysTool::UPhysTool()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	ZoomSpeed = 1.0f;
	ObjectRotateSpeedYaw = 0.1f;
	ObjectRotateSpeedPitch = 0.1f;
}

void UPhysTool::OnMouse1( bool pressed )
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

void UPhysTool::OnMouse2( bool Pressed )
{
	if( TargetActor )
		ReflexOutput( FString( TEXT( "Mouse2 Pressed - Freeze target" ) ) );

	TargetActor = nullptr;
}

void UPhysTool::OnRotateTarget( bool Pressed )
{
	RotatingTarget = Pressed;
	ReflexOutput( FString( RotatingTarget ? TEXT( "Rotate target Enabled" ) : TEXT( "Rotate target Disabled" ) ) );
}

void UPhysTool::OnRotateTargetAxis( bool Pressed )
{
	RotateAxisAligned = RotatingTarget = Pressed;
	ReflexOutput( FString( RotatingTarget ? TEXT( "Rotate target axis aligned Enabled" ) : TEXT( "Rotate target axis aligned Disabled" ) ) );
}

void UPhysTool::OnMouseWheel( bool wheel_down )
{
	TargetDistance = FMath::Max( 0.0f, TargetDistance + GetWorld()->GetDeltaSeconds() * ZoomSpeed * ( wheel_down ? -1.0f : 1.0f ) );
}

void UPhysTool::Turn( float Rate )
{
	if( RotatingTarget && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - Player->GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( z, Rate * ObjectRotateSpeedYaw ) * ( Player->GetActorRotation() + targetRotationOffset ).Quaternion();
		SetTargetRotationOffset( target_rotation.Rotator() );
	}
}

void UPhysTool::LookUp( float Rate )
{
	if( RotatingTarget && TargetActor )
	{
		const auto rotation = ( TargetActor->GetActorLocation() - Player->GetActorLocation() ).Rotation();
		FVector x, y, z;
		UKismetMathLibrary::GetAxes( rotation, x, y, z );
		const auto target_rotation = FQuat( y, -Rate * ObjectRotateSpeedPitch ) * ( Player->GetActorRotation() + targetRotationOffset ).Quaternion();
		SetTargetRotationOffset( target_rotation.Rotator() );
	}
}

void UPhysTool::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( TargetActor )
	{
		TargetActor->SetActorLocation( GetTargetLockLocation(), true );
		TargetActor->SetActorRotation( Player->GetActorRotation() + targetRotationOffset );
	}

	Player->SetRotationEnabled( !( RotatingTarget && TargetActor ) );
}

FVector UPhysTool::GetTargetLockLocation() const
{
	const auto direction = FRotationMatrix( Player->GetPlayerController()->GetControlRotation() ).GetUnitAxis( EAxis::X );
	const auto target_loc = Player->GetTransform().GetLocation() + direction * TargetDistance + Player->GetPlayerController()->GetControlRotation().RotateVector( TargetLocationOffset );
	return target_loc;
}

void UPhysTool::SetTargetRotationOffset( FRotator& target_rotation )
{
	targetRotationOffset = target_rotation - Player->GetActorRotation();

	if( RotateAxisAligned )
	{
		auto euler = targetRotationOffset.Euler();
		const auto LockValue = FMath::DegreesToRadians( 45.0f );
		euler.X = int( euler.X / LockValue + 0.5f ) * LockValue;
		euler.Y = int( euler.Y / LockValue + 0.5f ) * LockValue;
		euler.Z = int( euler.Z / LockValue + 0.5f ) * LockValue;
		targetRotationOffset = FRotator::MakeFromEuler( euler );
	}
}

void UPhysTool::ReleaseTarget()
{
	ReflexOutput( FString( TEXT( "Target Released" ) ) );
	Cast< UPrimitiveComponent >( TargetActor->GetRootComponent() )->SetSimulatePhysics( true );
	TargetActor = nullptr;
}

void UPhysTool::OnEnabledChanged( bool Enabled )
{
	if( !Enabled && TargetActor )
	{
		ReleaseTarget();
	}
}
