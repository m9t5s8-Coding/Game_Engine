#include <Editor/UndoRedo.hpp>

namespace ag {

UndoRedoManager& UndoRedoManager::get() {
  static UndoRedoManager instance;
  return instance;
}

void UndoRedoManager::execute(std::unique_ptr<UndoCommand> cmd) {
  if (!cmd) return;
  cmd->redo();
  push(std::move(cmd));
}

void UndoRedoManager::push(std::unique_ptr<UndoCommand> cmd) {
  if (!cmd) return;
  m_redo_stack.clear();

  if (m_group_depth > 0 && m_active_group) {
    m_active_group->commands.push_back(std::move(cmd));
    return;
  }

  if (!m_undo_stack.empty()) {
    if (m_undo_stack.back()->merge_with(cmd.get())) {
      // merged into previous; discard cmd
      return;
    }
  }

  m_undo_stack.push_back(std::move(cmd));
  trim_undo_stack();
}

void UndoRedoManager::undo() {
  if (m_group_depth > 0) return; // don't undo while grouping
  if (m_undo_stack.empty()) return;
  auto cmd = std::move(m_undo_stack.back());
  m_undo_stack.pop_back();
  cmd->undo();
  m_redo_stack.push_back(std::move(cmd));
}

void UndoRedoManager::redo() {
  if (m_group_depth > 0) return;
  if (m_redo_stack.empty()) return;
  auto cmd = std::move(m_redo_stack.back());
  m_redo_stack.pop_back();
  cmd->redo();
  m_undo_stack.push_back(std::move(cmd));
  trim_undo_stack();
}

const char* UndoRedoManager::undo_name() const {
  if (m_undo_stack.empty()) return "";
  // note: returning c_str of temporary is unsafe; keep a static? easier return pointer to member string via thread-local/static not needed, but name() returns by value.
  // To be safe, caller should not store; but we need stable pointer for ImGui. Use a thread-local buffer.
  thread_local std::string buf;
  buf = m_undo_stack.back()->name();
  return buf.c_str();
}

const char* UndoRedoManager::redo_name() const {
  if (m_redo_stack.empty()) return "";
  thread_local std::string buf;
  buf = m_redo_stack.back()->name();
  return buf.c_str();
}

void UndoRedoManager::clear() {
  m_undo_stack.clear();
  m_redo_stack.clear();
  m_active_group.reset();
  m_group_depth = 0;
}

void UndoRedoManager::begin_group(const std::string& name) {
  if (m_group_depth == 0) {
    m_active_group = std::make_unique<CommandGroup>();
    m_active_group->name_str = name;
  }
  ++m_group_depth;
}

void UndoRedoManager::end_group() {
  if (m_group_depth <= 0) return;
  --m_group_depth;
  if (m_group_depth == 0 && m_active_group) {
    if (!m_active_group->commands.empty()) {
      // push group as a single command (already executed actions inside group)
      // but execute() would redo again; we need push without redoing.
      push(std::move(m_active_group));
    }
    m_active_group.reset();
  }
}

void UndoRedoManager::trim_undo_stack() {
  while (m_undo_stack.size() > m_max_size) {
    m_undo_stack.erase(m_undo_stack.begin());
  }
}

} // namespace ag
