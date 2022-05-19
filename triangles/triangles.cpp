// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "CG triangles", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(1, 1, 1, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID1 = LoadShaders( "SimpleVertexShader.vertexshader",
                                    "SimpleFragmentShader1.fragmentshader" );

    GLuint programID2 = LoadShaders( "SimpleVertexShader.vertexshader",
                                     "SimpleFragmentShader2.fragmentshader" );

// Матрица модели : единичная матрица (Модель находится в начале координат)
    mat4 Model = glm::mat4(1.0f);  // Индивидуально для каждой модели

    mat4 Projection = perspective(radians(20.0f), 1.0f, 0.2f, 100.0f);

    static const GLfloat g_vertex_buffer_data[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5, 0.0f,
            0.0f,  0.5f, 0.0f,
                -0.5, 0.5, 0,
                0, -0.5, 0,
                0.5, 0.5, 0,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    float angle = 0;
    float step = 0.05;

    do{
        angle += step;
        if (angle > 360) {
            angle -= 360;
        }
        // Или, для ортокамеры
        mat4 View       = lookAt(
                vec3(4,3,3), // Камера находится в мировых координатах (4,3,3)
                vec3(0,0,0), // И направлена в начало координат
                vec3(sin(angle), cos(angle),0)  // "Голова" находится сверху
        );
        // Итоговая матрица ModelViewProjection, которая является результатом перемножения наших трех матриц
        mat4 MVP = Projection * View * Model;
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Use our shader


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        // Draw the triangle !
        glUseProgram(programID1);
        GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
        glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glUseProgram(programID2);
        GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glDrawArrays(GL_TRIANGLES, 3, 3);

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID1);
    glDeleteProgram(programID2);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

