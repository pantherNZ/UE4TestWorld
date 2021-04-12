#include "RemovalTool.h"

#include "Utility.h"
#include "UndoRedoSystem.h"

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

				ReflexOutput( ReflexFormat( "Mouse1 Pressed, creating weld between {0} and {1}", TargetActor->GetName(), Impact.GetActor()->GetName() ) );
			},
			[this]()
			{
				welds.Last()->DestroyComponent();
				welds.Pop();
			} );

		TargetActor = nullptr;
	}
}

void URemovalTool::OnMouse2_Implementation( bool Pressed )
{
	
}