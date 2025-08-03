from manim import *
import config

class EffectGraph(Scene):

    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/directional-lighting-0.m4a")
            self.wait(6)

        self.wait(1)
        directional_lighting_section = Text("I. Directional Lighting", font_size=32, color=WHITE)
        self.play(Write(directional_lighting_section))
        self.wait(1)

        self.play(directional_lighting_section.animate.move_to([0, 3, 0]))

        self.wait(2.5)

        # Technique labels and their graph positions
        techniques = [
            ("ambient light", [0.75, 0.75]),
            ("shadow maps", [8, 5]),
            ("ray tracing", [10, 5.75]),
            ("directional lights", [1.75, 4]),
        ]

        # Create Text objects for the list
        technique_texts = [Text(label, font_size=28) for label, _ in techniques]

        # Left-align as a VGroup
        technique_list = VGroup(*technique_texts).arrange(DOWN, aligned_edge=LEFT, buff=0.4)
        technique_list.move_to(ORIGIN + 0.25 * DOWN)

        # Header
        header = Text("Techniques", font_size=30)
        header.next_to(technique_list, UP, buff=0.5)
        
        # Underline (line spans width of the technique list)
        underline = Line(
            start=technique_list.get_left() + 0.5 * LEFT,
            end=technique_list.get_right() + 0.5 * RIGHT,
            stroke_width=2
        )
        underline.next_to(header, DOWN, buff=0.1)

        # Show header, underline, and list one by one
        self.play(Write(header))
        self.play(Create(underline))
        self.wait(0.3)

        for text in technique_texts:
            self.play(Write(text))
            self.wait(0.3)

        self.wait(5.5)

        # Construct axes behind the list
        axes = Axes(
            x_range=[0, 12],
            y_range=[0, 6],
            x_length=12,
            y_length=6,
            axis_config={"include_tip": True, "include_ticks": False},
        )

        self.play(Create(axes))

        # Axis labels
        realism_label = Text("realism", font_size=28).next_to(axes.y_axis, LEFT).rotate(PI / 2)
        realism_label.move_to(axes.y_axis.get_center() + LEFT * 0.4)

        cost_label = Text("cost (complexity/memory/time)", font_size=24)
        cost_label.move_to(axes.x_axis.get_center() + DOWN * 0.4)

        self.wait(2)
        self.play(Write(realism_label))

        self.wait(1)
        self.play(Write(cost_label))

        self.wait(0.5)
        # Fade out the "Techniques" header and underline
        self.play(FadeOut(header), FadeOut(underline))
        self.wait(0.5)

        # Move each technique label to its graph position with a dot
        for text_obj, (_, coords) in zip(technique_texts, techniques):
            pos = axes.c2p(*coords)
            dot = Dot(pos, color=YELLOW)
            self.play(
                AnimationGroup(
                    FadeIn(dot),
                    text_obj.animate.next_to(pos + 0.125 * RIGHT + 0.125 * DOWN, buff=0.1),
                    lag_ratio=0.1
                )
            )
            self.wait(0.5)

        # Highlight directional lights
        directional_coords = [1.75, 4]
        directional_dot_pos = axes.c2p(*directional_coords)
        highlight = Circle(radius=0.2, color=GREEN).move_to(directional_dot_pos)
        self.play(Create(highlight))
        self.wait(2)