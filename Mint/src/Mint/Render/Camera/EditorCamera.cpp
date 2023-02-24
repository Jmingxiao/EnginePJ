#include "pch.h"
#include "EditorCamera.h"

#include "Mint/Core/Input.h"
#include"Mint/Core/KeyCodes.h"


MT_NAMESPACE_BEGIN

EditorCamera::EditorCamera(float p_fov, float p_aspectRatio, float p_near, float p_far)
	:PerspectiveCamera(p_fov,p_aspectRatio,p_near,p_far)
{
	RecalculateViewMatrix();
	RecalculateProjMatrix();
}

EditorCamera::EditorCamera()
{
	RecalculateViewMatrix();
	RecalculateProjMatrix();
}

void EditorCamera::OnUpdate(Timestep ts)
{
	if (!m_blocked) {
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		if (firstmouse)
		{
			previousmouse = mouse;
			firstmouse = false;
		}
		glm::vec2 delta = (mouse - previousmouse) * 0.05f;
		previousmouse = mouse;
		if (Input::IsMouseButtonPressed(Mouse::Right)) {
			MouseRotation(delta);
		}
		KeyPress(0.01f);
		RecalculateViewMatrix();
		RecalculateProjMatrix();
	}
}

void EditorCamera::OnEvent(Event& e)
{
	if (!m_blocked) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(MT_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}
}

bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
{
	float delta = e.GetYOffset() * 0.1f;
	MouseZoom(delta);
	return false;
}

void EditorCamera::MouseZoom(float delta)
{
	m_fov -= delta;
	m_fov=std::max(30.0f, m_fov);
	m_fov=std::min(60.0f, m_fov);
}

void EditorCamera::RecalculateProjMatrix()
{
	m_aspectRatio = m_viewportWidth / m_viewportHeight;
	m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
}

void EditorCamera::MouseRotation(const glm::vec2& delta,bool constrain)
{
	yaw   += delta.x;
	pitch -= delta.y;

	if (constrain)
	{
		pitch = std::max(-80.0f,pitch);
		pitch = std::min(80.0f, pitch);
	}
}

void EditorCamera::KeyPress(float dt)
{
	bool ispressed = Input::IsKeyPressed(Key::LeftShift);
	float velocity = ispressed?2.0f*cameraSpeed * dt:cameraSpeed*dt;

	if (Input::IsKeyPressed(Key::W))
		m_pos += forward * velocity;
	else if (Input::IsKeyPressed(Key::S))
		m_pos -= forward * velocity;
	if (Input::IsKeyPressed(Key::A))
		m_pos -= right * velocity;
	else if (Input::IsKeyPressed(Key::D))
		m_pos += right * velocity;

}



MT_NAMESPACE_END

