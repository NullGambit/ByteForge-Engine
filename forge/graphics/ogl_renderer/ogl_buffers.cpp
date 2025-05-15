#include "ogl_buffers.hpp"
#include <cassert>

void forge::OglBuffers::bind() const
{
    glBindVertexArray(vao);
}

void forge::OglBuffers::unbind() const
{
    glBindVertexArray(0);
}

void forge::OglBuffers::free() const
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

forge::OglBufferBuilder& forge::OglBufferBuilder::start()
{
    glGenVertexArrays(1, &m_buffer.vao);
    glBindVertexArray(m_buffer.vao);

    return *this;
}

forge::OglBufferBuilder& forge::OglBufferBuilder::vbo(View<Vertex> verts)
{
    set_vbo(verts.data, sizeof(Vertex) * verts.size);
    return *this;
}

forge::OglBufferBuilder & forge::OglBufferBuilder::ebo(View<u32> indices)
{
    glGenBuffers(1, &m_buffer.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer.ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size, indices.data, GL_STATIC_DRAW);

    return *this;
}

forge::OglBufferBuilder& forge::OglBufferBuilder::stride(u32 value)
{
    m_stride = value;

    return *this;
}

forge::OglBufferBuilder& forge::OglBufferBuilder::attr(u32 size)
{
    assert(m_stride != INT32_MAX && "stride has not been set");

    auto layout = m_layout++;

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, m_stride, (void*)(m_offset * sizeof(f32)));

    m_offset += size;

    return *this;
}

forge::OglBuffers forge::OglBufferBuilder::finish() const
{
    glBindVertexArray(0);

    return m_buffer;
}

void forge::OglBufferBuilder::set_vbo(const void *data, size_t size)
{
    glGenBuffers(1, &m_buffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
