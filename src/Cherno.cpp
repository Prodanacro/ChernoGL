#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <signal.h>

#ifdef debug
    #define glcall(x) glclearerror(); x; assert(gllogcall(const char* function,
                const char* file,
                int line))
#else
    #define GLCall(x) x
#endif

#define ASSERT(x) if(!(x)) raise(SIGTRAP);
/*    
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
*/

static void GLClearError() {
    while(glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* function, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << 
           " " << file << ":" << line <<  std::endl;
        return false;
    }
    return true;
}


struct shaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static shaderProgramSource parseShader(const std::string& filepath){
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 
    };
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    
    while(getline(stream, line)) {
        if(line.find("#shader") != std::string::npos) {
            if(line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;        
            }
             else if(line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;        
            }
        }  
         else {
            ss[(int)type] << line << '\n';
        }
        
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, 
                                  const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling.
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) { // Shader did not compile properly.
        int length;           // Error message length.
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
                  << " shader!" << std::endl << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/** 
 * Creates a program from two shaders.
 * */
static int createProgram(const std::string& vertexShader,
                         const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}


int main(void) {
    GLFWwindow* window;

    // Initialize the GLWF library.
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current.
    glfwMakeContextCurrent(window);
    
    // Set swap interval for the current OpenGL context.
    glfwSwapInterval(1);

    // Initialize the GLEW library.
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    float positions[] = {
        -0.5f, -0.5f,	// 0
         0.5f, - 0.5f,	// 1
         0.5f, 0.5f,	// 2
	-0.5f, 0.5f	// 3
    };

    unsigned int indices[] = {
    	0, 1, 2,        // First triangle. 
	2, 3, 0         // Second triangle.
    };

    // Vertex array.
    unsigned int vao;   // Vertex array object ID.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex buffer.
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,               // Name of the buffer.
                 4 * 2 * sizeof(float),         // Buffer size.
                 positions,                     // Data.
                 GL_STATIC_DRAW);               // Usage.

    glEnableVertexAttribArray(0);
    // Layout. Links buffer with vao.
    glVertexAttribPointer(0,                    // Index (first attribute). 
                          2,                    // Attribute size.
                          GL_FLOAT,             // Attribute type.
                          GL_FALSE,             // Not normalized.
                          2 * sizeof(float),    // Offset between vertecies. 
                          0);                   // Offset between attributes.

    // Index buffer.
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,        // Name of the buffer.
                 6 * sizeof(unsigned int),       // Buffer size.
                 indices,                        // Data.
                 GL_STATIC_DRAW);                // Usage.


    shaderProgramSource source = parseShader("res/shaders/Basic.shader");
    unsigned int program = createProgram(source.vertexSource, 
                                         source.fragmentSource);
    glUseProgram(program);

    int location = glGetUniformLocation(program, "u_Color");
    ASSERT(location != -1);
    glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

    float r = 0.0f; 
    float increment = 0.05f;

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Render here.
        glClear(GL_COLOR_BUFFER_BIT);
         
        glUniform4f(location, r, 0.3f, 0.8f, 1.0f);

	// Note that 6 is number of indices, not vertices!
        // Note that type should always be unsigned!
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if(r > 1.0f)
            increment = -0.05f;
        else if(r < 0.0f) 
            increment = 0.05f;

        r += increment;

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
