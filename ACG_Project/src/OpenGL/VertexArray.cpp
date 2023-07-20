#include "pch.h"
#include "VertexArray.h"
#include "glad/glad.h"

namespace Acg {

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

		LOG_ASSERT(false, "Unknown DataType!");
		return 0;
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		LOG_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

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
				LOG_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}