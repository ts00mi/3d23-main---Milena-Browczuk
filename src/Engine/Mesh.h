//
// Created by Piotr Białas on 25/10/2021.
//

#pragma once

#include <vector>
#include "glad/gl.h"

#include "Engine/Material.h"


namespace xe {

    struct SubMesh {
        SubMesh(GLuint start, GLuint end, Material* mat = nullptr) : start(start), end(end), material(mat) {}

        GLuint start;
        GLuint end;

        GLuint count() const { return end - start; }

        Material* material; // Material variable included in SubMesh class
    };

    class Mesh {
    public:
        Mesh();

        void allocate_vertex_buffer(size_t size, GLenum hint);

        void allocate_index_buffer(size_t size, GLenum hint);

        void load_vertices(size_t offset, size_t size, void* data);

        void load_indices(size_t offset, size_t size, void* data);

        void vertex_attrib_pointer(GLuint index, GLuint size, GLenum type, GLsizei stride, GLsizei offset);

        void add_submesh(GLuint start, GLuint end, Material* mat) {
            submeshes_.push_back({ start, end });
            materials_.push_back(mat);
        }

        void add_submesh(GLuint start, GLuint end) {
            add_submesh(start, end, nullptr); 
        }

        void draw() const;

    private:

        GLuint vao_;
        GLuint v_buffer_;
        GLuint i_buffer_;

        std::vector<SubMesh> submeshes_;

        std::vector<Material*> materials_;
    };

}
