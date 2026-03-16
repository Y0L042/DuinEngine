#pragma once

#include "EditorCommand.h"

#include <memory>
#include <deque>

class CommandManager
{
  public:
    bool Do(std::shared_ptr<EditorCommand> cmd);
    bool Record(std::shared_ptr<EditorCommand> cmd);
    bool Undo();
    bool Redo();

  private:
    size_t maxQueueSize = 50;
    std::deque<std::shared_ptr<EditorCommand>> undoQueue;
    std::deque<std::shared_ptr<EditorCommand>> redoQueue;

    void CullUndo();
    void CullRedo();
};