from manim import *

class Motivation(Scene):
    def construct(self):
        # Load hillshade image
        hillshade = ImageMobject("assets/hillshade-example.png")
        hillshade.scale_to_fit_height(6)
        self.play(FadeIn(hillshade))
        self.wait(1)

        # Highlight flat area
        flat_dot = Dot([-2, 1.5, 0], color=GREEN)
        flat_label = Text("Flat", font_size=24).next_to(flat_dot, RIGHT)
        self.play(GrowFromCenter(flat_dot), Write(flat_label))
        self.wait(1)

        # Highlight steep slope
        steep_dot = Dot([1, -0.5, 0], color=RED)
        steep_label = Text("Steep", font_size=24).next_to(steep_dot, RIGHT)
        self.play(GrowFromCenter(steep_dot), Write(steep_label))
        self.wait(1)

        # Highlight gully
        gully_arrow = Arrow(start=[-1.5, -2, 0], end=[-1.0, -1.2, 0], color=BLUE)
        gully_label = Text("Gully", font_size=24).next_to(gully_arrow, DOWN)
        self.play(GrowArrow(gully_arrow), Write(gully_label))
        self.wait(1)

        # Highlight ridgeline
        ridge_arrow = Arrow(start=[0.5, 2, 0], end=[1.2, 1.5, 0], color=YELLOW)
        ridge_label = Text("Ridgeline", font_size=24).next_to(ridge_arrow, UP)
        self.play(GrowArrow(ridge_arrow), Write(ridge_label))
        self.wait(1)

        # Highlight rugged area
        rugged_rect = SurroundingRectangle(hillshade, color=ORANGE, buff=0.2)
        rugged_rect.scale(0.3).move_to([-2.5, -1.2, 0])
        rugged_label = Text("Rugged", font_size=24).next_to(rugged_rect, DOWN)
        self.play(Create(rugged_rect), Write(rugged_label))
        self.wait(1)

        # Highlight smoother slope
        smooth_rect = SurroundingRectangle(hillshade, color=TEAL, buff=0.2)
        smooth_rect.scale(0.3).move_to([2, 0.5, 0])
        smooth_label = Text("Smooth", font_size=24).next_to(smooth_rect, DOWN)
        self.play(Create(smooth_rect), Write(smooth_label))
        self.wait(1)

        # Fade out all highlights
        self.play(*[FadeOut(mob) for mob in self.mobjects])
        self.wait()