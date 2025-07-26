from manim import *

class Motivation(Scene):
    def construct(self):
        # # Load hillshade image
        # hillshade = ImageMobject("assets/hillshade-example.png")
        # hillshade.scale_to_fit_height(6)
        # self.play(FadeIn(hillshade))
        # self.wait(1)

        # # Highlight flat area
        # flat_dot = Dot([-2, 1.5, 0], color=GREEN)
        # flat_label = Text("Flat", font_size=24).next_to(flat_dot, RIGHT)
        # self.play(GrowFromCenter(flat_dot), Write(flat_label))
        # self.wait(1)

        # # Highlight steep slope
        # steep_dot = Dot([1, -0.5, 0], color=RED)
        # steep_label = Text("Steep", font_size=24).next_to(steep_dot, RIGHT)
        # self.play(GrowFromCenter(steep_dot), Write(steep_label))
        # self.wait(1)

        # # Highlight gully
        # gully_arrow = Arrow(start=[-1.5, -2, 0], end=[-1.0, -1.2, 0], color=BLUE)
        # gully_label = Text("Gully", font_size=24).next_to(gully_arrow, DOWN)
        # self.play(GrowArrow(gully_arrow), Write(gully_label))
        # self.wait(1)

        # # Highlight ridgeline
        # ridge_arrow = Arrow(start=[0.5, 2, 0], end=[1.2, 1.5, 0], color=YELLOW)
        # ridge_label = Text("Ridgeline", font_size=24).next_to(ridge_arrow, UP)
        # self.play(GrowArrow(ridge_arrow), Write(ridge_label))
        # self.wait(1)

        # # Highlight rugged area
        # rugged_rect = SurroundingRectangle(hillshade, color=ORANGE, buff=0.2)
        # rugged_rect.scale(0.3).move_to([-2.5, -1.2, 0])
        # rugged_label = Text("Rugged", font_size=24).next_to(rugged_rect, DOWN)
        # self.play(Create(rugged_rect), Write(rugged_label))
        # self.wait(1)

        # # Highlight smoother slope
        # smooth_rect = SurroundingRectangle(hillshade, color=TEAL, buff=0.2)
        # smooth_rect.scale(0.3).move_to([2, 0.5, 0])
        # smooth_label = Text("Smooth", font_size=24).next_to(smooth_rect, DOWN)
        # self.play(Create(smooth_rect), Write(smooth_label))
        # self.wait(1)

        # # Fade out all highlights
        # self.play(*[FadeOut(mob) for mob in self.mobjects])
        # self.wait()

        # Load and scale image
        hillshade = ImageMobject("assets/hillshade-example.png")
        hillshade.scale_to_fit_height(6)
        self.add(hillshade)

        def add_ellipse_label(pos, dim, rotation, text):
            ellipse = Ellipse(width=dim[0], height=dim[1], color=GREEN)
            ellipse.rotate(rotation)
            ellipse.move_to(pos)
            label = Text(text, font_size=24).next_to(ellipse, UP)
            self.add(ellipse, label)

        # Flat
        add_ellipse_label([2.35, 0.35, 0], [1.25, 1], PI / 6, "Flat")

        # Steep
        add_ellipse_label([-1.75, -2.1, 0], [1.75, 1], -PI / 6, "Steep")

        # Gully
        add_ellipse_label([0.675, 0, 0], [1, 0.5], PI / 6, "Gully")

        # Ridgeline
        add_ellipse_label([-1.8, 1.9, 0], [2, 0.75], -PI / 96, "Ridgeline")

        # Rugged
        add_ellipse_label([-1.75, 0.5, 0], [1.5, 1.25], -PI / 6, "Rugged")

        # Even
        add_ellipse_label([2.35, -2.5, 0], [1, 0.9], 0, "Even")