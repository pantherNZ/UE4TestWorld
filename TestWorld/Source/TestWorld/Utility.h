#pragma once

#define ReflexFormat( Message, ... ) \
	FString::Format( TEXT( Message ), { __VA_ARGS__ } )

#define ReflexOutput( Message ) \
	do \
	{ \
		GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Green, *( Message ) ); \
		UE_LOG( LogTemp, Display, TEXT( "%s" ), *( Message ) ); \
	} while( false )

//#define OutputF( Message, ... ) \
//	GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Green, FString::Format( TEXT( Message ), { __VA_ARGS__ } ) ); \
//	UE_LOG( LogTemp, Display, TEXT( Message ), __VA_ARGS__ );