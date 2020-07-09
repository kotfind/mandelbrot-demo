#version 400

#define M_PI 3.1415926535897932384626433832795

#define MAX_ITER 500.

uniform ivec2 resolution;

uniform dvec2 mandelbrot_center;
uniform double mandelbrot_height;

struct complex {
    double r;
    double i;
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

double len(in complex a) {
    return sqrt(a.r * a.r +  a.i * a.i);
}

// Returns iterations quantity
double mandelbrot(dvec2 uv) {
    complex c = complex(uv.x, uv.y);
    complex z = complex(0., 0.);

    double i = 0.;
    for (;i < MAX_ITER; ++i) {
        if (len(z) > 2.) break;

        z = add(sq(z), c);
    }

    return i;
}

vec3 getColor(double x) {
    x = clamp(x / 100. * pow(float(mandelbrot_height), 0.05), 0., 1.);
    x = x*x*(3. - 2.*x);

    const vec3 c1 = vec3(0., 0.027451, 0.392157);
    const vec3 c2 = vec3(0.12549, 0.419608, 0.796078);
    const vec3 c3 = vec3(0.929412, 1., 1.);
    const vec3 c4 = vec3(1., 0.666667, 0.);
    const vec3 c5 = vec3(0., 0.007843, 0.);

    if (x < 0.16)
        return mix(c1, c2, (float(x) - 0.) / 0.16);
    else if (x < 0.42)
        return mix(c2, c3, (float(x) - 0.16) / (0.42 - 0.16));
    else if (x < 0.6425)
        return mix(c3, c4, (float(x) - 0.42) / (0.6425 - 0.42));
    else if (x < 0.8575)
        return mix(c4, c5, (float(x) - 0.6425) / (0.8575 - 0.6425));
    else
        return mix(c5, vec3(0.), (float(x) - 0.8575) / (1. - 0.8575));
    return vec3(x);
}

out vec4 fragColor;

void main() {
    dvec2 uv = (gl_FragCoord.xy - 0.5 * dvec2(resolution)) / double(min(resolution.x, resolution.y));
    uv = (uv * mandelbrot_height) - mandelbrot_center;

    vec3 c = vec3(getColor(mandelbrot(uv)));

    fragColor = vec4(c, 1.);
}

