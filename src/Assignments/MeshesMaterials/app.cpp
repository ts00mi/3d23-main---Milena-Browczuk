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
#include "Application/application.h"

#include "camera.h"
#include "camera_controller.h"

#include "Engine/Mesh.h"
#include "Engine/Material.h"


void SimpleShapeApplication::init() {

    set_camera(new Camera);

    set_controller(new CameraController(camera()));

    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
        { {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
         {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"} });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    xe::ColorMaterial::init();

    auto pyramid = new xe::Mesh;

    // Pyramid - depth buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // A vector containing the x,y,z vertex coordinates *RGB parameters deleted (assigned in submeshes)
    // it also contains  data for pyramid -> simplified
    std::vector<GLfloat> vertices = {

            // Base
            -0.5f, 0.5f, 0.0f,
             0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,

            // Peak
            0.0f, 0.0f, 1.0f,
    };

    std::vector<GLushort> indices =
    {
            // Base
            0, 1, 2,
            2, 1, 3,

            // 1st face
            0, 4, 1,

            // 2nd face
            1, 4, 3,

            // 3rd face
            3, 4, 2,

            // 4th face
            2, 4, 0,
    };

    // Vertex initialization | Mesh 
    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW); // Allocate memory for vertex buffer
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data()); // Load appriopriate data | Vertcies
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);  // Position | Attribute

    // Indices initialization | Mesh
    pyramid->allocate_index_buffer(indices.size() * sizeof(GLushort), GL_STATIC_DRAW); // Allocate memory for indices buffer
    pyramid->load_indices(0, indices.size() * sizeof(GLushort), indices.data()); // Load appriopriate data | Indices

    // Adding submesh to a pyramid
    // Splitting the previous index buffer into submeshes for each pyramid face as individual
    // Assigning a material to a submesh

    // Base
    pyramid->add_submesh(0, 6, new xe::ColorMaterial(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));

    // 1st face
    pyramid->add_submesh(6, 9, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));

    //2nd face
    pyramid->add_submesh(9, 12, new xe::ColorMaterial(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)));

    // 3rd face
    pyramid->add_submesh(12, 15, new xe::ColorMaterial(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

    // 4th face
    pyramid->add_submesh(15, 18, new xe::ColorMaterial(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));

    meshes_.push_back(pyramid); //Add pyramid to meshes_ vector

    //Uniforms floats
    float strength = 1.0f;
    float color[3] = { 1.0f, 1.0f, 1.0f };

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
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), &color);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

    //Kolorki, dla location=1 + offset
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    int w, h;
    std::tie(w, h) = frame_buffer_size();

    camera()->look_at(glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0));

    float aspect_ = static_cast<float>(w) / h;
    float fov_ = glm::pi<float>() / 2.0;
    float near_ = 0.1f;
    float far_ = 100.0f;

    camera()->perspective(fov_, aspect_, near_, far_);

    //Generating u_pvm_buffer_
    //GLuint u_pvm_buffer_; -> overwritten variable (already existing in class (app.h))
    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);

    glViewport(0, 0, w, h);
    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {

    auto PVM = camera()->projection() * camera()->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);

    // MaterialMeshes
    for (auto m : meshes_)
        m->draw();

    glBindVertexArray(0);  
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float)w / h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    std::cout << "Scroll offset: " << yoffset << std::endl; // Scroll Movement Logs
    camera()->zoom(yoffset * 0.1f);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controller_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controller_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controller_->LMB_released(x, y);
    }
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controller_) {
        controller_->mouse_moved(x, y);
    }
}
