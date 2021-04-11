#include "UndoRedoSystem.h"

UndoRedoSystem::UndoRedoSystem()
{
}

UndoRedoSystem::~UndoRedoSystem()
{
}

UndoRedoSystem& UndoRedoSystem::GetInstance()
{
	if( !singleton )
		singleton = std::make_unique<UndoRedoSystem>();
	return *singleton;
}

void UndoRedoSystem::ExecuteAction( std::function< void() > action, std::function< void() > undo )
{
	actions.emplace_back();
	actions.back().do_action = action;
	actions.back().undo_action = undo;
	++current_index;
	action();
}

bool UndoRedoSystem::UndoAction()
{
	if( actions.empty() )
		return;

	if( current_index == 0 )
		return;

	--current_index;
	actions[current_index].undo_action();
}

bool UndoRedoSystem::RedoAction()
{
	if( actions.empty() )
		return;

	if( current_index == actions.size() - 1 )
		return;

	actions[current_index].do_action();
	++current_index;
	actions.resize( current_index );
}
