// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestWorldCharacter.generated.h"

UCLASS(config=Game)
class ATestWorldCharacter : public ACharacter
{
	GENERATED_BODY()

	// Methods
public:
	ATestWorldCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Members
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera )
		float BaseTurnRate;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera )
		float BaseLookUpRate;

private:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
		class USpringArmComponent* CameraBoom;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
		class UCameraComponent* FollowCamera;
};

