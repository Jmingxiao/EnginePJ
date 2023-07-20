#pragma once
#include "Common/Event.h"
#include "glm/glm.hpp"

namespace Acg {
	class Camera {

	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(float dt);
		void OnEvent(Event& e);
		bool OnWindowResize(WindowResizeEvent& e);

		inline void SetViewportSize(uint32_t width, uint32_t height) { m_viewportWidth = width; m_viewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_view; }
		glm::mat4 GetViewProjection() const { return m_proj * m_view; }
		const glm::mat4& GetProjection() const { return m_proj; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_pos; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return pitch; }
		float GetYaw() const { return yaw; }
		float GetFov() const { return m_fov; }
		float GetAspect() const { return m_aspectRatio; }
		float GetNearPlane() const { return m_near; }
		float GetFarPlane() const { return m_far; }


		static std::vector<glm::vec4> GetFrustumCorners(const glm::mat4& projview);


		uint32_t m_viewportWidth = 1600, m_viewportHeight = 900;
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);
		void MouseZoom(float delta);
		void MouseRotation(const glm::vec2& delta, bool constrain = true);
		void KeyPress(float dt);
	private:
		bool firstmouse = true;
		glm::vec2 previousmouse = { 0.0f,0.0f };
		float m_fov = 45.0f, m_aspectRatio = 1.8f, m_near = 0.2f, m_far = 1000.0f;

		glm::mat4 m_view;
		glm::mat4 m_proj = glm::mat4(1.0f);
		glm::vec3 m_pos = { 0.0f, 5.0f, 20.0f };

		glm::vec3 forward = { 0.0f, 0.0f, 1.0f }, right = { -1.0f, 0.0f, 0.0f }, up = { 0.0f, 1.0f, 0.0f };
		float cameraSpeed = 2.5f;

		float pitch = 0.0f, yaw = -90.0f;

	};
}