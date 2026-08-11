const { chromium } = require("playwright");
const path = require("path");

(async () => {
  const browser = await chromium.launch();
  const page = await browser.newPage({ viewport: { width: 1200, height: 800 } });
  const url = "file://" + path.resolve(__dirname, "prototype/index.html");
  await page.goto(url);

  // Screenshot 1: empty state
  await page.screenshot({ path: "shot1_empty.png" });

  // Add task 1
  await page.fill("#title", "Finish reading Chapter 4");
  await page.fill("#course", "CS 210");
  await page.fill("#dueDate", "2026-08-15");
  await page.selectOption("#priority", "High");
  await page.fill("#description", "Focus on recursion examples before the quiz.");
  await page.click("#submit-btn");
  await page.waitForTimeout(200);

  // Add task 2
  await page.fill("#title", "Submit MAT 350 Project Two");
  await page.fill("#course", "MAT 350");
  await page.fill("#dueDate", "2026-08-20");
  await page.selectOption("#priority", "Medium");
  await page.click("#submit-btn");
  await page.waitForTimeout(200);

  // Add task 3
  await page.fill("#title", "Review lecture notes");
  await page.fill("#course", "CS 210");
  await page.fill("#dueDate", "2026-08-10");
  await page.selectOption("#priority", "Low");
  await page.click("#submit-btn");
  await page.waitForTimeout(200);

  await page.screenshot({ path: "shot2_task_list.png", fullPage: true });

  // Trigger validation error: submit with empty title/date
  await page.click("#submit-btn");
  await page.waitForTimeout(150);
  await page.screenshot({ path: "shot3_validation_error.png", fullPage: true });

  // Clear the accidental empty fields status, then mark a task complete
  // (re-fill nothing, just click complete on the first task)
  const completeButtons = await page.$$(".complete-btn");
  await completeButtons[0].click();
  await page.waitForTimeout(150);
  await page.screenshot({ path: "shot4_marked_complete.png", fullPage: true });

  // Filter by course
  await page.selectOption("#course-filter", "CS 210");
  await page.waitForTimeout(150);
  await page.screenshot({ path: "shot5_filtered.png", fullPage: true });

  await browser.close();
  console.log("done");
})();
