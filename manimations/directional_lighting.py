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

class Assumptions(Scene):

    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/directional-lighting-1.m4a")

        directional_lighting_section = Text("I. Directional Lighting", font_size=32).move_to([0, 3, 0])
        self.add(directional_lighting_section)

        question = Text("What is a directional light?", font_size=30)
        self.play(Write(question))
        self.wait(1)
        self.play(FadeOut(directional_lighting_section), FadeOut(question))

        goal = Text("Goal: Pseudo-realistic lighting", font_size=32).move_to([0, 3, 0])
        self.wait(1)
        self.play(Write(goal))

        ray_color = YELLOW
        object_color = WHITE

        # Positions
        wall_x = 5.0
        wall_min_y = -3.5
        wall_length = 3
        obstruction_length = 0.7
        mid_y = wall_min_y + 0.5 * wall_length

        wall = Line(ORIGIN, wall_length * UP, color=object_color).shift(wall_x * RIGHT + wall_min_y * UP)
        obstruction = Line(ORIGIN, obstruction_length * UP).shift(UP * (mid_y - 0.5 * obstruction_length))

        start_light_pos = 4.5 * LEFT + mid_y * UP
        far_light_pos = 150 * LEFT + mid_y * UP

        point_light = Dot(start_light_pos, color=ray_color)

        point_light_label = Text("Light source", font_size=28).next_to(point_light, UP, buff=0.2)
        wall_label = Text("Object", font_size=28).next_to(wall, UP, buff=0.2)
        obstruction_label = Text("Obstruction", font_size=28).next_to(obstruction, RIGHT, buff=0.2)
        self.play(FadeIn(point_light), FadeIn(obstruction), FadeIn(wall))

        self.play(Write(point_light_label))
        self.play(Write(wall_label))
        self.play(Write(obstruction_label))

        # === STATE 1: Rays stop at the obstruction ===
        max_angle = np.atan2(0.5 * wall_length, wall_x - start_light_pos[0])
        eps = max_angle * 0.05
        angles = np.linspace(max_angle - eps, -max_angle + eps, 11)
        rays1 = VGroup()

        padding = 0.075
        for angle in angles:
            ray_origin = start_light_pos
            ray_direction = rotate_vector(RIGHT, angle)

            t = (obstruction.get_center()[0] - ray_origin[0]) / ray_direction[0]
            y = ray_direction[1] * t
            threshold = 0.5 * obstruction_length
            if not (-threshold <= y and y <= threshold):
                t = (wall.get_center()[0] - ray_origin[0]) / ray_direction[0]

            hit_point = ray_origin + t * ray_direction

            # Intersect approx with front edge of circle
            src = ray_origin + padding * ray_direction
            dst = hit_point - padding * ray_direction
            ray = Arrow(start=src, end=dst, color=ray_color, buff=0.05, stroke_width=2, tip_length=0.1)
            rays1.add(ray)

        self.play(LaggedStart(*[GrowArrow(ray) for ray in rays1], lag_ratio=0.075))
        self.wait(0.5)

        # Create Text objects for the list
        assumption_numberings = [ "#1:", "#2:" ]
        assumption_explanations = [
            "Ignore obstructions",
            "Infinite distance"
        ]

        numberings_texts = [Text(numbering, font_size=30) for numbering in assumption_numberings]
        assumptions_text = [Text(expl, font_size=30) for expl in assumption_explanations]

        # Align labels and explanations horizontally
        assumptions_rows = []
        for numbering, assumption in zip(numberings_texts, assumptions_text):
            row = VGroup(numbering, assumption).arrange(RIGHT, buff=0.4)
            assumptions_rows.append(row)

        # Left-align as a VGroup
        assumption_list = VGroup(*assumptions_rows).arrange(DOWN, aligned_edge=LEFT, buff=0.2)
        assumption_list.move_to(ORIGIN + 4 * LEFT + 0.75 * UP)

        # Header
        header = Text("Assumptions", font_size=30)
        header.next_to(assumption_list, UP, buff=0.5)
        
        # Underline (line spans width of the assumption list)
        underline = Line(
            start=assumption_list.get_left() + 0.25 * LEFT,
            end=assumption_list.get_right() + 0.25 * RIGHT,
            stroke_width=2
        )
        underline.next_to(header, DOWN, buff=0.2)

        # Bounding rectangle
        assumptions_group = VGroup(header, underline, assumption_list)
        assumptions_box = SurroundingRectangle(assumptions_group, color=WHITE, buff=0.4)

        # Show header, underline, and list one by one
        self.play(Write(header))
        self.play(Create(underline)) #, Create(assumptions_box))

        for numbering in numberings_texts:
            self.play(Write(numbering), run_time=0.5)

        self.wait(1)

        # === STATE 2: Rays pass through the obstruction and reach the wall ===
        rays2 = VGroup()
        for angle in angles:
            ray_origin = start_light_pos
            ray_direction = rotate_vector(RIGHT, angle)

            t = (wall.get_center()[0] - ray_origin[0]) / ray_direction[0]
            hit_point = ray_origin + t * ray_direction

            # Intersect approx with front edge of circle
            src = ray_origin + padding * ray_direction
            dst = hit_point - padding * ray_direction
            ray = Arrow(start=src, end=dst, color=ray_color, buff=0.05, stroke_width=2, tip_length=0.1)
            rays2.add(ray)

        self.play(
            ReplacementTransform(rays1, rays2),
            Write(assumptions_text[0]),
            FadeOut(obstruction_label),
            run_time=1.0
        )
        self.wait(0.5)

        # === STATE 3: Light moves far left → nearly parallel rays ===
        # recompute angles for new position
        max_angle = np.atan2(0.5 * wall_length, wall_x - far_light_pos[0])
        eps = max_angle * 0.05
        angles = np.linspace(max_angle - eps, -max_angle + eps, 11)

        rays3 = VGroup()
        for angle in angles:
            ray_origin = far_light_pos
            ray_direction = rotate_vector(RIGHT, angle)

            t = (wall.get_center()[0] - ray_origin[0]) / ray_direction[0]
            hit_point = ray_origin + t * ray_direction

            # Intersect approx with front edge of circle
            src = ray_origin + padding * ray_direction
            dst = hit_point - padding * ray_direction
            ray = Arrow(start=src, end=dst, color=ray_color, buff=0.05, stroke_width=2, tip_length=0.1)
            rays3.add(ray)

        self.play(
            point_light.animate.move_to(far_light_pos),
            point_light_label.animate.move_to(far_light_pos),
            ReplacementTransform(rays2, rays3),
            Write(assumptions_text[1]),
            run_time=1.5
        )
        self.wait(1)

        light_direction_arrow = Arrow(start=[1, 1, 0], end=[4, 1, 0], color=YELLOW)
        light_direction_label = MathTex("l", color=YELLOW).scale(1).next_to(light_direction_arrow, UP + RIGHT, buff=0.1)

        self.play(GrowArrow(light_direction_arrow))
        self.play(Write(light_direction_label))
        self.wait(1)

        reversed_light_direction_arrow = Arrow(start=[4, 1, 0], end=[1, 1, 0], color=YELLOW)
        self.play(
            ReplacementTransform(light_direction_arrow, reversed_light_direction_arrow),
            light_direction_label.animate.next_to(reversed_light_direction_arrow, direction=UP + LEFT, buff=0.1)
        )