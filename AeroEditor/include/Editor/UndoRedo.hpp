#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ag {

class UndoCommand {
public:
  virtual ~UndoCommand() = default;
  virtual void undo() = 0;
  virtual void redo() = 0;
  virtual std::string name() const { return "Action"; }

  // Optional: merge consecutive commands (e.g. text typing, slider drags)
  virtual bool merge_with(UndoCommand*) { return false; }
};

class FunctionCommand : public UndoCommand {
public:
  FunctionCommand(std::string name, std::function<void()> undo_fn, std::function<void()> redo_fn)
      : m_name(std::move(name)), m_undo(std::move(undo_fn)), m_redo(std::move(redo_fn)) {}

  void undo() override { if (m_undo) m_undo(); }
  void redo() override { if (m_redo) m_redo(); }
  std::string name() const override { return m_name; }

private:
  std::string m_name;
  std::function<void()> m_undo;
  std::function<void()> m_redo;
};

class UndoRedoManager {
public:
  static UndoRedoManager& get();

  void execute(std::unique_ptr<UndoCommand> cmd);
  void push(std::unique_ptr<UndoCommand> cmd); // push already-executed command
  void undo();
  void redo();

  bool can_undo() const { return !m_undo_stack.empty(); }
  bool can_redo() const { return !m_redo_stack.empty(); }
  const char* undo_name() const;
  const char* redo_name() const;

  void clear();
  void set_max_size(size_t max_size) { m_max_size = max_size; }

  // Grouping: begin_group/end_group to treat multiple commands as one undo step.
  void begin_group(const std::string& name = "Group");
  void end_group();
  bool in_group() const { return m_group_depth > 0; }

  // Used by UI to enable/disable menu items
  bool is_dirty_marker_enabled() const { return true; }

private:
  UndoRedoManager() = default;

  struct CommandGroup : UndoCommand {
    std::string name_str = "Group";
    std::vector<std::unique_ptr<UndoCommand>> commands;
    void undo() override {
      for (auto it = commands.rbegin(); it != commands.rend(); ++it) (*it)->undo();
    }
    void redo() override {
      for (auto& c : commands) c->redo();
    }
    std::string name() const override { return name_str; }
  };

  void trim_undo_stack();

  std::vector<std::unique_ptr<UndoCommand>> m_undo_stack;
  std::vector<std::unique_ptr<UndoCommand>> m_redo_stack;
  size_t m_max_size = 200;
  int m_group_depth = 0;
  std::unique_ptr<CommandGroup> m_active_group;
};

} // namespace ag
