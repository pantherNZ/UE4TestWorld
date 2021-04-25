// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityLibrary.generated.h"

UENUM( BlueprintType )
enum class LogDisplayType : uint8
{
	Gameplay,
	Warn,
	Error,
};

#define ReflexFormat( Message, ... ) \
	FString::Format( TEXT( Message ), { __VA_ARGS__ } )

UCLASS()
class TESTWORLD_API UUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//UFUNCTION( BlueprintPure, Category = "Utility" )
	//static UGameDataAssets* GetGameData();

	//UFUNCTION( BlueprintPure, Category = "Utility" )
	//static UCubeDataSingleton* GetSingletonGameData();

	UFUNCTION( BlueprintPure, Category = "Utility" )
	static bool IsWithEditor();

	UFUNCTION( BlueprintCallable, Category = "Utility" )
	static void CustomLog( FString CustomOutput, LogDisplayType CustomLogType = LogDisplayType::Gameplay );

	UFUNCTION( BlueprintCallable, Category = "Utility" )
	static FVector GetRainbowColour( int32 value, int32 max );
};
