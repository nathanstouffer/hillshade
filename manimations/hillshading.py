from manim import *
import config

class ShadowArea(ThreeDScene):

    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/cosine-0.m4a")

        self.set_camera_orientation(phi=72.5 * DEGREES, theta=60 * DEGREES)

        fixed_arrow = Arrow(ORIGIN, DOWN, color=YELLOW, buff=0)
        fixed_arrow.to_corner(UL, buff=0.75)
        self.add_fixed_in_frame_mobjects(fixed_arrow)

        # Grey base rectangle (solid floor)
        base_rect = Rectangle(
            width=20, height=20,
            fill_color=GREY, fill_opacity=0.2,
            stroke_opacity=0
        )
        grid = NumberPlane(
            x_range=[-10, 10, 1],
            y_range=[-10, 10, 1],
            axis_config={"stroke_opacity": 0},
            background_line_style={
                "stroke_color": GRAY,
                "stroke_opacity": 0.5,
                "stroke_width": 1
            }
        )
        self.add(base_rect, grid)

        def compute_adjustment(old, new):
            theta = angle_between_vectors(old, new)
            rotation_axis = np.cross(old, new)
            return (theta, rotation_axis)

        normal_dir = normalize(np.array([0, 0, 1]))

        # Square
        square = Square(side_length=3, fill_opacity=0.7, stroke_opacity=1)
        angle, axis = compute_adjustment(OUT, normal_dir)
        square.rotate(angle, axis)
        square.shift(2*OUT)
        self.play(FadeIn(square))

        # Draw polygon of projected points in XY plane
        vertices = square.get_vertices()
        shadow = Polygon(*[np.array([pt[0], pt[1], 0]) for pt in vertices], color=BLACK, fill_opacity=0.75)
        lines = [Line(corner, [corner[0], corner[1], 0], color=YELLOW, stroke_width=1, buff=0) for corner in vertices]
        self.play(FadeIn(shadow), AnimationGroup(*[Create(line) for line in lines]), lag_ratio=0)

        # similar vectors
        new_normal = normalize(np.array([-0.9, -0.7, 1]))
        angle, axis = compute_adjustment(normal_dir, new_normal)
        self.play(
            square.animate.rotate(angle, axis),
            run_time=3
        )
        normal_dir = new_normal