from manim import *
import config

class EffectGraph(Scene):

    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/directional-lighting-0.m4a")

        self.wait(7)
        directional_lighting_section = Text("I. Directional Lighting", font_size=32, color=WHITE)
        self.play(Write(directional_lighting_section))
        self.wait(1)

        self.play(directional_lighting_section.animate.move_to([0, 3.25, 0]))

        self.wait(16)

        # Axes without ticks
        axes = Axes(
            x_range=[0, 12],
            y_range=[0, 6],
            x_length=12,
            y_length=6,
            axis_config={"include_tip": True, "include_ticks": False},
        )

        self.play(Create(axes))

        # Custom axis labels
        realism_label = Text("realism", font_size=28).next_to(axes.y_axis, LEFT).rotate(PI / 2)
        realism_label.move_to(axes.y_axis.get_center() + LEFT * 0.4)

        cost_label = Text("cost (complexity/memory/time)", font_size=24)
        cost_label.next_to(axes.x_axis, DOWN)
        cost_label.move_to(axes.x_axis.get_center() + DOWN * 0.4)

        self.play(Write(realism_label))
        
        self.wait(3)
        self.play(Write(cost_label))

        # Lighting techniques (fade in one-by-one, directional last)
        labels_and_points = [
            ("shadow maps", [8, 5]),
            ("ambient light", [0.75, 0.75]),
            ("ray tracing", [10, 5.75]),
            ("directional lights", [1.75, 4]),  # shown last
        ]

        for label, (x, y) in labels_and_points[:-1]:  # all but directional light
            dot = Dot(axes.c2p(x, y), color=YELLOW)
            text = Text(label, font_size=28).next_to(dot, RIGHT + 0.125 * DOWN, buff=0.1)
            self.play(FadeIn(dot), Write(text))
            self.wait(0.3)

        # Directional light: last and highlighted
        label, (x, y) = labels_and_points[-1]
        directional_dot = Dot(axes.c2p(x, y), color=YELLOW)
        directional_text = Text(label, font_size=28).next_to(directional_dot, RIGHT + 0.125 * DOWN, buff=0.1)

        self.play(FadeIn(directional_dot), Write(directional_text))
        self.wait(0.5)

        # Highlight the sweet spot
        circle = Circle(radius=0.2, color=GREEN).move_to(directional_dot)

        self.play(Create(circle))
        self.wait(2)