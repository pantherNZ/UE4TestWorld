#include "BasePlayerTool.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

UBasePlayerTool::UBasePlayerTool()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBasePlayerTool::SetPlayerRef( AFP_FirstPersonCharacter& _Player )
{
	this->Player = &_Player;
}

void UBasePlayerTool::SetEnabled( bool _IsEnabled )
{
	this->IsEnabled = _IsEnabled;
}

void UBasePlayerTool::BeginPlay()
{
	Super::BeginPlay();

}

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
