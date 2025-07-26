from manim import *

class Motivation(Scene):
    def construct(self):
        # Load and scale image
        hillshade = ImageMobject("assets/hillshade-example.png")
        hillshade.scale_to_fit_height(6)
        self.play(FadeIn(hillshade))
        self.wait(1)

        def add_ellipse_label(pos, dim, rotation, text, label_pos, arc):
            # Ellipse
            ellipse = Ellipse(width=dim[0], height=dim[1], color=GREEN)
            ellipse.rotate(rotation)
            ellipse.move_to(pos)

            # Label (placed in margin)
            label = Text(text, font_size=24, color=WHITE).move_to(label_pos)

            # Direction vectors
            to_label = label.get_center() - ellipse.get_center()
            to_label = to_label / np.linalg.norm(to_label)

            padding = 0.1

            # Edge centers of label bounding box
            top_center    = (label.get_corner(UP + LEFT) + label.get_corner(UP + RIGHT)) / 2
            bottom_center = (label.get_corner(DOWN + LEFT) + label.get_corner(DOWN + RIGHT)) / 2
            left_center   = (label.get_corner(UP + LEFT) + label.get_corner(DOWN + LEFT)) / 2
            right_center  = (label.get_corner(UP + RIGHT) + label.get_corner(DOWN + RIGHT)) / 2

            # Direction to ellipse
            direction = ellipse.get_center() - label.get_center()

            # Pick edge center based on dominant direction
            if abs(direction[0]) > abs(direction[1]):
                start_point = right_center if direction[0] > 0 else left_center
            else:
                start_point = top_center if direction[1] > 0 else bottom_center

            to_center = label.get_center() - start_point
            to_center /= np.linalg.norm(to_center)

            # Curved arrow from label to ellipse
            opacity = 0.85
            arrow = CurvedArrow(
                start_point=start_point - padding * to_center,
                end_point=ellipse.get_boundary_point(to_label) + padding * to_label,
                color=ORANGE,
                stroke_opacity=opacity,
                angle=arc  # adjust angle for curve shape
            )
            arrow.tip.set_opacity(opacity)

            self.play(FadeIn(ellipse), Write(label), FadeIn(arrow))
            #self.add(ellipse, label, arrow)

        # --- Call with explicit label positions in the margin ---
        self.wait(0.5)
        add_ellipse_label([2.35, 0.35, 0], [1.25, 1], PI / 6, "Flat", [4.5, 0, 0], -PI / 8)
        self.wait(0.5)
        add_ellipse_label([-1.75, -2.1, 0], [1.75, 1], -PI / 6, "Steep", [-4.5, -2.5, 0], PI / 4)
        self.wait(0.5)
        add_ellipse_label([0.675, 0, 0], [1, 0.5], PI / 6, "Gully", [4.5, 2.5, 0], PI / 4)
        self.wait(0.5)
        add_ellipse_label([-1.8, 1.9, 0], [2, 0.75], -PI / 96, "Ridgeline", [-5, 3, 0], -PI / 6)
        self.wait(0.5)
        add_ellipse_label([-1.75, 0.5, 0], [1.5, 1.25], -PI / 6, "Rugged", [-4.75, 0.5, 0], PI / 12)
        self.wait(0.5)
        add_ellipse_label([2.35, -2.5, 0], [1, 0.9], 0, "Even", [4.5, -2, 0], PI / 12)
        self.wait(1.5)