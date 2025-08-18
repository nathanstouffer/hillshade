from manim import *
import config

class Motivation(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/Motivation.m4a")
 
        # Load and scale image
        hillshade = ImageMobject("assets/bench-lakes.png")
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

        # --- Call with explicit label positions in the margin ---
        self.wait(4.0)
        add_ellipse_label([2.35, 0.35, 0], [1.25, 1], PI / 6, "Flat", [4.5, 0, 0], -PI / 8)
        self.wait(0.5)
        add_ellipse_label([-1.75, -2.1, 0], [1.75, 1], -PI / 6, "Steep", [-4.5, -2.5, 0], PI / 4)

        self.wait(2)
        add_ellipse_label([0.675, 0, 0], [1, 0.5], PI / 6, "Gully", [4.5, 2.5, 0], PI / 4)
        self.wait(0.5)
        add_ellipse_label([-1.8, 1.9, 0], [2, 0.75], -PI / 96, "Ridgeline", [-5, 3, 0], -PI / 6)

        self.wait(1.5)
        add_ellipse_label([-1.75, 0.5, 0], [1.5, 1.25], -PI / 6, "Rugged", [-4.75, 0.5, 0], PI / 12)
        self.wait(0.5)
        add_ellipse_label([2.35, -2.5, 0], [1, 0.9], 0, "Smooth", [4.5, -2, 0], PI / 12)

        self.wait(6.5)
        self.play(*[FadeOut(mob) for mob in self.mobjects if mob != hillshade], run_time=2)

class Topic(Scene):
    def construct(self): 
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/Topic.m4a")

        hillshade = ImageMobject("assets/bench-lakes.png")
        hillshade.scale_to_fit_height(6)
        self.add(hillshade)

        self.wait(1)
        self.play(hillshade.animate.move_to([0, -0.5, 0]))

        text = Text("Topic: Hillshade", font_size=32, color=WHITE).move_to([0, 3.125, 0])
        underline = Underline(text)
        self.play(Write(text))
        self.play(Write(underline))
        self.wait(1)

class TopicTester(Scene):
    def construct(self):
        directional_light_thumbnail = ImageMobject("assets/directional-light-thumbnail.png")
        derivation_thumbnail = ImageMobject("assets/derivation-thumbnail.png")
        hillshading_thumbnail = ImageMobject("assets/hillshading-thumbnail.png")

        width = 3.25
        directional_light_thumbnail.scale_to_fit_width(width).shift([-4.5, 1, 0])
        derivation_thumbnail.scale_to_fit_width(width).shift([0, -2, 0])
        hillshading_thumbnail.scale_to_fit_width(width).shift([4.5, 1, 0])

        directional_light_rectangle = SurroundingRectangle(directional_light_thumbnail, color=WHITE)
        derivation_rectangle = SurroundingRectangle(derivation_thumbnail, color=WHITE)
        hillshading_rectangle = SurroundingRectangle(hillshading_thumbnail, color=WHITE)

        directional_light_label = Text("I. Directional Lighting", font_size=20).next_to(directional_light_rectangle, UP)
        derivation_label = Text("II. Derivation", font_size=20).next_to(derivation_rectangle, UP)
        hillshading_label = Text("III. Hillshading", font_size=20).next_to(hillshading_rectangle, UP)

        directional_light_to_derivation = CurvedArrow(
            start_point=directional_light_thumbnail.get_bottom() + 0.5 * DOWN,
            end_point=derivation_rectangle.get_left() + 0.5 * LEFT,
            angle=PI / 3
        )

        derivation_to_hillshading = CurvedArrow(
            start_point=derivation_rectangle.get_right() + 0.5 * RIGHT,
            end_point=hillshading_thumbnail.get_bottom() + 0.5 * DOWN,
            angle=PI / 3
        )

        outline = [
            directional_light_thumbnail,
            directional_light_rectangle,
            directional_light_label,
            derivation_thumbnail,
            derivation_rectangle,
            derivation_label,
            hillshading_thumbnail,
            hillshading_rectangle,
            hillshading_label,
            directional_light_to_derivation,
            derivation_to_hillshading,
        ]

        self.add(*outline)