#include "pch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

MT_NAMESPACE_BEGIN

static unsigned int DataTypeToOpenGLBaseType(DataType type)
{
	switch (type) 
	{
	case DataType::Float:    return GL_FLOAT;
	case DataType::Float2:   return GL_FLOAT;
	case DataType::Float3:   return GL_FLOAT;
	case DataType::Float4:   return GL_FLOAT;
	case DataType::Mat3:     return GL_FLOAT;
	case DataType::Mat4:     return GL_FLOAT;
	case DataType::Int:      return GL_INT;
	case DataType::Int2:     return GL_INT;
	case DataType::Int3:     return GL_INT;
	case DataType::Int4:     return GL_INT;
	case DataType::Bool:     return GL_BOOL;
	}

	MT_ASSERT(false, "Unknown DataType!");
	return 0;
}


OpenGLVertexArray::OpenGLVertexArray()
	:m_RendererID(-1)
{
	glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	MT_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();
	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case DataType::Float:
		case DataType::Float2:
		case DataType::Float3:
		case DataType::Float4:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				DataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case DataType::Int:
		case DataType::Int2:
		case DataType::Int3:
		case DataType::Int4:
		case DataType::Bool:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribIPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				DataTypeToOpenGLBaseType(element.Type),
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case DataType::Mat3:
		case DataType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					count,
					DataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		default:
			MT_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}


MT_NAMESPACE_END
