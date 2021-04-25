#pragma once

#include "CoreMinimal.h"
#include "BasePlayerTool.h"
#include "TestWorldSaveGame.h"
#include "RemovalTool.generated.h"

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class TESTWORLD_API URemovalTool : public UBasePlayerTool
{
	GENERATED_BODY()

protected:
	FString GetName() const override { return TEXT( "Removal Tool" ); }

	void OnMouse1_Implementation( bool Pressed ) override;
	void OnMouse2_Implementation( bool Pressed ) override;

protected:
	UPROPERTY( EditInstanceOnly )
	TArray< FActorSaveData > SavedObjectData;
};
