#pragma once
#include <functional>

class EditorCommand
{
  public:
    virtual ~EditorCommand() = default;

    std::function<void(void)> Do;
    std::function<void(void)> Undo;

    bool IsValid() const
    {
        return (Do && Undo);
    }

    explicit operator bool() const
    {
        return IsValid();
    }

  private:
};