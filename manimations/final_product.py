from manim import *
import config

class FinalProductTitle(Scene):
    def construct(self):
        self.wait(1)
        final_product_section = Text("III. Final Product", font_size=32, color=WHITE)
        self.play(Write(final_product_section))
        self.wait(1)
        self.play(FadeOut(final_product_section))

class FinalProductSection(Scene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/final-product-0.m4a")

        strength = "S = \\frac{1}{2} \left( 1 + \cos \\theta \\right)"
        cosine = "\\text{ where } \cos \\theta = 1 - \\frac{1}{2} * | {l} - {n} | ^2"
        color_map = {
            "{l}": YELLOW,
            "{n}": BLUE,
        }
        formula = MathTex(strength, cosine, tex_to_color_map=color_map, font_size=40)
        formula.move_to([0, 3, 0])
        self.wait(4)
        self.play(Write(formula[0]))

        self.wait(2)
        self.play(Write(formula[1:]))

        shift = 3.25
        gnp = ImageMobject("assets/gnp.png")
        gnp.scale_to_fit_height(5).shift(0.5 * DOWN + shift * LEFT)

        san_juans = ImageMobject("assets/san-juans.png")
        san_juans.scale_to_fit_height(5).shift(0.5 * DOWN + shift * RIGHT)

        self.wait(5)
        self.play(FadeIn(gnp), FadeIn(san_juans), run_time=2)
        self.wait(1)

class Endnotes(ThreeDScene):
    def construct(self):
        if config.INCLUDE_AUDIO:
            self.add_sound(f"{config.AUDIO_ASSETS}/endnotes-0.m4a")
            self.add_sound(f"{config.AUDIO_ASSETS}/modifications-0.m4a", time_offset=4)
            self.add_sound(f"{config.AUDIO_ASSETS}/pseudoscopic-illusion-0.m4a", time_offset=4 + 17)
            self.add_sound(f"{config.AUDIO_ASSETS}/simplicity-0.m4a", time_offset=4 + 17 + 27)

        # Create Text objects for the list
        numberings = [ "#1:", "#2:", "#3:" ]
        endnotes = [
            "Modifications",
            "Pseudoscopic illusion",
            "Simplicity",
        ]

        numberings_texts = [Text(numbering, font_size=30) for numbering in numberings]
        endnotes_text = [Text(note, font_size=30) for note in endnotes]

        # Align labels and explanations horizontally
        rows = []
        for numbering, endnote in zip(numberings_texts, endnotes_text):
            row = VGroup(numbering, endnote).arrange(RIGHT, buff=0.4)
            rows.append(row)

        # Left-align as a VGroup
        endnotes_list = VGroup(*rows).arrange(DOWN, aligned_edge=LEFT, buff=0.4)

        # Header
        header = Text("Endnotes", font_size=30)
        header.next_to(endnotes_list, UP, buff=0.5)
        
        # Underline (line spans width of the assumption list)
        underline = Line(
            start=endnotes_list.get_left() + 0.25 * LEFT,
            end=endnotes_list.get_right() + 0.25 * RIGHT,
            stroke_width=2
        )
        underline.next_to(header, DOWN, buff=0.2)

        endnotes_group = VGroup(header, underline, endnotes_list)
        endnotes_group.move_to(ORIGIN + 3.5 * LEFT)
        endnotes_box = SurroundingRectangle(endnotes_group, color=WHITE, buff=0.4)

        # Show header, underline, and list one by one
        self.play(Write(header), Create(underline), Create(endnotes_box))

        for numbering in numberings_texts:
            self.play(Write(numbering), run_time=0.5)

        self.wait(1)

        shift = 3.5 * RIGHT
        bench_lakes = ImageMobject("assets/bench-lakes.png")
        bench_lakes.scale_to_fit_height(5).shift(shift)
        self.play(Write(endnotes_text[0]), FadeIn(bench_lakes))

        pseudoscopic_bridgers = ImageMobject("assets/pseudoscopic-bridgers.png")
        pseudoscopic_bridgers.scale_to_fit_height(5).shift(shift)
        self.play(
            Write(endnotes_text[1]),
            FadeOut(bench_lakes),
            FadeIn(pseudoscopic_bridgers),
        )

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

            # self.play(Write(label))
            self.play(FadeIn(ellipse), FadeIn(label), FadeIn(arrow))
            self.wait(1)
            self.play(FadeOut(ellipse), FadeOut(label), FadeOut(arrow))

        src = np.array([7, -3.5, 0])
        arrow = Arrow(src, src + np.array([-1, 1, 0]), color=YELLOW)
        light_label = Text("(Lit from bottom left)", font_size=24)
        light_label.next_to(arrow, 0.01 * LEFT + 0.125 * DOWN)
        self.play(GrowArrow(arrow), FadeIn(light_label))
        add_ellipse_label([3.25, 0, 0], [1, 4], -PI / 18, "This is a ridgeline", [0, 3, 0], -PI / 8)
        self.play(FadeOut(arrow), FadeOut(light_label))
        self.wait(0.5)

        gnp = ImageMobject("assets/gnp.png")
        gnp.scale_to_fit_height(5).shift(shift)
        self.play(
            Write(endnotes_text[2]),
            FadeOut(pseudoscopic_bridgers),
            FadeIn(gnp),
        )

        self.play(FadeOut(endnotes_group), FadeOut(endnotes_box))

class ThanksForWatching(ThreeDScene):
    def construct(self):
        gnp = ImageMobject("assets/gnp.png")
        gnp.shift(3.5 * RIGHT)

        self.add(gnp)

        self.play(gnp.animate.move_to(ORIGIN))

        square = Square(fill_color=GREEN, fill_opacity=1)
        self.play(Create(square))

        self.move_camera(phi=65*DEGREES)
        self.begin_ambient_camera_rotation(rate=0.1)
        self.wait(15)

        thanks = Text("Thanks for watching!", font_size=36).to_edge(UP)
        self.add_fixed_in_frame_mobjects(thanks)
        self.play(Write(thanks))