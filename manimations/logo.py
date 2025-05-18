from manim import *
import numpy as np
from PIL import Image

def mandelbrot(width, height, x_min, x_max, y_min, y_max, max_iter=100):
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

            color = 255 - int(iteration * 255 / max_iter)
            image[row, col] = (color, color, color)  # grayscale

    return image


class Logo(Scene):
    def construct(self):
        # Parameters
        width, height = 800, 800
        x_min, x_max = -2.0, 1.0
        y_min, y_max = -1.5, 1.5

        # Generate Mandelbrot set as NumPy array
        data = mandelbrot(width, height, x_min, x_max, y_min, y_max, max_iter=100)
        img = Image.fromarray(data, mode="RGB")

        # Save temporarily (optional, for debugging or saving output)
        img.save("mandelbrot.png")

        # Create Manim ImageMobject
        mandelbrot_img = ImageMobject(img)
        mandelbrot_img.scale(6.5 / mandelbrot_img.get_width())  # fit scene

        self.add(mandelbrot_img)
        self.wait()
