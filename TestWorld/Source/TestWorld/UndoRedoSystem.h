#pragma once

#include "CoreMinimal.h"

#include <vector>
#include <functional>
#include <memory>

class TESTWORLD_API UndoRedoSystem
{
public:
	UndoRedoSystem();
	~UndoRedoSystem();

	static UndoRedoSystem& GetInstance();
	void ExecuteAction( std::function< void() > action, std::function< void() > undo );
	bool UndoAction();
	bool RedoAction();

protected:
	struct Action
	{
		std::function< void() > do_action;
		std::function< void() > undo_action;
	};

	unsigned current_index = 0;
	std::vector< Action > actions;
	static std::unique_ptr< UndoRedoSystem > singleton;
};
 