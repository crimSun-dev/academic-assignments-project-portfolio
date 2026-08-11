/**
 * taskStore.js
 * Responsibility: persistence only. Knows how to read/write Task records
 * to localStorage and generate ids. Has no knowledge of validation rules
 * or the DOM — this keeps it swappable (e.g., for a real backend/API)
 * without touching UI or controller code.
 */
const TaskStore = (function () {
  const STORAGE_KEY = "studytrack_tasks_v1";

  function _readAll() {
    try {
      const raw = localStorage.getItem(STORAGE_KEY);
      return raw ? JSON.parse(raw) : [];
    } catch (e) {
      console.error("TaskStore: failed to parse stored tasks, resetting.", e);
      return [];
    }
  }

  function _writeAll(tasks) {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(tasks));
  }

  function getAll() {
    return _readAll();
  }

  function add(task) {
    const tasks = _readAll();
    const newTask = {
      id: "t_" + Date.now() + "_" + Math.floor(Math.random() * 1000),
      title: task.title,
      course: task.course || "",
      dueDate: task.dueDate,
      priority: task.priority || "Medium",
      description: task.description || "",
      status: "Open",
      createdAt: new Date().toISOString(),
    };
    tasks.push(newTask);
    _writeAll(tasks);
    return newTask;
  }

  function updateStatus(id, status) {
    const tasks = _readAll();
    const idx = tasks.findIndex((t) => t.id === id);
    if (idx === -1) return null;
    tasks[idx].status = status;
    tasks[idx].completedAt = status === "Complete" ? new Date().toISOString() : null;
    _writeAll(tasks);
    return tasks[idx];
  }

  function remove(id) {
    const tasks = _readAll().filter((t) => t.id !== id);
    _writeAll(tasks);
  }

  function clearAll() {
    _writeAll([]);
  }

  return { getAll, add, updateStatus, remove, clearAll };
})();
