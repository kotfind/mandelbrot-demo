#version 400

#define M_PI 3.1415926535897932384626433832795

#define MAX_ITER 150.

uniform float time;
uniform vec2 resolution;

uniform vec2 mandelbrot_center;
uniform float mandelbrot_height;

struct complex {
    float r;
    float i;
};

complex add(in complex a, in complex b) {
    complex c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
}

complex sq(in complex a) {
    complex c;
    c.r = a.r * a.r - a.i * a.i;
    c.i = 2. * a.r * a.i;
    return c;
}

float len(in complex a) {
    return sqrt(a.r * a.r +  a.i * a.i);
}

// Returns iterations quantity divided by MAX_ITER
float mandelbrot(vec2 uv) {
    complex c = complex(uv.x, uv.y);
    complex z = complex(0., 0.);

    float i = 0.;
    for (;i < MAX_ITER; ++i) {
        if (len(z) > 2.) break;

        z = add(sq(z), c);
    }

    return i / MAX_ITER;
}

vec3 getColor(float x) {
    return vec3(x*x*(3. - 2.*x));
}

out vec4 fragColor;

void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * resolution) / min(resolution.x, resolution.y);
    uv = (uv * mandelbrot_height) - mandelbrot_center;

    vec3 c = vec3(getColor(mandelbrot(uv)));

    fragColor = vec4(c, 1.);
}

