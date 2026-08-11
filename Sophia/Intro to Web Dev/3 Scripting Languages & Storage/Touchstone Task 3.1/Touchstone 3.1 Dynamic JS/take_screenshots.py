import asyncio
from playwright.async_api import async_playwright
import os

screenshots_dir = '/mnt/agents/output/screenshots'
os.makedirs(screenshots_dir, exist_ok=True)
site_dir = '/mnt/agents/output/book-haven-site'

async def main():
    p = await async_playwright().start()
    browser = await p.chromium.launch(headless=True)
    
    # Screenshot 1: Subscribe alert on Home page
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    async def handle_dialog1(dialog):
        await page.screenshot(path=f'{screenshots_dir}/01_subscribe_alert.png')
        await dialog.accept()
    page.on('dialog', handle_dialog1)
    await page.goto(f'file://{site_dir}/index.html')
    await page.wait_for_timeout(1000)
    await page.evaluate("document.getElementById('subscribe-btn').click();")
    await page.wait_for_timeout(1000)
    await page.close()
    print("Screenshot 1: Subscribe alert - Done")
    
    # Screenshot 2: Add to Cart alert
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    async def handle_dialog2(dialog):
        await page.screenshot(path=f'{screenshots_dir}/02_add_to_cart_alert.png')
        await dialog.accept()
    page.on('dialog', handle_dialog2)
    await page.goto(f'file://{site_dir}/gallery.html')
    await page.wait_for_timeout(1000)
    await page.evaluate("document.querySelector('.add-to-cart-btn').click();")
    await page.wait_for_timeout(1000)
    await page.close()
    print("Screenshot 2: Add to Cart alert - Done")
    
    # Screenshot 3: Clear Cart alert
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    async def handle_dialog3(dialog):
        await page.screenshot(path=f'{screenshots_dir}/03_clear_cart_alert.png')
        await dialog.accept()
    page.on('dialog', handle_dialog3)
    await page.goto(f'file://{site_dir}/gallery.html')
    await page.wait_for_timeout(1000)
    await page.evaluate("document.getElementById('clear-cart-btn').click();")
    await page.wait_for_timeout(1000)
    await page.close()
    print("Screenshot 3: Clear Cart alert - Done")
    
    # Screenshot 4: Process Order alert
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    async def handle_dialog4(dialog):
        await page.screenshot(path=f'{screenshots_dir}/04_process_order_alert.png')
        await dialog.accept()
    page.on('dialog', handle_dialog4)
    await page.goto(f'file://{site_dir}/gallery.html')
    await page.wait_for_timeout(1000)
    await page.evaluate("document.getElementById('process-order-btn').click();")
    await page.wait_for_timeout(1000)
    await page.close()
    print("Screenshot 4: Process Order alert - Done")
    
    # Screenshot 5: Submit alert on About page
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    async def handle_dialog5(dialog):
        await page.screenshot(path=f'{screenshots_dir}/05_submit_alert.png')
        await dialog.accept()
    page.on('dialog', handle_dialog5)
    await page.goto(f'file://{site_dir}/about.html')
    await page.wait_for_timeout(1000)
    await page.fill('#name', 'Test User')
    await page.fill('#email', 'test@example.com')
    await page.evaluate("document.getElementById('submit-btn').click();")
    await page.wait_for_timeout(1000)
    await page.close()
    print("Screenshot 5: Submit alert - Done")
    
    # Screenshot 6: Validation message (no alert - just HTML validation)
    page = await browser.new_page(viewport={'width': 1280, 'height': 800})
    await page.goto(f'file://{site_dir}/about.html')
    await page.wait_for_timeout(1000)
    await page.evaluate("document.getElementById('submit-btn').click();")
    await page.wait_for_timeout(1000)
    await page.screenshot(path=f'{screenshots_dir}/06_validation_message.png')
    await page.close()
    print("Screenshot 6: Validation message - Done")
    
    # Full page screenshots
    for page_name in ['index', 'gallery', 'about', 'events']:
        page = await browser.new_page(viewport={'width': 1280, 'height': 800})
        await page.goto(f'file://{site_dir}/{page_name}.html')
        await page.wait_for_timeout(1000)
        await page.screenshot(path=f'{screenshots_dir}/page_{page_name}.png', full_page=True)
        await page.close()
        print(f"Full page screenshot: {page_name} - Done")
    
    await browser.close()
    await p.stop()
    print("\nAll screenshots captured!")

asyncio.run(main())
