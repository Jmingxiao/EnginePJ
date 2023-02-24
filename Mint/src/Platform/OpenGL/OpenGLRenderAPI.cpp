#include "pch.h"
#include "OpenGLRenderAPI.h"
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>



MT_NAMESPACE_BEGIN


void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:       MT_ERROR(message); return;
	case GL_DEBUG_SEVERITY_LOW:          MT_WARN(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: MT_TRACE(message); return;
	}

	MT_ASSERT(false, "Unknown severity level!");
}

void OpenGLRenderAPI::Init()
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void OpenGLRenderAPI::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRenderAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void OpenGLRenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRenderAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
{
	vertexArray->Bind();
	glDrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGLRenderAPI::DrawFullScreenQuad()
{
	GLboolean previous_depth_state;
	glGetBooleanv(GL_DEPTH_TEST, &previous_depth_state);
	glDisable(GL_DEPTH_TEST);

	static GLuint vertexArrayObject = 0;
	static int nofVertices = 6;
	if (vertexArrayObject == 0)
	{
		GLuint vb =0;
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);
		static const glm::vec2 positions[] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f },
											   { -1.0f, -1.0f }, { 1.0f, 1.0f },  { -1.0f, 1.0f } };
		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false /*normalized*/, sizeof(float) * 2 /*stride*/, 0 /*offset*/);
	}
	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, nofVertices);

	if (previous_depth_state)
		glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLRenderAPI::SetLineWidth(float width)
{
	glLineWidth(width);
}

MT_NAMESPACE_END