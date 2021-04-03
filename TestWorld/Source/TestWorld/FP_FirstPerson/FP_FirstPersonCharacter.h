// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include <bitset>

#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;
class UBasePlayerTool;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_FirstPersonCharacter();

	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void SetRotationEnabled( bool Enabled );
	void SetMovementEnabled( bool Enabled );
	void SetToolSwapEnabled( bool Enabled );

	APlayerController* GetPlayerController() { return PlayerController; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent( class UInputComponent* InputComponent ) override;

	void OnMouse1( bool Pressed );
	void OnMouse2( bool Pressed );
	void OnMouseWheel( bool WheelDown );
	void OnRotateTarget( bool Pressed );
	void OnSprint( bool Pressed );
	void OnNoclip();
	void OnJumped( bool Pressed );
	void OnJumped( float Rate );
	void MoveForward( float Val );
	void MoveRight(float Val);
	void Turn(float Rate);
	void LookUp(float Rate);
	void OnToolChange( int32 ToolIndex );

	UFUNCTION( BlueprintImplementableEvent )
	void UIToolIndexSelected( int32 PreviousIndex, int32 NewIndex );

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	USoundBase* FireSound;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	UAnimMontage* FireAnimation;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float SprintSpeedMultiplier;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float NoclipHeightMultiplier;

private:
	UPROPERTY( VisibleDefaultsOnly, Category = Components )
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY( VisibleDefaultsOnly, Category = Components )
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = ( AllowPrivateAccess = "true" ) )
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	TArray< UBasePlayerTool* > Tools;

	UPROPERTY()
	UBasePlayerTool* CurrentTool;

	UPROPERTY()
	int32 CurrentToolIndex;

	enum EFlags
	{
		RotationEnabled,
		MovementEnabled,
		ToolSwapEnabled,
		Sprint,
		NumFlags
	};

	std::bitset< NumFlags > Flags;
};

