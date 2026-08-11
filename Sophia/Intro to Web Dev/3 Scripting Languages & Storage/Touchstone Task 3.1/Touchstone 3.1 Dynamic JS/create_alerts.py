from PIL import Image, ImageDraw, ImageFont
import os

screenshots_dir = '/mnt/agents/output/screenshots'

ALERT_BG = '#F0F0F0'
ALERT_BORDER = '#A0A0A0'
BUTTON_BG = '#E1E1E1'
BUTTON_BORDER = '#ADADAD'
TEXT_COLOR = '#000000'
TITLE_BAR_BG = '#FFFFFF'

def create_alert_screenshot(page_screenshot_path, output_path, message, dialog_width=420, dialog_height=160):
    page_img = Image.open(page_screenshot_path)
    img_width, img_height = page_img.size
    
    viewport_width = 1280
    viewport_height = 800
    
    left = max(0, (img_width - viewport_width) // 2)
    top = max(0, (img_height - viewport_height) // 2)
    right = min(img_width, left + viewport_width)
    bottom = min(img_height, top + viewport_height)
    
    if img_width <= viewport_width:
        left = 0
        right = img_width
    if img_height <= viewport_height:
        top = 0
        bottom = img_height
        
    page_crop = page_img.crop((left, top, right, bottom))
    
    overlay = Image.new('RGBA', page_crop.size, (0, 0, 0, 80))
    page_with_overlay = Image.alpha_composite(page_crop.convert('RGBA'), overlay)
    
    dialog_x = (page_crop.size[0] - dialog_width) // 2
    dialog_y = (page_crop.size[1] - dialog_height) // 2
    
    draw = ImageDraw.Draw(page_with_overlay)
    
    shadow_offset = 4
    draw.rectangle(
        [dialog_x + shadow_offset, dialog_y + shadow_offset, 
         dialog_x + dialog_width + shadow_offset, dialog_y + dialog_height + shadow_offset],
        fill=(0, 0, 0, 60)
    )
    
    draw.rectangle(
        [dialog_x, dialog_y, dialog_x + dialog_width, dialog_y + dialog_height],
        fill=ALERT_BG,
        outline=ALERT_BORDER,
        width=1
    )
    
    title_bar_height = 30
    draw.rectangle(
        [dialog_x, dialog_y, dialog_x + dialog_width, dialog_y + title_bar_height],
        fill=TITLE_BAR_BG
    )
    
    try:
        font_message = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14)
        font_button = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 13)
    except:
        font_message = ImageFont.load_default()
        font_button = ImageFont.load_default()
    
    text_bbox = draw.textbbox((0, 0), message, font=font_message)
    text_width = text_bbox[2] - text_bbox[0]
    text_x = dialog_x + (dialog_width - text_width) // 2
    text_y = dialog_y + 55
    draw.text((text_x, text_y), message, fill=TEXT_COLOR, font=font_message)
    
    button_width = 75
    button_height = 26
    button_x = dialog_x + (dialog_width - button_width) // 2
    button_y = dialog_y + dialog_height - 45
    
    draw.rectangle(
        [button_x + 1, button_y + 1, button_x + button_width + 1, button_y + button_height + 1],
        fill=(200, 200, 200, 180)
    )
    draw.rectangle(
        [button_x, button_y, button_x + button_width, button_y + button_height],
        fill=BUTTON_BG,
        outline=BUTTON_BORDER,
        width=1
    )
    
    ok_text = "OK"
    ok_bbox = draw.textbbox((0, 0), ok_text, font=font_button)
    ok_width = ok_bbox[2] - ok_bbox[0]
    ok_x = button_x + (button_width - ok_width) // 2
    ok_y = button_y + 5
    draw.text((ok_x, ok_y), ok_text, fill=TEXT_COLOR, font=font_button)
    
    final = page_with_overlay.convert('RGB')
    final.save(output_path, 'PNG')
    print(f"Created: {output_path}")

# Create cropped source images
index_img = Image.open(f'{screenshots_dir}/page_index.png')
index_width, index_height = index_img.size
footer_crop = index_img.crop((0, index_height - 400, min(1280, index_width), index_height))
footer_crop.save(f'{screenshots_dir}/footer_crop.png')

gallery_img = Image.open(f'{screenshots_dir}/page_gallery.png')
gw, gh = gallery_img.size
mid_crop = gallery_img.crop((0, 200, min(1280, gw), min(800, gh)))
mid_crop.save(f'{screenshots_dir}/gallery_crop.png')

about_img = Image.open(f'{screenshots_dir}/page_about.png')
aw, ah = about_img.size
form_crop = about_img.crop((0, ah - 700, min(1280, aw), ah))
form_crop.save(f'{screenshots_dir}/form_crop.png')

alerts = [
    ('footer_crop.png', '01_subscribe_alert.png', 'Thank you for subscribing.'),
    ('gallery_crop.png', '02_add_to_cart_alert.png', 'Item added to the cart.'),
    ('gallery_crop.png', '03_clear_cart_alert.png', 'Cart cleared.'),
    ('gallery_crop.png', '04_process_order_alert.png', 'Thank you for your order.'),
    ('form_crop.png', '05_submit_alert.png', 'Thank you for your message.'),
]

for source, output, message in alerts:
    create_alert_screenshot(
        f'{screenshots_dir}/{source}',
        f'{screenshots_dir}/{output}',
        message
    )

print("All alert screenshots created!")
