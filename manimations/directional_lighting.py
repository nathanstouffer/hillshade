from manim import *
import numpy as np


class CosineThetaToDotProduct(Scene):

    def place(self, theta, radius, y_translation):
        x = radius * np.cos(theta)
        y = radius * np.sin(theta) + y_translation
        return np.array([x, y, 0])

    def construct(self):
        # --- Geometry setup ---
        RENDER_RADIUS = 5
        RENDER_SHIFT = -1.5
        THETA_N = np.pi / 6
        THETA_L = 3 * np.pi / 4
        P = np.array([0, RENDER_SHIFT, 0])
        N = self.place(THETA_N, RENDER_RADIUS, RENDER_SHIFT)
        L = self.place(THETA_L, RENDER_RADIUS, RENDER_SHIFT)

        # --- Vector representation ---
        PN_vec = Arrow(P, N, buff=0, color=BLUE)
        PL_vec = Arrow(P, L, buff=0, color=YELLOW)
        LN_vec = Arrow(L, N, buff=0, color=GREEN)

        side_n = Line(P, N, color=BLUE)
        side_w = Line(N, L, color=GREEN)
        side_l = Line(P, L, color=YELLOW)

        self.play(GrowArrow(PL_vec), Create(side_l), GrowArrow(PN_vec), Create(side_n))
        self.wait()

        # --- Side and angle labels ---
        # TODO (stouff) improve label placement
        n_label = MathTex("n", color=BLUE).next_to(side_n.get_center(), 2 * DOWN)
        l_label = MathTex("l", color=YELLOW).next_to(side_l.get_center(), 2 * LEFT)

        angle_theta = Angle(Line(P, N), Line(P, L), radius=0.6, other_angle=False, color=WHITE)
        theta_label = MathTex("\\theta", color=WHITE).next_to(angle_theta, UP)

        self.play(
            Write(n_label),
            Write(l_label),
            Create(angle_theta),
        )
        self.play(Write(theta_label))
        self.wait()

        # --- Algebraic proof steps ---
        # step1 = MathTex("c^{2}", "=", "\\|\\vec{AB}\\|^{2}")
        # step2 = MathTex("=", "\\|\\vec{AC}-\\vec{BC}\\|^{2}")
        # step3 = MathTex("=", "\\|\\vec{AC}\\|^{2}+\\|\\vec{BC}\\|^{2}-2\\,\\vec{AC}\\cdot\\vec{BC}")
        # step4 = MathTex("=", "b^{2} + a^{2} - 2ab\\cos\\gamma")
        # conclusion = MathTex("\\therefore\\; c^{2} = a^{2} + b^{2} - 2ab\\cos\\gamma")

        # eq_group = VGroup(step1, step2, step3, step4, conclusion).arrange(
        #     DOWN, aligned_edge=LEFT
        # ).to_corner(UL)

        # for eq in eq_group:
        #     self.play(Write(eq))
        #     self.wait(0.5)