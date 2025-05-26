from manim import *
import cupy as cp
from PIL import Image

def cuda_mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(0, 0, 0), max_iter=500):
    # Define the region of the complex plane to visualize
    re = cp.linspace(x_min, x_max, width)
    im = cp.linspace(y_min, y_max, height)
    c = re[cp.newaxis, :] + 1j * im[:, cp.newaxis]

    # Initialize z to zero and an output array to hold iteration counts
    z = cp.zeros_like(c)
    div_time = cp.zeros(c.shape, dtype=cp.int32)

    # Compute Mandelbrot iterations
    for i in range(max_iter):
        mask = cp.abs(z) <= 2
        z[mask] = z[mask] * z[mask] + c[mask]
        div_time[mask & (cp.abs(z) > 2)] = i

    # Normalize and scale to 0–255 grayscale
    img = (div_time / max_iter * 255).astype(cp.uint8)
    img_rgb = cp.stack([img] * 3, axis=2)  # Convert to RGB

    # Create alpha channel (fully opaque)
    alpha_channel = cp.full((height, width, 1), 255, dtype=cp.uint8)

    # Concatenate alpha channel to RGB to form RGBA
    img_rgba = cp.concatenate([img_rgb, alpha_channel], axis=2)

    # Move data back to CPU and return as NumPy array
    return img_rgba.get()

def numpy_to_pil_image(np_array): 
    return Image.fromarray(np_array, mode='RGBA')  # Assumes shape (H, W, 3)

class Logo(Scene):
    def construct(self):
        width, height = config["pixel_width"], config["pixel_height"]
        aspect_ratio = height / width
        x_min, x_max = -3.333333333, 1.0
        half_height = 0.5 * (x_max - x_min) * aspect_ratio
        y_min, y_max = -half_height, half_height

        max_iter_tracker = ValueTracker(1)

        def compute_fractal_array():
            max_iter = int(max_iter_tracker.get_value())
            image_array = cuda_mandelbrot(width, height, x_min, x_max, y_min, y_max, max_iter=max_iter)
            return image_array

        image = ImageMobject(numpy_to_pil_image(compute_fractal_array()))
        image.stretch_to_fit_width(config["frame_width"])  # fit scene
        image.stretch_to_fit_height(config["frame_height"])  # fit scene
        self.add(image)
        self.play(FadeIn(image, run_time=2))

        last_iter = [None] # use a list to work around a Python scoping issue
        def update_fractal(mob, dt):
            current_iter = int(max_iter_tracker.get_value())
            if not last_iter[0] or current_iter != last_iter[0]:
                print(last_iter[0], current_iter)
                mob.set_pixel_array(compute_fractal_array())
                last_iter[0] = current_iter

        image.add_updater(update_fractal)

        self.play(max_iter_tracker.animate.set_value(10), run_time=2.5, rate_func=rush_into)
        self.wait()