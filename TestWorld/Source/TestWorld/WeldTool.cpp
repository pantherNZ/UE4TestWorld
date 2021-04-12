#include "WeldTool.h"

#include "Utility.h"
#include "UndoRedoSystem.h"

void UWeldTool::OnMouse1_Implementation( bool Pressed )
{
	if( !Pressed )
		return;

	if( TargetActor )
	{
		const FHitResult Impact = WeaponTrace();

		if( Impact.GetActor() && Impact.GetActor()->IsRootComponentMovable() )
		{
			UndoRedoSystem::GetInstance().ExecuteAction( 
				[this, Impact, TargetActor = TargetActor]()
				{
					auto* weld = NewObject<UPhysicsConstraintComponent>( TargetActor );
					weld->RegisterComponent();
					weld->SetConstrainedComponents( Cast<UPrimitiveComponent>( TargetActor->GetRootComponent() ), {}, Cast<UPrimitiveComponent>( Impact.GetActor()->GetRootComponent() ), {} );
					weld->SetAngularSwing1Limit( EAngularConstraintMotion::ACM_Locked, 0.0f );
					weld->SetAngularSwing2Limit( EAngularConstraintMotion::ACM_Locked, 0.0f );
					weld->SetAngularTwistLimit( EAngularConstraintMotion::ACM_Locked, 0.0f );
					weld->SetLinearXLimit( ELinearConstraintMotion::LCM_Locked, 0.0f );
					weld->SetLinearYLimit( ELinearConstraintMotion::LCM_Locked, 0.0f );
					weld->SetLinearZLimit( ELinearConstraintMotion::LCM_Locked, 0.0f );
					weld->SetDisableCollision( DisableCollision );
					weld->SetLinearBreakable( WeldStrengthLinear != 0.0f, WeldStrengthLinear );
					weld->SetAngularBreakable( WeldStrengthAngular != 0.0f, WeldStrengthAngular );
					weld->AttachToComponent( TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform );
					welds.Add( weld );
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
	else
	{
		const FHitResult Impact = WeaponTrace();

		if( Impact.GetActor() && Impact.GetActor()->IsRootComponentMovable() )
		{
			TargetActor = Impact.GetActor();

			ReflexOutput( ReflexFormat( "Mouse1 Pressed, target: {0}", TargetActor->GetName() ) );
		}
	}
}

void UWeldTool::OnMouse2_Implementation( bool Pressed )
{
	TargetActor = nullptr;
}

void UWeldTool::OnEnabledChanged_Implementation( bool Enabled )
{
	if( !Enabled )
		TargetActor = nullptr;
}
