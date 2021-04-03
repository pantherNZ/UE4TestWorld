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

FHitResult UBasePlayerTool::WeaponTrace( const FVector& StartTrace, const FVector& EndTrace ) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams( SCENE_QUERY_STAT( WeaponTrace ), true, Player->GetInstigator() );
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit( ForceInit );
	GetWorld()->LineTraceSingleByChannel( Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams );

	return Hit;
}
