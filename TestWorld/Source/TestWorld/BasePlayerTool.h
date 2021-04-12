#pragma once

#include "CoreMinimal.h"
#include "FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "BasePlayerTool.generated.h"

class AFP_FirstPersonCharacter;

UCLASS( BlueprintType, Blueprintable )
class TESTWORLD_API UBasePlayerTool : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UBasePlayerTool();

	void SetPlayerRef( AFP_FirstPersonCharacter& Player );
	void SetEnabled( bool Enabled );

	UFUNCTION( BlueprintCallable )
	virtual FString GetName() const PURE_VIRTUAL( UBasePlayerTool::GetName, return {}; );

	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnEnabledChanged( bool Enabled );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnMouse1( bool Pressed );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnMouse2( bool Pressed );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnMouseWheel( bool WheelDown );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnRotateTarget( bool Pressed );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void OnRotateTargetAxis( bool Pressed );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void Turn( float Rate );
	UFUNCTION( BlueprintNativeEvent, Category = "Tool" )
	void LookUp( float Rate );

protected:
	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable )
	FHitResult WeaponTrace( float TraceDistance = 1000.0f ) const;

public:	

protected:
	UPROPERTY()
	bool IsEnabled;

	UPROPERTY()
	AFP_FirstPersonCharacter* Player;
};
