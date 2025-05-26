from manim import *
import cupy as cp
from PIL import Image

def mobius_rotation(phi):
    """Return a Möbius transformation matrix rotating by angle phi around the great circle on the Riemann sphere that is the real axis"""
    a = np.cos(phi / 2)
    b = np.sin(phi / 2)
    c = -np.sin(phi / 2)
    d = np.cos(phi / 2)
    return (a, b, c, d)

def window(width, height, x_min, x_max, y_min, y_max, phi):
    # Define the region of the complex plane to visualize
    re = cp.linspace(x_min, x_max, width)
    im = cp.linspace(y_min, y_max, height)
    z = re[cp.newaxis, :] + 1j * im[:, cp.newaxis]
    a, b, c, d = mobius_rotation(phi)
    return (a * z + b) / (c * z + d)

def cuda_mandelbrot(width, height, x_min, x_max, y_min, y_max, phi, max_iter=500):
    # Define the region of the complex plane to visualize
    c = window(width, height, x_min, x_max, y_min, y_max, phi)

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

def get_supersample_from_quality():
    quality = config["quality"]  # "low_quality", "medium_quality", etc.
    
    return {
        "low_quality": 2,
        "medium_quality": 2,
        "high_quality": 3,
        "fourk_quality": 4,
    }.get(quality, 1)  # Default to 1 if unknown

class Logo(Scene):
    def construct(self):
        width, height = config["pixel_width"], config["pixel_height"]
        aspect_ratio = height / width
        x_min, x_max = -3.333333333, 1.0
        half_height = 0.5 * (x_max - x_min) * aspect_ratio
        y_min, y_max = -half_height, half_height
        supersample = get_supersample_from_quality()

        max_iter_tracker = ValueTracker(5)
        phi_tracker = ValueTracker(0)

        def get_state():
            return {
                "max_iter": int(max_iter_tracker.get_value()),
                "phi": phi_tracker.get_value()
            }

        def compute_fractal_array():
            state = get_state()
            w = supersample * width
            h = supersample * height
            supersampled_array = cuda_mandelbrot(w, h, x_min, x_max, y_min, y_max, state["phi"], max_iter=state["max_iter"])
            pil_img = Image.fromarray(supersampled_array, mode='RGBA')
            downscaled_img = pil_img.resize((width, height), Image.LANCZOS)
            return np.array(downscaled_img)

        image = ImageMobject(Image.fromarray(compute_fractal_array(), mode='RGBA'))
        image.stretch_to_fit_width(config["frame_width"])  # fit scene
        image.stretch_to_fit_height(config["frame_height"])  # fit scene
        self.add(image)
        self.play(FadeIn(image, run_time=2))

        last_state = [None] # use a list to work around a Python scoping issue
        def update_fractal(mob, dt):
            current_state = get_state()
            if not last_state[0] or current_state != last_state[0]:
                mob.set_pixel_array(compute_fractal_array())
                last_state[0] = current_state

        image.add_updater(update_fractal)

        self.play(
            max_iter_tracker.animate.set_value(100),
            phi_tracker.animate.set_value(-np.pi),
            run_time=2.5,
            rate_func=rush_into
        )
        self.wait()