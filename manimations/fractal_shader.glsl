// // fractal_shader.glsl
// #version 330 core

// uniform float u_time;
// uniform vec2 u_resolution;
// uniform vec2 u_center;
// uniform float u_zoom;

// out vec4 fragColor;

// int mandelbrot(vec2 c)
// {
//     vec2 z = vec2(0.0);
//     int i;
//     for(i = 0; i < 100; i++)
//     {
//         if (dot(z, z) > 4.0) break;
//         z = vec2(
//             z.x * z.x - z.y * z.y + c.x,
//             2.0 * z.x * z.y + c.y
//         );
//     }
//     return i;
// }

// void main()
// {
//     vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution) / u_resolution.y;
//     vec2 c = u_center + uv * u_zoom;
//     int i = mandelbrot(c);
//     float norm = float(i) / 100.0;
//     fragColor = vec4(vec3(norm), 1.0);
// }

// shader.fs
// Simple fragment shader that colors the object red

#version 330 core
in vec4 fragColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red color
}