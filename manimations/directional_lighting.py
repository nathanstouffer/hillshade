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

        self.wait(2)

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

        self.wait(6)

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

        self.wait(1.5)
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

class Shortcuts(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/directional-lighting-1.m4a")

        directional_lighting_section = Text("I. Directional Lighting", font_size=32, color=WHITE).move_to([0, 3, 0])
        self.add(directional_lighting_section)

        question = Text("What is a directional light?", font_size=30)
        self.play(Write(question))
        self.wait(1)
        self.play(FadeOut(question))

        # Create Text objects for the list
        shortcuts = [
            "#1: Ignore obstructions",
            "#2: Infinite distance"
        ]
        shortcut_texts = [Text(shortcut, font_size=30) for shortcut in shortcuts]

        # Left-align as a VGroup
        shortcut_list = VGroup(*shortcut_texts).arrange(DOWN, aligned_edge=LEFT, buff=0.4)
        shortcut_list.move_to(ORIGIN + 4 * LEFT + 0.5 * UP)

        # Header
        header = Text("Shortcuts", font_size=30)
        header.next_to(shortcut_list, UP, buff=0.5)
        
        # Underline (line spans width of the shortcut list)
        underline = Line(
            start=shortcut_list.get_left() + 0.25 * LEFT,
            end=shortcut_list.get_right() + 0.25 * RIGHT,
            stroke_width=2
        )
        underline.next_to(header, DOWN, buff=0.2)

        # Show header, underline, and list one by one
        self.play(Write(header))
        self.play(Create(underline))
        self.wait(0.3)

        for text in shortcut_texts:
            self.play(Write(text))
            self.wait(0.3)

        # TODO draw a bounding rectangle around the shortcuts list

        # Colors
        ray_color = YELLOW
        object_color = WHITE

        # Common elements
        circle = Circle(radius=0.5, color=object_color).shift(RIGHT * 2)
        wall = Line(UP, DOWN, color=object_color).next_to(circle, RIGHT, buff=1)

        # State 1: Diverging rays from point source
        point1 = Dot(LEFT * 4, color=ray_color)
        rays1 = VGroup(*[
            Line(point1.get_center(), circle.point_from_proportion(p), color=ray_color)
            for p in [0.2, 0.4, 0.5, 0.6, 0.8]
        ])
        rays1.add(Line(point1.get_center(), wall.get_top(), color=ray_color))
        rays1.add(Line(point1.get_center(), wall.get_bottom(), color=ray_color))

        state1 = VGroup(point1, circle.copy(), wall.copy(), rays1)

        # State 2: Rays partially blocked
        rays2 = VGroup()
        for p in [0.2, 0.4, 0.5, 0.6, 0.8]:
            target = circle.point_from_proportion(p)
            mid = interpolate(point1.get_center(), target, 0.5)
            rays2.add(Line(point1.get_center(), target, color=ray_color))

        rays2.add(Line(point1.get_center(), wall.get_top(), color=ray_color))
        rays2.add(Line(point1.get_center(), wall.get_bottom(), color=ray_color))
        state2 = VGroup(point1.copy(), circle.copy(), wall.copy(), rays2)

        # State 3: Parallel rays
        rays3 = VGroup()
        ray_y = [-0.6, -0.3, 0, 0.3, 0.6]
        for y in ray_y:
            rays3.add(Line(LEFT * 4 + UP * y, RIGHT * 4 + UP * y, color=ray_color))

        state3 = VGroup(circle.copy(), wall.copy(), rays3)

        # Animations
        self.play(FadeIn(point1), FadeIn(circle), FadeIn(wall), *[Create(ray) for ray in rays1])
        self.wait(1)
        self.play(Transform(rays1, rays2))
        self.wait(1)
        self.play(
            FadeOut(point1),
            FadeOut(rays1),
            FadeIn(rays3),
        )
        self.wait(1)

        # # Title
        # title = Text("Directional Light", font_size=48).to_edge(UP)
        # self.play(Write(title))
        # self.wait(0.5)

        # # Part 1: Ignoring obstructions (shadows demo)
        # self.play(FadeIn(Text("1. Ignores obstructions").scale(0.7).next_to(title, DOWN)))
        # self.wait(1)

        # # Terrain bump and light source
        # bump = Arc(radius=1, start_angle=PI, angle=PI, color=WHITE)
        # bump.shift(DOWN * 2)
        # light = Dot(UP * 3 + LEFT * 3, color=YELLOW)
        # light_label = Tex("Light source").next_to(light, UP)

        # ray1 = Arrow(light.get_center(), bump.point_from_proportion(0.2), buff=0.1, color=YELLOW)
        # ray2 = Arrow(light.get_center(), bump.point_from_proportion(0.5), buff=0.1, color=YELLOW)
        # ray3 = Arrow(light.get_center(), bump.point_from_proportion(0.8), buff=0.1, color=YELLOW)

        # self.play(Create(bump), FadeIn(light), Write(light_label))
        # self.play(GrowArrow(ray1), GrowArrow(ray2), GrowArrow(ray3))
        # self.wait(1)

        # # Shadow region (fake – no occlusion)
        # shadow = Rectangle(width=2.5, height=0.4, fill_opacity=0.5, fill_color=BLACK)
        # shadow.next_to(bump, DOWN, buff=0.05)

        # self.play(FadeIn(shadow))
        # self.wait(1.5)

        # # Emphasize realism perception
        # realism_note = Tex(
        #     "Even without true shadows,\\ directional light suggests 3D shape",
        #     font_size=36
        # ).next_to(shadow, DOWN)
        # self.play(Write(realism_note))
        # self.wait(2)

        # self.play(*[FadeOut(mob) for mob in [bump, light, light_label, ray1, ray2, ray3, shadow, realism_note]])

        # # Part 2: Light from the same direction
        # label2 = Text("2. Light from same direction $l$", font_size=30).next_to(title, DOWN)
        # self.play(TransformFromCopy(title, label2))
        # self.wait(1)

        # # Draw parallel rays
        # terrain = Rectangle(width=5, height=0.5, fill_opacity=1, fill_color=GREY)
        # terrain.shift(DOWN * 2)
        # rays = VGroup(*[
        #     Arrow(start=UP * 2 + x * RIGHT, end=ORIGIN + x * RIGHT, color=YELLOW, buff=0.1)
        #     for x in [-2, -1, 0, 1, 2]
        # ])

        # self.play(FadeIn(terrain), *[GrowArrow(ray) for ray in rays])
        # self.wait(0.5)

        # # Label l vector (pointing toward the light source)
        # l_arrow = Arrow(start=ORIGIN, end=UP * 1.2, color=BLUE, buff=0.1).shift(RIGHT * 3)
        # l_label = MathTex("l", color=BLUE).next_to(l_arrow, UP)
        # toward_label = Tex("Direction \\textit{toward} the light source").scale(0.6).next_to(l_arrow, RIGHT)

        # self.play(GrowArrow(l_arrow), FadeIn(l_label), FadeIn(toward_label))
        # self.wait(2)

        # self.play(*[FadeOut(mob) for mob in [terrain, rays, l_arrow, l_label, toward_label, label2]])