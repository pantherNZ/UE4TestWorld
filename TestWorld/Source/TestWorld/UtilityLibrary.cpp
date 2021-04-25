// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityLibrary.h"


bool UUtilityLibrary::IsWithEditor()
{
	return GIsEditor;
}

void UUtilityLibrary::CustomLog( FString CustomOutput, LogDisplayType CustomLogType /*= Gameplay*/ )
{
	//if( GetSingletonGameData() && GetSingletonGameData()->EnableDebugLogging )
	{
		if( CustomLogType == LogDisplayType::Gameplay )
		{
			GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Green, CustomOutput );
			UE_LOG( LogTemp, Display, TEXT( "%s" ), *CustomOutput );
		}
		else if( CustomLogType == LogDisplayType::Warn )
		{
			GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Yellow, CustomOutput );
			UE_LOG( LogTemp, Warning, TEXT( "%s" ), *CustomOutput );
		}
	}

	if( CustomLogType == LogDisplayType::Error )
	{
		GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Red, CustomOutput );
		UE_LOG( LogTemp, Error, TEXT( "%s" ), *CustomOutput );
	}
}

FVector UUtilityLibrary::GetRainbowColour( int32 value, int32 max )
{
	float inc = 6.0 / max;
	float x = value * inc;
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;

	if( ( 0 <= x && x <= 1 ) || ( 5 <= x && x <= 6 ) ) r = 1.0f;
	else if( 4 <= x && x <= 5 ) r = x - 4;
	else if( 1 <= x && x <= 2 ) r = 1.0f - ( x - 1 );
	if( 1 <= x && x <= 3 ) g = 1.0f;
	else if( 0 <= x && x <= 1 ) g = x - 0;
	else if( 3 <= x && x <= 4 ) g = 1.0f - ( x - 3 );
	if( 3 <= x && x <= 5 ) b = 1.0f;
	else if( 2 <= x && x <= 3 ) b = x - 2;
	else if( 5 <= x && x <= 6 ) b = 1.0f - ( x - 5 );

	return FVector( r, g, b );
}