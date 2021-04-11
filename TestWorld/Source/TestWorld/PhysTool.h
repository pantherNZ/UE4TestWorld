#pragma once

#include "CoreMinimal.h"
#include "BasePlayerTool.h"
#include "PhysTool.generated.h"

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class TESTWORLD_API UPhysTool : public UBasePlayerTool
{
	GENERATED_BODY()
	
public:
	UPhysTool();

protected:
	FString GetName() const override { return TEXT( "Phys Tool" ); }

	void OnMouse1( bool Pressed ) override;
	void OnMouse2( bool Pressed ) override;
	void OnMouseWheel( bool WheelDown ) override;
	void OnRotateTarget( bool Pressed ) override;
	void OnRotateTargetAxis( bool Pressed ) override;
	void Turn( float Rate ) override;
	void LookUp( float Rate ) override;
	void OnEnabledChanged( bool Enabled ) override;

	void ReleaseTarget();
	void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	FVector GetTargetLockLocation() const;
	FRotator GetTargetLockRotation();

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float ZoomSpeed;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float ObjectRotateSpeedYaw;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float ObjectRotateSpeedPitch;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float RotationLockAngle;

private:
	bool RotatingTarget;
	bool RotateAxisAligned;

	UPROPERTY()
	AActor* TargetActor;

	FVector TargetLocationOffset;
	FRotator targetRotationOffset;
	float TargetDistance;
};
