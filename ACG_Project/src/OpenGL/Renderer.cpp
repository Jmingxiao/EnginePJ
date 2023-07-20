#include "pch.h"
#include "Renderer.h"
#include "glad/glad.h"

void Acg::RendererCommand::Init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Acg::RendererCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void Acg::RendererCommand::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Acg::RendererCommand::Clear()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Acg::RendererCommand::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void Acg::RendererCommand::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
{
	vertexArray->Bind();
	glDrawArrays(GL_LINES, 0, vertexCount);
}

void Acg::RendererCommand::DrawFullScreenQuad()
{
	GLboolean previous_depth_state;
	glGetBooleanv(GL_DEPTH_TEST, &previous_depth_state);
	glDisable(GL_DEPTH_TEST);
	static GLuint vertexArrayObject = 0;
	static int nofVertices = 6;
	if (vertexArrayObject == 0)
	{
		GLuint vb = 0;
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);
		static const glm::vec2 positions[] = { { -1.0f, -1.0f } ,{0.0f,0.0f}, {1.0f, -1.0f},{1.0f,0.0f}, 
												{1.0f, 1.0f}, {1.0f,1.0f},{ -1.0f, -1.0f }, {0.0f,0.0f} ,
												{ 1.0f, 1.0f }, {1.0f,1.0f}, { -1.0f, 1.0f } ,{0.0f,1.0f} };
		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false /*normalized*/, sizeof(float) * 4/*stride*/, 0 /*offset*/);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false /*normalized*/, sizeof(float) * 4 /*stride*/, (void*)(2 * sizeof(float)) /*offset*/);
	}
	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, nofVertices);

	if (previous_depth_state)
		glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Acg::RendererCommand::DrawPartScreenQuad()
{
	GLboolean previous_depth_state;
	glGetBooleanv(GL_DEPTH_TEST, &previous_depth_state);
	glDisable(GL_DEPTH_TEST);
	static GLuint vertexArrayObject = 0;
	static int nofVertices = 6;
	if (vertexArrayObject == 0)
	{
		GLuint vb = 0;
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);
		static const glm::vec2 positions[] = { { -1.0f, 0.5f },{0.0f,0.0f}, { -0.5f, 0.5f },{1.0f,0.0f}, { -0.5f, 1.0f },{1.0f,1.0f},
											   { -1.0f, 0.5f },{0.0f,0.0f}, { -0.5f, 1.0f },{1.0f,1.0f}, { -1.0f, 1.0f },{0.0f,1.0f} };
		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false /*normalized*/, sizeof(float) * 4 /*stride*/, 0 /*offset*/);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false /*normalized*/, sizeof(float) * 4 /*stride*/, (void*)(2 * sizeof(float)) /*offset*/);
	}
	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, nofVertices);

	if (previous_depth_state)
		glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Acg::RendererCommand::DrawFrustum()
{
}
