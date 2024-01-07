//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Application/utils.h"

namespace xe {

    class Material {
    public:
        virtual void bind() = 0;

        virtual void unbind() {};
    };

    class ColorMaterial : public Material {
    public:
        // texture_ and texture_unit_ values added to class constructor
        ColorMaterial(const glm::vec4 color) : color_(color), texture_(0), texture_unit_(0) {} 

        void bind();

        static void init();

        static GLuint program() { return shader_; }

        //Get | methods for texture_ and texture_unit_
        GLuint get_texture() const { return texture_; }
        GLuint get_texture_unit() const { return texture_unit_; }

        // Set | methods for texture_ and texture_unit_
        void set_texture(GLuint tex) { texture_ = tex; }
        void set_texture_unit(GLuint texUnit) { texture_unit_ = texUnit; }

    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;
        static GLint uniform_map_Kd_location_;

        glm::vec4 Kd;         
        bool use_map_Kd;

        glm::vec4 color_;
        GLuint texture_;
        GLint texture_unit_;

    };


}


