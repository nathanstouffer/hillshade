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
        self.play(FadeIn(gnp), FadeIn(san_juans))
        self.wait(1)

class Endnotes(Scene):
    def construct(self):
        self.wait(1)
        directional_lighting_section = Text("III. Final Product", font_size=32, color=WHITE)
        self.play(Write(directional_lighting_section))
        self.wait(1)