from manim import *
from manim.opengl import *
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
    return img_rgb.get()  # Return CPU NumPy array

def numpy_to_pil_image(np_array): 
    return Image.fromarray(np_array, mode='RGB')  # Assumes shape (H, W, 3)

class Logo(Scene):
    def construct(self):
        width, height = config["pixel_width"], config["pixel_height"]
        aspect_ratio = height / width
        x_min, x_max = -3.333333333, 1.0
        half_height = 0.5 * (x_max - x_min) * aspect_ratio
        y_min, y_max = -half_height, half_height

        image_array = cuda_mandelbrot(width, height, x_min, x_max, y_min, y_max, max_iter=100)
        image_pil = numpy_to_pil_image(image_array)  # NumPy -> PIL

        img_mobj = ImageMobject(image_pil)
        img_mobj.stretch_to_fit_width(config["frame_width"])  # fit scene
        img_mobj.stretch_to_fit_height(config["frame_height"])  # fit scene
        self.add(img_mobj)
        self.wait()