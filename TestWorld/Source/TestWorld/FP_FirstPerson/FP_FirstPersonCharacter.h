// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_FirstPersonCharacter();

	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected:
	void OnMouse1( const bool pressed );
	void OnMouse2();
	void OnMouseWheelDown();
	void OnMouseWheelUp();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void Turn(float Rate);
	void LookUp(float Rate);

	void UpdateTargetLocation() const;
	FVector GetTargetLockLocation() const;
	virtual void Tick( float DeltaTime ) override;

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	FVector GunOffset;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	USoundBase* FireSound;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	UAnimMontage* FireAnimation;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float TraceDistance;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float ScrollSpeed;

private:
	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	UCameraComponent* FirstPersonCameraComponent;

	// Target (phys gun object) variables
	UPROPERTY()
	AActor* target_cmp;

	FVector target_location_offset;
	FRotator target_rotation_offset;
	float target_distance;
};

