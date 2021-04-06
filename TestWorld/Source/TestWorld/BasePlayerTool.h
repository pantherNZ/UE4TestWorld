#pragma once

#include "CoreMinimal.h"
#include "FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "BasePlayerTool.generated.h"

class AFP_FirstPersonCharacter;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class TESTWORLD_API UBasePlayerTool : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UBasePlayerTool();

	void SetPlayerRef( AFP_FirstPersonCharacter& Player );
	void SetEnabled( bool Enabled );

	UFUNCTION( BlueprintCallable )
	virtual FString GetName() const PURE_VIRTUAL( UBasePlayerTool::GetName, return {}; );

	virtual void OnEnabledChanged( bool Enabled ) { }
	virtual void OnMouse1( bool Pressed ) { }
	virtual void OnMouse2( bool Pressed ) { }
	virtual void OnMouseWheel( bool WheelDown ) { }
	virtual void OnRotateTarget( bool Pressed ) { }
	virtual void OnRotateTargetAxis( bool Pressed ) { }
	virtual void Turn( float Rate ) { }
	virtual void LookUp( float Rate ) { }

protected:
	virtual void BeginPlay() override;
	FHitResult WeaponTrace( float TraceDistance = 1000.0f ) const;

public:	

protected:
	UPROPERTY()
	bool IsEnabled;

	UPROPERTY()
	AFP_FirstPersonCharacter* Player;
};
