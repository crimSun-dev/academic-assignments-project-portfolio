/**
 * app.js
 * Responsibility: DOM rendering and event wiring only. Reads/writes the
 * page, but delegates every decision (validation, sorting, persistence)
 * to TaskController / TaskStore. This separation is what lets the
 * storage or validation rules change later without editing this file.
 */
document.addEventListener("DOMContentLoaded", () => {
  const form = document.getElementById("task-form");
  const titleInput = document.getElementById("title");
  const courseInput = document.getElementById("course");
  const dueDateInput = document.getElementById("dueDate");
  const priorityInput = document.getElementById("priority");
  const descriptionInput = document.getElementById("description");
  const formStatus = document.getElementById("form-status");
  const errTitle = document.getElementById("err-title");
  const errDueDate = document.getElementById("err-dueDate");

  const taskList = document.getElementById("task-list");
  const emptyState = document.getElementById("empty-state");
  const courseFilter = document.getElementById("course-filter");
  const hideCompleteCheckbox = document.getElementById("hide-complete");

  function clearErrors() {
    errTitle.textContent = "";
    errDueDate.textContent = "";
    formStatus.textContent = "";
    formStatus.className = "status";
  }

  function refreshCourseFilterOptions() {
    const current = courseFilter.value;
    const courses = TaskController.distinctCourses();
    courseFilter.innerHTML = '<option value="">All</option>';
    courses.forEach((c) => {
      const opt = document.createElement("option");
      opt.value = c;
      opt.textContent = c;
      courseFilter.appendChild(opt);
    });
    if (courses.includes(current)) courseFilter.value = current;
  }

  function render() {
    const tasks = TaskController.listTasks({
      course: courseFilter.value || undefined,
      hideCompleted: hideCompleteCheckbox.checked,
    });

    taskList.innerHTML = "";
    emptyState.style.display = tasks.length === 0 ? "block" : "none";

    tasks.forEach((task) => {
      const li = document.createElement("li");
      li.className = `task-item priority-${task.priority}${task.status === "Complete" ? " completed" : ""}`;
      li.dataset.id = task.id;

      const main = document.createElement("div");
      main.className = "task-main";

      const title = document.createElement("div");
      title.className = "task-title";
      title.textContent = task.title;
      main.appendChild(title);

      const meta = document.createElement("div");
      meta.className = "task-meta";
      meta.innerHTML = `
        <span>Due: ${task.dueDate}</span>
        ${task.course ? `<span>Course: ${escapeHtml(task.course)}</span>` : ""}
        <span class="badge ${task.priority}">${task.priority}</span>
        <span>${task.status}</span>
      `;
      main.appendChild(meta);

      if (task.description) {
        const desc = document.createElement("div");
        desc.className = "task-desc";
        desc.textContent = task.description;
        main.appendChild(desc);
      }

      const actions = document.createElement("div");
      actions.className = "task-actions";

      const completeBtn = document.createElement("button");
      completeBtn.className = "complete-btn";
      completeBtn.textContent = task.status === "Complete" ? "Mark Open" : "Mark Complete";
      completeBtn.addEventListener("click", () => {
        TaskController.toggleComplete(task.id, task.status);
        render();
      });

      const deleteBtn = document.createElement("button");
      deleteBtn.className = "delete-btn";
      deleteBtn.textContent = "Delete";
      deleteBtn.addEventListener("click", () => {
        if (confirm(`Delete "${task.title}"?`)) {
          TaskController.deleteTask(task.id);
          refreshCourseFilterOptions();
          render();
        }
      });

      actions.appendChild(completeBtn);
      actions.appendChild(deleteBtn);

      li.appendChild(main);
      li.appendChild(actions);
      taskList.appendChild(li);
    });
  }

  function escapeHtml(str) {
    const div = document.createElement("div");
    div.textContent = str;
    return div.innerHTML;
  }

  form.addEventListener("submit", (e) => {
    e.preventDefault();
    clearErrors();

    const input = {
      title: titleInput.value,
      course: courseInput.value,
      dueDate: dueDateInput.value,
      priority: priorityInput.value,
      description: descriptionInput.value,
    };

    const result = TaskController.createTask(input);

    if (!result.success) {
      if (result.errors.title) errTitle.textContent = result.errors.title;
      if (result.errors.dueDate) errDueDate.textContent = result.errors.dueDate;
      formStatus.textContent = "Please fix the highlighted fields.";
      formStatus.className = "status error";
      return;
    }

    formStatus.textContent = `"${result.task.title}" added.`;
    formStatus.className = "status success";
    form.reset();
    priorityInput.value = "Medium";
    refreshCourseFilterOptions();
    render();
  });

  courseFilter.addEventListener("change", render);
  hideCompleteCheckbox.addEventListener("change", render);

  refreshCourseFilterOptions();
  render();
});
