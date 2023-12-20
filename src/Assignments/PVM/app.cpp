//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the (triangle) -> 3x for house + RGB parameters for colors
    std::vector<GLfloat> vertices = 
    {
            -0.5f, 0.0f, 0.0f, 1.0, 0.0, 0.0,
            0.5f, 0.0f, 0.0f, 1.0, 0.0, 0.0,
            0.0f, 0.5f, 0.0f, 1.0, 0.0, 0.0,

            -0.5f, 0.0f, 0.0f, 0.0, 1.0, 0.0,
            -0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
            0.5f, 0.0f, 0.0f, 0.0, 1.0, 0.0,

            -0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
            0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0,
            0.5f, 0.0f, 0.0f, 0.0, 1.0, 0.0,
    };

    //Optimized vertices due to position (x, y, z & r, g, b)
    std::vector<GLushort> indices = 
    {
            0, 1, 2, 
            3, 4, 5, 
            4, 7, 5
    }; 

    //Uniforms floats
        float strength = 0.9f;
        float color[3] = {0.4f, 0.2f, 0.7f};

    //"Moving houses" - uniforms movement parameters
    //    float theta = 1.0 * glm::pi<float>() / 6.0f;
    //    auto cs = std::cos(theta);
    //    auto ss = std::sin(theta);

    // Projection | Matrix   
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 0.1f, 0.1f, 100.0f);
    // Camera View | Matrix
        glm::mat4 View = glm::lookAt(
            glm::vec3(4, 8, 3), // Camera coordinates in World Space
            glm::vec3(0, 0, 0), // "looks at" the origin
            glm::vec3(2, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down) -> from OpenGL Documentations
        );
    // Model | Matrix
        glm::mat4 Model = glm::mat4(1.0f);
    // PVM | Matrix
        glm::mat4 PVM = Projection * View * Model; //remember! The matrix multiplication is in inverted order | standard: MVP - PVM



    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Generating indices buffer
    GLuint i_buffer_handle;
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Generating uniforms buffer | colors + PVM
    GLuint u_buffer_handle;
    glGenBuffers(1, &u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handle);
    
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(strength), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float),&color);
    
    //Generating uniforms buffer | movement + PVM
    GLuint u_buffer_movement_handle;
    glGenBuffers(1, &u_buffer_movement_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_movement_handle);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_buffer_movement_handle);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(glm::mat4), &PVM[1]);
    glBufferSubData(GL_UNIFORM_BUFFER, 8 * sizeof(glm::vec4), sizeof(glm::mat4), &PVM[2]);
    glBufferSubData(GL_UNIFORM_BUFFER, 12 * sizeof(glm::vec4), sizeof(glm::mat4), &PVM[3]);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle); 

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    
    //Kolorki, dla location=1 + offset
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
