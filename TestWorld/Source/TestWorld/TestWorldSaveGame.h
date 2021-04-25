// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "TestWorldSaveGame.generated.h"

USTRUCT()
struct FActorSaveData
{
	GENERATED_USTRUCT_BODY()

	FString ActorClass;
	FName ActorName;
	FTransform ActorTransform;
	TArray<uint8> ActorData;

	friend FArchive& operator<<( FArchive& Ar, FActorSaveData& ActorData )
	{
		Ar << ActorData.ActorClass;
		Ar << ActorData.ActorName;
		Ar << ActorData.ActorTransform;
		Ar << ActorData.ActorData;
		return Ar;
	}
};

USTRUCT()
struct FSaveGameData
{
	GENERATED_USTRUCT_BODY()

	FName GameID;
	FDateTime Timestamp;
	TArray<FActorSaveData> SavedActors;

	friend FArchive& operator<<( FArchive& Ar, FSaveGameData& GameData )
	{
		Ar << GameData.GameID;
		Ar << GameData.Timestamp;
		Ar << GameData.SavedActors;
		return Ar;
	}
};

UCLASS()
class TESTWORLD_API UTestWorldSaveGame : public USaveGame
{
	GENERATED_BODY()
	
};
