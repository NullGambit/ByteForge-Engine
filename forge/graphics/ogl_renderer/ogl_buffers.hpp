#pragma once

#include <vector>
#include <glad/glad.h>
#include <span>

namespace forge
{
    struct OglBuffers
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

    class OglBufferBuilder
    {
    public:
        OglBufferBuilder& start();

        OglBufferBuilder& vbo(const std::span<const f32> &verts);

        OglBufferBuilder& ebo(const std::vector<u32> &verts);

        OglBufferBuilder& stride(u32 value);

        template<class ...Args>
        OglBufferBuilder& stride()
        {
            m_stride = (sizeof(Args) + ...);
            return *this;
        }

        OglBufferBuilder& attr(u32 size);

        [[nodiscard]]
        OglBuffers finish() const;

    private:
        OglBuffers m_buffer {};
        i32 m_layout = 0;
        u32 m_offset = 0;
        i32 m_stride = INT32_MAX;

        void set_vbo(const void *data, size_t size);
    };
}
