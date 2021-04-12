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

	void OnMouse1_Implementation( bool Pressed ) override;
	void OnMouse2_Implementation( bool Pressed ) override;
	void OnMouseWheel_Implementation( bool WheelDown ) override;
	void OnRotateTarget_Implementation( bool Pressed ) override;
	void OnRotateTargetAxis_Implementation( bool Pressed ) override;
	void Turn_Implementation( float Rate ) override;
	void LookUp_Implementation( float Rate ) override;
	void OnEnabledChanged_Implementation( bool Enabled ) override;

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
