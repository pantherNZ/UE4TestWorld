#include "BasePlayerTool.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

UBasePlayerTool::UBasePlayerTool()
{
	PrimaryComponentTick.bCanEverTick = false;
	DisableCollision = true;
}

void UBasePlayerTool::SetPlayerRef( AFP_FirstPersonCharacter& _Player )
{
	this->Player = &_Player;
}

void UBasePlayerTool::SetEnabled( bool _IsEnabled )
{
	if( IsEnabled != _IsEnabled )
	{
		IsEnabled = _IsEnabled;
		OnEnabledChanged( IsEnabled );
	}
}

void UBasePlayerTool::BeginPlay()
{
	Super::BeginPlay();

}

void UBasePlayerTool::OnEnabledChanged_Implementation( bool Enabled ) { }
void UBasePlayerTool::OnMouse1_Implementation( bool Pressed ) { }
void UBasePlayerTool::OnMouse2_Implementation( bool Pressed ) { }
void UBasePlayerTool::OnMouseWheel_Implementation( bool WheelDown ) { }
void UBasePlayerTool::OnRotateTarget_Implementation( bool Pressed ) { }
void UBasePlayerTool::OnRotateTargetAxis_Implementation( bool Pressed ) { }
void UBasePlayerTool::Turn_Implementation( float Rate ) { }
void UBasePlayerTool::LookUp_Implementation( float Rate ) { }

FHitResult UBasePlayerTool::WeaponTrace( float TraceDistance ) const
{
	FVector ShootDir = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;

	// Calculate the direction of fire and the start location for trace
	FRotator CamRot;
	Player->GetPlayerController()->GetPlayerViewPoint( StartTrace, CamRot );
	ShootDir = CamRot.Vector();

	// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
	StartTrace = StartTrace + ShootDir * ( ( Player->GetActorLocation() - StartTrace ) | ShootDir );

	// Calculate endpoint of trace
	const FVector EndTrace = StartTrace + ShootDir * TraceDistance;

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams( SCENE_QUERY_STAT( WeaponTrace ), true, Player->GetInstigator() );
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit( ForceInit );
	GetWorld()->LineTraceSingleByChannel( Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams );

	return Hit;
}
