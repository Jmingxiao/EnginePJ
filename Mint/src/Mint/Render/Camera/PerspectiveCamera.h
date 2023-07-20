#pragma once

#include "Camera.h"

MT_NAMESPACE_BEGIN

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera() = default;
	PerspectiveCamera(float p_fov, float p_aspectRatio, float p_near, float p_far);
	
	void SetPosition(const glm::vec3& position);

	glm::vec3 GetRotation() const;
	void SetRotation(const glm::vec3& rotation);
	

protected:
	void RecalculateViewMatrix();
protected:
	float pitch = 0.0f, yaw =0.0f, roll = 0.0f;
	float m_fov = 45.0f, m_aspectRatio = 1.8f, m_near = 0.1f, m_far = 1000.0f;
	glm::vec3 forward = { 0.0f, 0.0f, 1.0f },right = { -1.0f, 0.0f, 0.0f },up = { 0.0f, 1.0f, 0.0f };
};

MT_NAMESPACE_END