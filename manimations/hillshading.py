from manim import *
import config

class HillshadingSection(Scene):
    def construct(self):
        hillshading_section = Text("II. Hillshading", font_size=32, color=WHITE)
        self.play(Write(hillshading_section))
        self.wait(1)

class ShadowArea(ThreeDScene):

    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/cosine-0.m4a")

        self.set_camera_orientation(phi=72.5 * DEGREES, theta=60 * DEGREES)

        fixed_arrow = Arrow(ORIGIN, UP, color=YELLOW, buff=0)
        fixed_arrow.to_corner(UL, buff=0.85)
        light_label = MathTex("l", color=YELLOW, font_size=50)
        light_label.next_to(fixed_arrow, RIGHT + 0.1 * UP, buff=0.2)
        self.add_fixed_in_frame_mobjects(fixed_arrow, light_label)

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

        self.wait(2)
        self.play(FadeIn(base_rect), run_time=2)
        self.play(Create(grid), run_time=2)
        self.wait(1)

        def compute_adjustment(old, new):
            theta = angle_between_vectors(old, new)
            rotation_axis = np.cross(old, new)
            return (theta, rotation_axis)

        normal_dir = normalize(np.array([0, 1, 1]))

        # Square
        square = Square(side_length=2, fill_opacity=0.7, stroke_opacity=1)
        angle, axis = compute_adjustment(OUT, normal_dir)
        square.rotate(angle, axis)
        square.shift(2*OUT)
        self.play(FadeIn(square))

        self.wait(2)
        self.begin_ambient_camera_rotation(rate=0.1)
        self.wait(18)

        # Draw polygon of projected points in XY plane
        vertices = square.get_vertices()
        shadow = Polygon(*[np.array([pt[0], pt[1], 0]) for pt in vertices], color=BLACK, fill_opacity=0.75)
        lines = [Line(corner, [corner[0], corner[1], 0], color=YELLOW, stroke_width=1, buff=0) for corner in vertices]
        self.play(FadeIn(shadow), AnimationGroup(*[Create(line) for line in lines]), lag_ratio=0)

        def update_shadow_and_lines(mob):
            # get the square's new vertices after rotation
            vertices = square.get_vertices()
            # update shadow polygon points projected onto XY plane
            new_points = [np.array([pt[0], pt[1], 0]) for pt in vertices]
            mob.set_points_as_corners(new_points + [new_points[0]])
            
            # update each yellow line from the square vertex down to XY plane
            for line, corner in zip(lines, vertices):
                line.put_start_and_end_on(corner, np.array([corner[0], corner[1], 0]))

        new_normal = normalize(np.array([0, 1, 1]))
        angle, axis = compute_adjustment(normal_dir, new_normal)
        self.play(
            square.animate.rotate(angle, axis),
            UpdateFromFunc(shadow, update_shadow_and_lines),
            *[UpdateFromFunc(line, update_shadow_and_lines) for line in lines],
            run_time=3
        )
        normal_dir = new_normal

        new_normal = normalize(np.array([-0.9, -0.5, 1]))
        angle, axis = compute_adjustment(normal_dir, new_normal)
        self.play(
            square.animate.rotate(angle, axis),
            UpdateFromFunc(shadow, update_shadow_and_lines),
            *[UpdateFromFunc(line, update_shadow_and_lines) for line in lines],
            run_time=3
        )
        normal_dir = new_normal

        self.wait(3)

        area_label = MathTex("A_s = A * \cos \\theta", font_size=36).to_corner(UR)
        self.add_fixed_in_frame_mobjects(area_label)
        self.play(Write(area_label))

        self.wait(4)

        new_normal = normalize(np.array([0, -1, 0.25]))
        angle, axis = compute_adjustment(normal_dir, new_normal)
        self.play(
            square.animate.rotate(angle, axis),
            UpdateFromFunc(shadow, update_shadow_and_lines),
            *[UpdateFromFunc(line, update_shadow_and_lines) for line in lines],
            run_time=3
        )
        normal_dir = new_normal

        self.wait(11)

        implies_arrow = MathTex(r"\Rightarrow", font_size=36).rotate(-90 * DEGREES)
        implies_arrow.next_to(area_label, DOWN)
        self.add_fixed_in_frame_mobjects(implies_arrow)
        self.play(Write(implies_arrow))

        strength_label = MathTex("S = \cos \\theta", font_size=36).next_to(implies_arrow, DOWN)
        self.add_fixed_in_frame_mobjects(strength_label)
        self.play(Write(strength_label))

        self.wait(3)

        new_normal = normalize(np.array([0, 0, 1]))
        angle, axis = compute_adjustment(normal_dir, new_normal)
        self.play(
            square.animate.rotate(angle, axis),
            UpdateFromFunc(shadow, update_shadow_and_lines),
            *[UpdateFromFunc(line, update_shadow_and_lines) for line in lines],
            run_time=3
        )
        normal_dir = new_normal

        self.wait(6)

class TransformCosine(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/cosine-1.m4a")

        self.wait(1)

        # Create axes
        axes = Axes(
            x_range=[0, 2 * np.pi, np.pi / 2],  # x from 0 to 2π with ticks every π/2
            y_range=[-1.25, 1.25, 0.5],           # y from -1.5 to 1.5 with ticks every 0.5
            x_length=10,
            y_length=6,
            axis_config={"color": WHITE},
            tips=False
        )

        y_label = Text("light strength", font_size=24)
        y_label.rotate(90 * DEGREES)
        y_label.next_to(axes.y_axis, LEFT)

        cosine_graph = axes.plot(lambda x: np.cos(x), color=YELLOW)
        cosine_label = MathTex("f(x) = \cos \\theta", color=WHITE, font_size=36)
        cosine_label.move_to([0, 3, 0])

        self.play(FadeIn(axes), run_time=1.5)
        self.play(Create(cosine_graph), run_time=1)
        self.play(Write(cosine_label), run_time=1)
        self.play(Write(y_label), run_time=1)

        self.wait(5)

        clamped_graph = axes.plot(lambda x: max(0, np.cos(x)), color=YELLOW, x_range=[0, 2*PI, 0.01])
        self.play(ReplacementTransform(cosine_graph, clamped_graph))

        self.wait(14)

        cosine_graph = axes.plot(lambda x: np.cos(x), color=YELLOW)
        self.play(ReplacementTransform(clamped_graph, cosine_graph))

        self.wait(6)

        shifted_graph = axes.plot(lambda x: 1 + np.cos(x), color=YELLOW)
        shifted_label = MathTex("f(x) = 1 + \cos \\theta", color=WHITE, font_size=36)
        shifted_label.move_to([0, 3, 0])

        self.play(
            ReplacementTransform(cosine_graph, shifted_graph),
            ReplacementTransform(cosine_label, shifted_label)
        )
        self.wait(0.25)

        transformed_graph = axes.plot(lambda x: (1 + np.cos(x)) / 2, color=YELLOW)
        transformed_label = MathTex("f(x) = \dfrac{1 + \cos \\theta}{2}", color=WHITE, font_size=36)
        transformed_label.move_to([0, 3, 0])

        self.play(
            ReplacementTransform(shifted_graph, transformed_graph),
            ReplacementTransform(shifted_label, transformed_label)
        )
        self.wait(1)

class WhatIsLink(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/law-of-cosines-0.m4a")

        def compute(phi, rho):
            origin = np.array([0, -1.75, 0])
            phi_dir = np.array([np.cos(phi), np.sin(phi), 0])
            rho_dir = np.array([np.cos(rho), np.sin(rho), 0])
            arrow_length = 4

            p0 = origin
            p1 = origin + arrow_length * phi_dir
            p2 = origin + arrow_length * rho_dir

            phi_vector = Arrow(start=p0, end=p1, buff=0, color=BLUE)
            rho_vector = Arrow(start=p0, end=p2, buff=0, color=YELLOW)

            theta_arc = Arc(
                radius=0.5,
                start_angle=phi,
                angle=rho - phi,
                color=WHITE,
                arc_center=p0
            )
            theta_label = MathTex("\\theta").next_to(theta_arc, UP, buff=0.25)

            return {
                "v0": p0, "v1": p1, "v2": p2,
                "phi_vector": phi_vector, "rho_vector": rho_vector,
                "theta_arc": theta_arc, "theta_label": theta_label
            }

        self.wait(2)

        state = compute(15 * DEGREES, 137 * DEGREES)
        self.play(
            GrowArrow(state["phi_vector"]),
            GrowArrow(state["rho_vector"]),
        )

        self.play(Create(state["theta_arc"]))
        self.play(Write(state["theta_label"]))

        self.wait(2)
        cosine = MathTex("\cos \\theta = \ ?").to_corner(UR)
        self.play(Write(cosine))

        light_vector_parts = MathTex("l", "=", "[l_x, l_y, l_z]", color=YELLOW).to_corner(UL)
        normal_vector_parts = MathTex("n", "=", "[n_x, n_y, n_z]", color=BLUE)
        normal_vector_parts.next_to(light_vector_parts, DOWN)
        shift = light_vector_parts[1].get_center()[0] - normal_vector_parts[1].get_center()[0]
        normal_vector_parts.shift(np.array([shift, 0, 0]))
        self.wait(2)
        self.play(Write(light_vector_parts), Write(normal_vector_parts), run_time=2)

        new_state = compute(55 * DEGREES, 137 * DEGREES)
        self.wait(6)
        self.play(
            ReplacementTransform(state["phi_vector"], new_state["phi_vector"]),
            ReplacementTransform(state["rho_vector"], new_state["rho_vector"]),
            ReplacementTransform(state["theta_arc"], new_state["theta_arc"]),
            ReplacementTransform(state["theta_label"], new_state["theta_label"]),
        )

        state = new_state
        new_state = compute(55 * DEGREES, 120 * DEGREES)
        self.wait(8)
        self.play(
            ReplacementTransform(state["phi_vector"], new_state["phi_vector"]),
            ReplacementTransform(state["rho_vector"], new_state["rho_vector"]),
            ReplacementTransform(state["theta_arc"], new_state["theta_arc"]),
            ReplacementTransform(state["theta_label"], new_state["theta_label"]),
        )

        self.wait(6)
        state = new_state
        new_state = compute(15 * DEGREES, 137 * DEGREES)
        self.play(
            ReplacementTransform(state["phi_vector"], new_state["phi_vector"]),
            ReplacementTransform(state["rho_vector"], new_state["rho_vector"]),
            ReplacementTransform(state["theta_arc"], new_state["theta_arc"]),
            ReplacementTransform(state["theta_label"], new_state["theta_label"]),
        )

        self.wait(7)
        v0 = new_state["v0"]
        v1 = new_state["v1"]
        v2 = new_state["v2"]
        n_leg = Line(v0, v1, color=BLUE)
        l_leg = Line(v0, v2, color=YELLOW)
        delta_leg = Line(v1, v2, color=GREEN)
        self.play(
            ReplacementTransform(new_state["phi_vector"], n_leg),
            ReplacementTransform(new_state["rho_vector"], l_leg),
        )
        self.play(Create(delta_leg))

class LawOfCosines(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/law-of-cosines-1.m4a")

class DotProduct(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/law-of-cosines-2.m4a")