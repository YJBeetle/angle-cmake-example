#include <iostream>
#include <SDL.h>
#include <angle_gl.h>

namespace {
void printProgramLog(GLuint f_programId) {
  if (glIsProgram(f_programId)) {
    int logLen = 0;
    glGetProgramiv(f_programId, GL_INFO_LOG_LENGTH, &logLen);

    char* infoLog_a = new char[logLen];
    int infoLogLen = 0;
    glGetProgramInfoLog(f_programId, logLen, &infoLogLen, infoLog_a);

    std::cout << infoLog_a << std::endl;
    delete[] infoLog_a;
  }
}

void printShaderLog(GLuint f_shaderId) {
  if (glIsShader(f_shaderId)) {
    int logLen = 0;
    glGetShaderiv(f_shaderId, GL_INFO_LOG_LENGTH, &logLen);

    char* infoLog_a = new char[logLen];
    int infoLogLen = 0;
    glGetShaderInfoLog(f_shaderId, logLen, &infoLogLen, infoLog_a);

    std::cout << infoLog_a << std::endl;
    delete[] infoLog_a;
  }
}

GLuint loadShader(const GLchar* f_source_p, GLenum f_type) {
  GLuint shaderId = glCreateShader(f_type);
  glShaderSource(shaderId, 1, &f_source_p, nullptr);
  glCompileShader(shaderId);

  GLint compileStatus = GL_FALSE;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

  if (!compileStatus) {
    printShaderLog(shaderId);
    glDeleteShader(shaderId);
    shaderId = 0;
  }

  return shaderId;
}

GLuint loadProgram(const GLchar* f_vertSource_p, const GLchar* f_fragSource_p) {
  GLuint vertShader = loadShader(f_vertSource_p, GL_VERTEX_SHADER);
  GLuint fragShader = loadShader(f_fragSource_p, GL_FRAGMENT_SHADER);

  if (!glIsShader(vertShader) || !glIsShader(fragShader)) {
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return 0;
  }

  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertShader);
  glAttachShader(programId, fragShader);

  glLinkProgram(programId);
  GLint linkStatus = GL_FALSE;
  glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

  if (!linkStatus) {
    printProgramLog(programId);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    glDeleteProgram(programId);
    return 0;
  }

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  return programId;
}
}  // namespace

int main(int, char**) {
  // Init SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }

  SDL_version version;
  SDL_GetVersion(&version);
  std::cout << "SDL version: " << static_cast<int>(version.major) << "."
            << static_cast<int>(version.minor) << "."
            << static_cast<int>(version.patch) << std::endl;

  // Create window
  SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

  // Explicitly set channel depths, otherwise we might get some < 8
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  auto windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
  auto window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 512, 512, windowFlags);

  // Init GL
  auto glContext = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, glContext);
  std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;

  // Load shader program
  constexpr char kVS[] = R"(attribute vec4 vPosition;
void main()
{
    gl_Position = vPosition;
})";

  constexpr char kFS[] = R"(precision mediump float;
void main()
{
    gl_FragColor = vec4(gl_FragCoord.x / 512.0, gl_FragCoord.y / 512.0, 0.0, 0.1);
})";

  auto program = loadProgram(kVS, kFS);

  // Main loop
  bool isRunning = true;
  while (isRunning) {
    SDL_Event event;
    while (0 != SDL_PollEvent(&event)) {
      if (SDL_QUIT == event.type) {
        isRunning = false;
      } else if (SDL_KEYDOWN == event.type) {
        const auto keyStates_p = SDL_GetKeyboardState(nullptr);
        if (keyStates_p[SDL_SCANCODE_ESCAPE]) {
          isRunning = false;
        }
      }
    }

    // Clear
    glClearColor(0.2F, 0.2F, 0.2F, 1.F);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 512, 512);

    // Render scene
    GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
    };
    glUseProgram(program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Update window
    SDL_GL_SwapWindow(window);
  }

  // Clean up
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}