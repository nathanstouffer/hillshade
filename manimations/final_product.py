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

        formulas = [
            "S = \\frac{1}{2} \left( 1 + \cos \\theta \\right)",
            "\cos \\theta = 1 - \\frac{1}{2} * | {l} - {n} | ^2",
        ]
        color_map = {
            "{l}": YELLOW,
            "{n}": BLUE,
        }
        formulas_text = [
            MathTex(formula, tex_to_color_map=color_map, font_size=40)
            for formula in formulas
        ]

        shift = 3.5

        formulas_text[0].move_to([-shift, 3, 0])
        formulas_text[1].move_to([shift, 3, 0])

        self.play(Write(formulas_text[0]))
        self.play(Write(formulas_text[1]))

        gnp = ImageMobject("assets/gnp.png")
        gnp.scale_to_fit_height(5).shift(0.5 * DOWN + shift * LEFT)

        san_juans = ImageMobject("assets/san-juans.png")
        san_juans.scale_to_fit_height(5).shift(0.5 * DOWN + shift * RIGHT)

        self.play(FadeIn(gnp), FadeIn(san_juans))

class Endnotes(Scene):
    def construct(self):
        self.wait(1)
        directional_lighting_section = Text("III. Final Product", font_size=32, color=WHITE)
        self.play(Write(directional_lighting_section))
        self.wait(1)