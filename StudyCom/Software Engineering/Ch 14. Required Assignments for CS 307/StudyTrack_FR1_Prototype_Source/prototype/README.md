# StudyTrack — Task Creation & Management Prototype

A working prototype of **FR1 / US1 (Task Creation & Management)** from the
StudyTrack product backlog (Assignment 1). Lets a student create, view,
filter, complete, and delete tasks, each tied to a course, due date, and
priority.

## Files

- `index.html` — page structure and form/list markup
- `style.css` — visual styling
- `taskStore.js` — persistence layer (reads/writes `localStorage`)
- `taskController.js` — validation and business rules (create, list, filter, complete, delete)
- `app.js` — DOM rendering and event wiring; calls `TaskController` only

## How to run

No build step or server required.

1. Download/unzip this folder.
2. Double-click `index.html`, or open it in any modern browser
   (Chrome, Firefox, Edge, Safari) via **File → Open**.
3. Add a task using the form on the left (Title and Due Date are required).
   Use the course filter and "Hide completed" checkbox on the right to
   narrow the list. Click **Mark Complete** or **Delete** on any task card.

Data is stored in the browser's `localStorage` under the key
`studytrack_tasks_v1`. It persists across page reloads but is local to
that browser (clearing browser storage removes all tasks).

## Architecture

`app.js` (View) → `taskController.js` (validation/business rules) →
`taskStore.js` (persistence) → `localStorage`. The view never talks to
storage directly; see the component and sequence diagrams in the
write-up for the full design rationale.

## Automated checks

Two Node/Playwright scripts were used during development (not required
to run the app, included for transparency):

- `shots.js` — captures the UI screenshots used in the write-up
- `vv_tests.js` — runs the 5 test cases from the V&V plan headlessly
  and prints pass/fail results

Run with `node shots.js` / `node vv_tests.js` from this folder if Node
and `npm install playwright` are available; not required to use the app.
