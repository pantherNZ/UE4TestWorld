#include "RemovalTool.h"

#include "UtilityLibrary.h"
#include "UndoRedoSystem.h"
#include "Serialization/ObjectWriter.h"
#include "Serialization/ObjectReader.h"
#include "Containers/UnrealString.h"

void URemovalTool::OnMouse1_Implementation( bool Pressed )
{
	if( !Pressed )
		return;

	const FHitResult Impact = WeaponTrace();


	if( Impact.GetActor() && Impact.GetActor()->IsRootComponentMovable() )
	{
		UndoRedoSystem::GetInstance().ExecuteAction(
			[this, Impact]()
			{
				auto* Actor = Impact.GetActor();
				FActorSaveData ActorRecord;
				ActorRecord.ActorName = FName( *Actor->GetName() );
				ActorRecord.ActorClass = Actor->GetClass()->GetPathName();
				ActorRecord.ActorTransform = Actor->GetTransform();
				FObjectWriter archive( Impact.GetActor(), ActorRecord.ActorData );
				SavedObjectData.Add( ActorRecord );
				Actor->Destroy();
				UUtilityLibrary::CustomLog( ReflexFormat( "Mouse1 Pressed, removing object {0}", Actor->GetName() ) );
			},
			[this]()
			{
				auto ActorRecord = SavedObjectData.Pop();
				FVector SpawnPos = ActorRecord.ActorTransform.GetLocation();
				FRotator SpawnRot = ActorRecord.ActorTransform.Rotator();
				FActorSpawnParameters SpawnParams;
				SpawnParams.Name = ActorRecord.ActorName;
				UClass* SpawnClass = FindObject<UClass>( ANY_PACKAGE, *ActorRecord.ActorClass );
				if( SpawnClass )
				{
					GetWorld()->PerformGarbageCollectionAndCleanupActors();
					AActor* NewActor = GetWorld()->SpawnActor( SpawnClass, &SpawnPos, &SpawnRot, SpawnParams );
					FObjectReader archive( NewActor, ActorRecord.ActorData );
				}

				UUtilityLibrary::CustomLog( ReflexFormat( "Mouse1 Pressed, recreating object from binary data {0}", SpawnParams.Name.ToString() ) );

			} );
	}
}

void URemovalTool::OnMouse2_Implementation( bool Pressed )
{
	
}