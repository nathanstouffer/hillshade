from manim import *
import numpy as np
from PIL import Image

def mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(0, 0, 0), max_iter=500):
    """Generate a Mandelbrot set image as a NumPy array."""
    image = np.zeros((height, width, 3), dtype=np.uint8)

    for row in range(height):
        for col in range(width):
            x = x_min + (x_max - x_min) * col / width
            y = y_min + (y_max - y_min) * row / height
            c = complex(x, y)
            z = 0
            iteration = 0
            while abs(z) <= 2 and iteration < max_iter:
                z = z**2 + c
                iteration += 1

            if abs(z) <= 2:
                image[row, col] = contained_color
            else:
                color = int(iteration * 255 / max_iter)
                image[row, col] = (color, color, color)  # grayscale

    return image


class Logo(Scene):
    def construct(self):
        # Parameters
        width, height = config["pixel_width"], config["pixel_height"]
        aspect_ratio = height / width
        x_min, x_max = -3.333333333, 1.0
        half_height = 0.5 * (x_max - x_min) * aspect_ratio
        y_min, y_max = -half_height, half_height

        fps = config["frame_rate"]

        # Generate Mandelbrot set as NumPy array
        data = mandelbrot(width, height, x_min, x_max, y_min, y_max, max_iter=100)
        img = Image.fromarray(data, mode="RGB")

        # Save temporarily (optional, for debugging or saving output)
        os.mkdir(f"{config.media_dir}/frames")
        img.save(f"{config.media_dir}/frames/mandelbrot.png")

        # Create Manim ImageMobject
        mandelbrot_img = ImageMobject(img)
        mandelbrot_img.stretch_to_fit_width(config["frame_width"])  # fit scene
        mandelbrot_img.stretch_to_fit_height(config["frame_height"])  # fit scene
        self.add(mandelbrot_img)

        self.wait()
