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

def frame_name(i):
    return f"{i:0004d}"

# TODO (stouff) possibly add supersampling
def generate_frames(width, height, fps, duration):
    aspect_ratio = height / width
    x_min, x_max = -3.333333333, 1.0
    half_height = 0.5 * (x_max - x_min) * aspect_ratio
    y_min, y_max = -half_height, half_height

    frames_dir = f"{config.media_dir}/logo_frames_{width}x{height}_{fps}fps_{duration}s"
    if not os.path.isdir(frames_dir):
        os.mkdir(frames_dir)

    num_frames = duration * fps

    data = mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(0, 0, 0), max_iter=100)
    img = Image.fromarray(data, mode="RGB")
    img.save(f"{frames_dir}/{frame_name(0)}.png")

    data = mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(255, 255, 255), max_iter=100)
    img = Image.fromarray(data, mode="RGB")
    img.save(f"{frames_dir}/{frame_name(1)}.png")

    return frames_dir, 2


class Logo(Scene):
    def construct(self):
        # Parameters
        width, height = config["pixel_width"], config["pixel_height"]
        aspect_ratio = height / width
        x_min, x_max = -3.333333333, 1.0
        half_height = 0.5 * (x_max - x_min) * aspect_ratio
        y_min, y_max = -half_height, half_height

        fps = config["frame_rate"]
        duration = 2

        frames_dir, num_frames = generate_frames(width, height, fps, duration)
        w = config["frame_width"]
        h = config["frame_height"]
        images = [ 
            ImageMobject(f"{frames_dir}/{frame_name(i)}.png").stretch_to_fit_width(w).stretch_to_fit_height(h)
            for i in range(num_frames)
        ]

        current = images[0]
        self.add(current)

        for next_img in images[1:]:
            next_img.move_to(current)
            self.play(Transform(current, next_img), run_time=1/fps)

        self.wait()
