#pragma once

#include "CoreMinimal.h"
#include "BasePlayerTool.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "WeldTool.generated.h"

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class TESTWORLD_API UWeldTool : public UBasePlayerTool
{
	GENERATED_BODY()

public:
	UWeldTool();

protected:
	FString GetName() const override { return TEXT( "Weld Tool" ); }

	void OnMouse1( bool Pressed ) override;
	void OnMouse2( bool Pressed ) override;
	void OnEnabledChanged( bool Enabled ) override;

protected:
	UPROPERTY()
	float WeldStrengthLinear;

	UPROPERTY()
	float WeldStrengthAngular;

	UPROPERTY()
	bool DisableCollision;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY( EditInstanceOnly )
	TArray<UPhysicsConstraintComponent*> welds;
};
