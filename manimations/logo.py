from manim import *
from manim.opengl import *
import cupy as cp
from PIL import Image

# def cpu_mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(0, 0, 0), max_iter=500):
#     """Generate a Mandelbrot set image as a NumPy array."""
#     image = np.zeros((height, width, 3), dtype=np.uint8)

#     for row in range(height):
#         for col in range(width):
#             x = x_min + (x_max - x_min) * col / width
#             y = y_min + (y_max - y_min) * row / height
#             c = complex(x, y)
#             z = 0
#             iteration = 0
#             while abs(z) <= 2 and iteration < max_iter:
#                 z = z**2 + c
#                 iteration += 1

#             if abs(z) <= 2:
#                 image[row, col] = contained_color
#             else:
#                 color = int(iteration * 255 / max_iter)
#                 image[row, col] = (color, color, color)  # grayscale

#     return image

# # TODO (stouff) possibly add supersampling?
# def generate_frame(width, height, t):
#     aspect_ratio = height / width
#     x_min, x_max = -3.333333333, 1.0
#     half_height = 0.5 * (x_max - x_min) * aspect_ratio
#     y_min, y_max = -half_height, half_height

#     v = t * 255 
#     return cpu_mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(v, v, v), max_iter=10)

# # Set the parameters for the Mandelbrot Set
# WIDTH, HEIGHT = 600, 600  # Resolution of the image
# MAX_ITER = 100  # Max iterations to determine if a point is in the Mandelbrot set
# ZOOM = 1  # Zoom level (larger = zoomed out)
# CENTER_X, CENTER_Y = -0.5, 0  # The center of the set

# # Function to compute whether a point is in the Mandelbrot set
# def mandelbrot(c, max_iter=MAX_ITER):
#     z = c
#     for n in range(max_iter):
#         if abs(z) > 2:
#             return n
#         z = z*z + c
#     return max_iter

# # Create a Manim Scene
# class Logo(Scene):
#     def construct(self):
#         # Create a grid of points for the Mandelbrot Set calculation
#         x_vals = cp.linspace(CENTER_X - 2.0, CENTER_X + 2.0, WIDTH)
#         y_vals = cp.linspace(CENTER_Y - 2.0, CENTER_Y + 2.0, HEIGHT)
        
#         # Create a complex grid (X, Y) for the Mandelbrot set
#         X, Y = cp.meshgrid(x_vals, y_vals)
#         Z = X + 1j * Y 
        
#         # Calculate the Mandelbrot set on the GPU
#         mandelbrot_result = cp.vectorize(lambda c: mandelbrot(c))(Z)
        
#         # Convert Mandelbrot result to a 2D grayscale image (normalized)
#         mandelbrot_image = cp.log(mandelbrot_result + 1)  # Apply log scale for visibility
#         mandelbrot_image /= mandelbrot_image.max()  # Normalize to [0, 1]
        
#         # Create an image from the result and display it
#         image = ImageMobject(mandelbrot_image.get())
#         image.scale(3)
        
#         self.add(image)
#         self.wait(2)

def cuda_mandelbrot(width, height, x_min, x_max, y_min, y_max, contained_color=(0, 0, 0), max_iter=500):
    x = cp.linspace(0, 1, width)
    y = cp.linspace(0, 1, height)
    xv, yv = cp.meshgrid(x, y)

    red = xv
    green = yv
    blue = cp.sqrt(xv * yv)

    image_gpu = cp.stack([red, green, blue], axis=2)  # Shape: (H, W, 3)
    image_gpu = (image_gpu * 255).clip(0, 255).astype(cp.uint8)

    return image_gpu.get()  # Transfer to CPU as NumPy array

def numpy_to_pil_image(np_array):
    return Image.fromarray(np_array, mode='RGB')  # Assumes shape (H, W, 3)

class Logo(Scene):
    def construct(self):# Parameters
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