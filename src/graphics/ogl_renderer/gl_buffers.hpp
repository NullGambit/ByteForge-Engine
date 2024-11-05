#pragma once

#include <vector>
#include <glad/glad.h>
#include <span>

namespace forge
{
    struct GlBuffers
    {
        u32 vbo;
        u32 vao;
        u32 ebo;

        void free() const
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }
    };

    class GlBufferBuilder
    {
    public:
        GlBufferBuilder& start();

        GlBufferBuilder& vbo(const std::span<const f32> &verts);

        GlBufferBuilder& ebo(const std::vector<u32> &verts);

        GlBufferBuilder& stride(u32 value);

        GlBufferBuilder& attr(u32 size);

        [[nodiscard]]
        GlBuffers finish() const;

    private:
        GlBuffers m_buffer {};
        i32 m_layout = 0;
        u32 m_offset = 0;
        i32 m_stride = INT32_MAX;

        void set_vbo(const void *data, size_t size);
    };
}
