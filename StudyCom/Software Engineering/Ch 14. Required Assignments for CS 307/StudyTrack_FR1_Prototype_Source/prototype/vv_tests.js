const { chromium } = require("playwright");
const path = require("path");

(async () => {
  const browser = await chromium.launch();
  const page = await browser.newPage();
  const url = "file://" + path.resolve(__dirname, "prototype/index.html");

  const results = [];

  // TC1: Create task with valid required fields only
  await page.goto(url);
  await page.evaluate(() => localStorage.clear());
  await page.reload();
  await page.fill("#title", "Read syllabus");
  await page.fill("#dueDate", "2026-09-01");
  await page.click("#submit-btn");
  const tc1Count = await page.locator(".task-item").count();
  const tc1Priority = await page.locator(".badge").first().textContent();
  results.push({
    id: "TC1",
    desc: "Create task with only required fields (title, due date)",
    expected: "Task saved, appears in list, defaults to Medium priority",
    actual: `count=${tc1Count}, defaultPriority=${tc1Priority.trim()}`,
    pass: tc1Count === 1 && tc1Priority.trim() === "Medium",
  });

  // TC2: Reject empty title and due date
  await page.click("#submit-btn"); // submit again with now-empty fields (form was reset)
  const errTitle = await page.locator("#err-title").textContent();
  const errDue = await page.locator("#err-dueDate").textContent();
  const tc2Count = await page.locator(".task-item").count();
  results.push({
    id: "TC2",
    desc: "Reject task creation when title and due date are blank",
    expected: "Inline errors shown; task count unchanged (still 1)",
    actual: `errTitle="${errTitle}", errDue="${errDue}", count=${tc2Count}`,
    pass: errTitle.includes("required") && errDue.includes("required") && tc2Count === 1,
  });

  // TC3: Mark complete updates status and re-sorts
  await page.fill("#title", "Early task");
  await page.fill("#dueDate", "2026-08-01");
  await page.click("#submit-btn");
  await page.waitForTimeout(100);
  // complete the later-dated task (Read syllabus, due 09-01) which should currently be second in the list
  const titlesBefore = await page.locator(".task-title").allTextContents();
  const idxReadSyllabus = titlesBefore.indexOf("Read syllabus");
  await page.locator(".complete-btn").nth(idxReadSyllabus).click();
  await page.waitForTimeout(100);
  const lastItemText = await page.locator(".task-item").last().locator(".task-title").textContent();
  const completedBadge = await page.locator(".task-item.completed").count();
  results.push({
    id: "TC3",
    desc: "Marking a task complete updates status and moves it below open tasks",
    expected: "Completed task shows 'Complete' status and sorts after open tasks",
    actual: `lastItem="${lastItemText}", completedCount=${completedBadge}`,
    pass: lastItemText.trim() === "Read syllabus" && completedBadge === 1,
  });

  // TC4 (reliability/persistence): reload page, data should survive
  await page.reload();
  const countAfterReload = await page.locator(".task-item").count();
  const completedAfterReload = await page.locator(".task-item.completed").count();
  results.push({
    id: "TC4",
    desc: "Quality attribute: Reliability — task data persists across a full page reload",
    expected: "All 2 tasks still present after reload, completed status retained",
    actual: `count=${countAfterReload}, completedCount=${completedAfterReload}`,
    pass: countAfterReload === 2 && completedAfterReload === 1,
  });

  // TC5: Delete task removes it and updates course filter options
  await page.fill("#title", "Temp task to delete");
  await page.fill("#course", "TEMP101");
  await page.fill("#dueDate", "2026-08-05");
  await page.click("#submit-btn");
  await page.waitForTimeout(100);
  page.once("dialog", (d) => d.accept());
  const deleteButtons = page.locator(".delete-btn");
  const delTargetIndex = (await page.locator(".task-title").allTextContents()).indexOf("Temp task to delete");
  await deleteButtons.nth(delTargetIndex).click();
  await page.waitForTimeout(100);
  const countAfterDelete = await page.locator(".task-item").count();
  const filterOptions = await page.locator("#course-filter option").allTextContents();
  results.push({
    id: "TC5",
    desc: "Deleting a task removes it from the list and from the course filter options",
    expected: "Task count returns to 2; 'TEMP101' no longer in course filter",
    actual: `count=${countAfterDelete}, filterOptions=${JSON.stringify(filterOptions)}`,
    pass: countAfterDelete === 2 && !filterOptions.includes("TEMP101"),
  });

  console.log(JSON.stringify(results, null, 2));
  await browser.close();
})();
