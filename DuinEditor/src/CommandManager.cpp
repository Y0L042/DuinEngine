#include "CommandManager.h"
#include <Duin/Core/Debug/DNLog.h>

bool CommandManager::Do(std::shared_ptr<EditorCommand> cmd)
{
    bool res = false;
    if (cmd)
    {
        cmd->Do();
        undoQueue.push_back(cmd);
        redoQueue.clear();

        res = true;
    }
    else
    {
        DN_WARN("Attempted to execute invalid Do command!");
    }

    CullUndo();

    return res;
}

bool CommandManager::Record(std::shared_ptr<EditorCommand> cmd)
{
    bool res = false;
    if (cmd)
    {
        undoQueue.push_back(cmd);
        redoQueue.clear();

        res = true;
    }
    else
    {
        DN_WARN("Attempted to record invalid command!");
    }

    CullUndo();

    return res;
}

bool CommandManager::Undo()
{
    bool res = false;
    if (!undoQueue.empty())
    {
        std::shared_ptr<EditorCommand> cmd = undoQueue.back();
        if (cmd)
        {
            cmd->Undo();
            undoQueue.pop_back();
            redoQueue.push_back(cmd);

            res = true;
        }
        else
        {
            DN_WARN("Attempted to execute invalid Undo command!");
        }
    }

    CullRedo();

    return res;
}

bool CommandManager::Redo()
{
    bool res = false;
    if (!redoQueue.empty())
    {
        std::shared_ptr<EditorCommand> cmd = redoQueue.back();
        if (cmd)
        {
            cmd->Do();
            redoQueue.pop_back();
            undoQueue.push_back(cmd);

            res = true;
        }
        else
        {
            DN_WARN("Attempted to execute invalid Redo (Do) command!");
        }
    }

    CullUndo();

    return res;
}

void CommandManager::CullUndo()
{
    if (undoQueue.size() >= maxQueueSize)
    {
        undoQueue.pop_front();
    }
}

void CommandManager::CullRedo()
{
    if (redoQueue.size() >= maxQueueSize)
    {
        redoQueue.pop_front();
    }
}

// Doctest unit tests for CommandManager
// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include "CommandManager.h"
#include <vector>

namespace TestCommandManager
{
struct TestCommand : public EditorCommand
{
    TestCommand(std::vector<int> &data, int value) : data(data), value(value)
    {
        Do = [this, &data, value]() { data.push_back(value); };
        Undo = [this, &data]() {
            if (!data.empty())
                data.pop_back();
        };
    }
    std::vector<int> &data;
    int value;
};
TEST_SUITE("Basic Undo/Redo tests")
{
    TEST_CASE("CommandManager::Do/Undo/Redo basic operations")
    {
        CommandManager mgr;
        std::vector<int> data;

        auto cmd1 = std::make_shared<TestCommand>(data, 42);
        auto cmd2 = std::make_shared<TestCommand>(data, 99);

        // Do cmd1
        CHECK(mgr.Do(cmd1));
        CHECK(data.size() == 1);
        CHECK(data.back() == 42);

        // Do cmd2
        CHECK(mgr.Do(cmd2));
        CHECK(data.size() == 2);
        CHECK(data.back() == 99);

        // Undo cmd2
        CHECK(mgr.Undo());
        CHECK(data.size() == 1);
        CHECK(data.back() == 42);

        // Undo cmd1
        CHECK(mgr.Undo());
        CHECK(data.empty());

        // Redo cmd1
        CHECK(mgr.Redo());
        CHECK(data.size() == 1);
        CHECK(data.back() == 42);

        // Redo cmd2
        CHECK(mgr.Redo());
        CHECK(data.size() == 2);
        CHECK(data.back() == 99);
    }

    TEST_CASE("CommandManager handles invalid commands")
    {
        CommandManager mgr;
        auto invalid = std::make_shared<EditorCommand>();
        CHECK_FALSE(mgr.Do(invalid));
        CHECK_FALSE(mgr.Undo());
        CHECK_FALSE(mgr.Redo());
    }

    TEST_CASE("CommandManager::Record pushes without executing Do")
    {
        CommandManager mgr;
        std::vector<int> data;

        auto cmd = std::make_shared<TestCommand>(data, 10);

        // Record should NOT call Do
        CHECK(mgr.Record(cmd));
        CHECK(data.empty());

        // But Undo should still work
        CHECK(mgr.Undo());
        // Undo pops back (empty vec => pop_back is guarded)
        CHECK(data.empty());

        // Redo should re-execute Do
        CHECK(mgr.Redo());
        CHECK(data.size() == 1);
        CHECK(data.back() == 10);
    }

    TEST_CASE("CommandManager::Record clears redo queue")
    {
        CommandManager mgr;
        std::vector<int> data;

        auto cmd1 = std::make_shared<TestCommand>(data, 1);
        auto cmd2 = std::make_shared<TestCommand>(data, 2);

        // Do cmd1, then undo to populate redo queue
        mgr.Do(cmd1);
        mgr.Undo();
        CHECK(data.empty());

        // Record cmd2 should clear redo
        mgr.Record(cmd2);
        CHECK_FALSE(mgr.Redo()); // redo queue was cleared
    }

    TEST_CASE("CommandManager::Do clears redo queue")
    {
        CommandManager mgr;
        std::vector<int> data;

        auto cmd1 = std::make_shared<TestCommand>(data, 1);
        auto cmd2 = std::make_shared<TestCommand>(data, 2);

        mgr.Do(cmd1);
        mgr.Undo();

        // New Do should clear redo
        mgr.Do(cmd2);
        CHECK_FALSE(mgr.Redo());
    }
}
} // namespace TestCommandManager