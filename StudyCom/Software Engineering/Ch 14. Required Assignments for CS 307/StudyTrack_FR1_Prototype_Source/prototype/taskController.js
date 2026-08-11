/**
 * taskController.js
 * Responsibility: business rules and validation. Sits between the UI
 * (app.js) and the persistence layer (taskStore.js). The UI never talks
 * to TaskStore directly — every create/update/delete goes through here,
 * so validation logic exists in exactly one place (FR1 requires title
 * and due date; this enforces that rule regardless of which UI calls it).
 */
const TaskController = (function () {
  function validate(input) {
    const errors = {};
    if (!input.title || !input.title.trim()) {
      errors.title = "Title is required.";
    }
    if (!input.dueDate) {
      errors.dueDate = "Due date is required.";
    }
    return { valid: Object.keys(errors).length === 0, errors };
  }

  function createTask(input) {
    const result = validate(input);
    if (!result.valid) {
      return { success: false, errors: result.errors };
    }
    const task = TaskStore.add({
      title: input.title.trim(),
      course: (input.course || "").trim(),
      dueDate: input.dueDate,
      priority: input.priority || "Medium",
      description: (input.description || "").trim(),
    });
    return { success: true, task };
  }

  function listTasks({ course, hideCompleted } = {}) {
    let tasks = TaskStore.getAll();
    if (course) {
      tasks = tasks.filter((t) => t.course === course);
    }
    if (hideCompleted) {
      tasks = tasks.filter((t) => t.status !== "Complete");
    }
    // Sort: open tasks first, then by due date ascending
    return tasks.sort((a, b) => {
      if (a.status !== b.status) return a.status === "Complete" ? 1 : -1;
      return new Date(a.dueDate) - new Date(b.dueDate);
    });
  }

  function toggleComplete(id, currentStatus) {
    const next = currentStatus === "Complete" ? "Open" : "Complete";
    return TaskStore.updateStatus(id, next);
  }

  function deleteTask(id) {
    TaskStore.remove(id);
  }

  function distinctCourses() {
    const tasks = TaskStore.getAll();
    return [...new Set(tasks.map((t) => t.course).filter(Boolean))].sort();
  }

  return { createTask, listTasks, toggleComplete, deleteTask, distinctCourses, validate };
})();
