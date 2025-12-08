import os
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib.patches as patches

target_size = (127, 102)  # width x height
crop_size_1 = (120, 95)
crop_size_2 = (84, 56)

luma_limit_upper = 1.0
luma_limit_lower = 0.8

chroma_limit_upper = 0.33+0.1
chroma_limit_lower = 0.33-0.1

colors = ['#cc0066', '#00a0d7']

def calculate_luminance(rgb_image):
    r, g, b = rgb_image[..., 0], rgb_image[..., 1], rgb_image[..., 2]
    #luma = 0.2126 * r + 0.7152 * g + 0.0722 * b
    luma = 0.3333 * r + 0.3333 * g + 0.3333 * b
    return luma / np.max(luma)

def calculate_chromaticity(rgb_image):
    rgb = rgb_image.astype(float)
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    total = r + g + b + 1e-10
    x = r / total
    y = g / total
    return x, y

def center_crop(image_np, crop_size):
    h, w = image_np.shape[:2]
    ch, cw = crop_size[1], crop_size[0]
    start_y = (h - ch) // 2
    start_x = (w - cw) // 2
    return image_np[start_y:start_y + ch, start_x:start_x + cw]

def cct_to_xy(T):
    """Approximate CIE 1931 xy chromaticity for a given color temperature T in Kelvin."""
    if T <= 4000:
        x = -0.2661239e9 / T**3 - 0.2343580e6 / T**2 + 0.8776956e3 / T + 0.179910
    else:
        x = -3.0258469e9 / T**3 + 2.1070379e6 / T**2 + 0.2226347e3 / T + 0.240390
    y = -1.1063814 * x**3 - 1.34811020 * x**2 + 2.18555832 * x - 0.20219683
    return x, y

def planckian_locus_xy(num_points=100):
    """Returns a list of (x, y) chromaticity coordinates along the Planckian locus."""
    # Approximate CIE 1931 xy from correlated color temperature (CCT)
    temps = np.linspace(1000, 20000, num_points)
    x_vals = []
    y_vals = []

    for T in temps:
        x, y = cct_to_xy(T)
        x_vals.append(x)
        y_vals.append(y)

    return x_vals, y_vals

def process_images_in_directory():
    output_dir = 'plots'
    os.makedirs(output_dir, exist_ok=True)

    plt.rcParams['font.family'] = 'sans-serif'  # or 'sans-serif', 'monospace', or a specific font name
    plt.rcParams['font.sans-serif'] = ['Barlow']  # Specify a list of fonts to try for 'serif' family\
    plt.rcParams['font.size'] = 12

    image_files = [f for f in os.listdir('.') if f.lower().endswith('.jpg')]

    for img_file in image_files:
        print(f"Processing: {img_file}")

        # Load and resize
        image = Image.open(img_file).convert('RGB')
        image = image.resize(target_size)
        image_np = np.array(image) / 255.0

        # Cropped regions
        crop1 = center_crop(image_np, crop_size_1)
        crop2 = center_crop(image_np, crop_size_2)

        lum_crop1 = calculate_luminance(crop1)
        cx1, cy1 = calculate_chromaticity(crop1)

        lum_crop2 = calculate_luminance(crop2)
        cx2, cy2 = calculate_chromaticity(crop2)

        # Set up figure: 1 row, 3 columns
        fig, axs = plt.subplots(1, 3, figsize=(15, 5))
        base_name = os.path.splitext(os.path.basename(img_file))[0]
        fig.suptitle(f'{base_name}', fontsize=20)

        # 1. RGB image (100x72mm)
        ax = axs[0]
        ax.imshow(image_np)
        ax.set_title('RGB Image')
        ax.axis('off')

        for size, color, label in zip([crop_size_1, crop_size_2], colors, ['120x95mm', '84x56mm']):
            rect_w, rect_h = size
            start_x = (target_size[0] - rect_w) // 2
            start_y = (target_size[1] - rect_h) // 2
            rect = patches.Rectangle((start_x, start_y), rect_w, rect_h,
                                     linewidth=2, edgecolor=color, facecolor='none', label=label)
            ax.add_patch(rect)
            ax.legend(fontsize=10, loc='upper center', bbox_to_anchor=(0.5, -0.02), ncol=2)

        # 2. Luminance (75x56)
        im1 = axs[1].imshow(lum_crop1, cmap='viridis', vmin=luma_limit_lower, vmax=luma_limit_upper)
        axs[1].set_title('Normalized Luminance (120x95mm)')
        axs[1].axis('off')
        fig.colorbar(im1, ax=axs[1], shrink=0.7)

        # 4. Luminance (36x24)
        im2 = axs[2].imshow(lum_crop2, cmap='viridis', vmin=luma_limit_lower, vmax=luma_limit_upper)
        axs[2].set_title('Normalized Luminance (84x56mm)')
        axs[2].axis('off')
        fig.colorbar(im2, ax=axs[2], shrink=0.7)

        # Luminance stats
        lum1_min = lum_crop1.min()
        lum1_max = lum_crop1.max()
        lum1_std = lum_crop1.std()

        lum2_min = lum_crop2.min()
        lum2_max = lum_crop2.max()
        lum2_std = lum_crop2.std()

        # Save figure
        save_path = os.path.join(output_dir, f"luminance_plot_{base_name}.png")
        plt.tight_layout()
        axs[1].text(s=f'range = {(lum1_max-lum1_min):.2f}, stdev = {lum1_std:.3f}', x=0.5, y=-0.08, fontsize=10, transform=axs[1].transAxes, ha='center', va='bottom')
        axs[2].text(s=f'range = {(lum2_max-lum2_min):.2f}, stdev = {lum2_std:.3f}', x=0.5, y=-0.08, fontsize=10, transform=axs[2].transAxes, ha='center', va='bottom')
        plt.savefig(save_path, dpi=150)
        plt.close()
        print(f"Saved analysis to: {save_path}")

if __name__ == '__main__':
    process_images_in_directory()
