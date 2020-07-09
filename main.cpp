#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    // SDL2 init
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Mandelbrot Set",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            200, 200,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GLContext cont = SDL_GL_CreateContext(win);

    // Load shader code from file
    FILE *fragment_shader_fp = fopen("shader.frag", "r");
    fseek(fragment_shader_fp, 0L, SEEK_END);
    long int fragment_shader_fs = ftell(fragment_shader_fp);
    rewind(fragment_shader_fp);

    char *fragment_shader = (char*) malloc(fragment_shader_fs * sizeof(char));
    for (int i = 0; i < fragment_shader_fs; ++i) {
        fragment_shader[i] = fgetc(fragment_shader_fp);
        cout << (int)fragment_shader[i] << '\n';
    }
    fragment_shader[fragment_shader_fs - 1] = '\0';

    fclose(fragment_shader_fp);

    // Shader
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, (const GLchar**)&fragment_shader, NULL);
    glCompileShader(shader);

    // Check shader for errors
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    GLint len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char *errlog = (char*) malloc(len * sizeof(char));
    glGetShaderInfoLog(shader, len, &len, errlog);
    cerr << errlog << '\n';

    if (success == GL_FALSE) {
        glDeleteShader(shader);
        SDL_GL_DeleteContext(cont);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return -1;
    }

    // Shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glUseProgram(program);

    // Get uniform locaions
    int uTime             = glGetUniformLocation(program, "time");
    int uResolution       = glGetUniformLocation(program, "resolution");
    int uMandelbrotCenter = glGetUniformLocation(program, "mandelbrot_center");
    int uMandelbrotHeight = glGetUniformLocation(program, "mandelbrot_height");

    glDisable(GL_DEPTH_TEST);
    glClearColor(0., 1., 0., 1.);

    // Mainloop
    double mandelbrotHeight  = 3.5f;
    double mandelbrotCenterX = 0.f;
    double mandelbrotCenterY = 0.f;

    float last_time = SDL_GetTicks() / 1000.;
    SDL_Event e;
    for(;;) {
        // time count
        float time = SDL_GetTicks() / 1000.;
        float dtime = last_time - time;    // delta time
        last_time = time;

        // Event loop
        bool quit = 0;
        while (SDL_PollEvent(&e) != 0)
            if (e.type == SDL_QUIT)
                quit = 1;
        if (quit) break;

        // Keyboard events
        const Uint8 *key = SDL_GetKeyboardState(NULL);
        float speedFactor = 1.;

        if (key[SDL_SCANCODE_ESCAPE]) break;
        if (key[SDL_SCANCODE_LSHIFT] || key[SDL_SCANCODE_RSHIFT]) speedFactor = 2.;
   else if (key[SDL_SCANCODE_LCTRL] || key[SDL_SCANCODE_RCTRL])   speedFactor = 0.5;
   else                                                           speedFactor = 1.;
        if (key[SDL_SCANCODE_E] || key[SDL_SCANCODE_EQUALS]) mandelbrotHeight  *= pow(5, dtime * speedFactor);
        if (key[SDL_SCANCODE_Q] || key[SDL_SCANCODE_MINUS])  mandelbrotHeight  /= pow(5, dtime * speedFactor);
        if (key[SDL_SCANCODE_W] || key[SDL_SCANCODE_UP])     mandelbrotCenterY += mandelbrotHeight * 0.7 * dtime * speedFactor;
        if (key[SDL_SCANCODE_S] || key[SDL_SCANCODE_DOWN])   mandelbrotCenterY -= mandelbrotHeight * 0.7 * dtime * speedFactor;
        if (key[SDL_SCANCODE_D] || key[SDL_SCANCODE_RIGHT])  mandelbrotCenterX += mandelbrotHeight * 0.7 * dtime * speedFactor;
        if (key[SDL_SCANCODE_A] || key[SDL_SCANCODE_LEFT])   mandelbrotCenterX -= mandelbrotHeight * 0.7 * dtime * speedFactor;

        // Set viewport
        int width, height;
        SDL_GetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);

        // Setting the uniform values
        glUniform1f(uTime, time);
        glUniform2i(uResolution, width, height);

        // Set mandelbrot set setting uniform values
        glUniform2d(uMandelbrotCenter, mandelbrotCenterX, mandelbrotCenterY);
        glUniform1d(uMandelbrotHeight, mandelbrotHeight);

        // Draw
        glClear(GL_COLOR_BUFFER_BIT);
        glRecti(-1, -1, 1, 1);

        SDL_GL_SwapWindow(win);
    }

    // Quit
    SDL_GL_DeleteContext(cont);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
