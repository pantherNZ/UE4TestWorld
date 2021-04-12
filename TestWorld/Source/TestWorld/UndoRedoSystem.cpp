#include "UndoRedoSystem.h"

std::unique_ptr< UndoRedoSystem > UndoRedoSystem::singleton;

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
		return false;

	if( current_index == 0 )
		return false;

	--current_index;
	actions[current_index].undo_action();
	return true;
}

bool UndoRedoSystem::RedoAction()
{
	if( actions.empty() )
		return false;

	if( current_index == actions.size() )
		return false;

	actions[current_index].do_action();
	++current_index;
	actions.resize( current_index );
	return true;
}
