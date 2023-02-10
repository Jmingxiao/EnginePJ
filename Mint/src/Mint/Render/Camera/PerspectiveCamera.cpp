#include "pch.h"
#include "PerspectiveCamera.h"
#include <glm/gtx/quaternion.hpp>


MT_NAMESPACE_BEGIN

PerspectiveCamera::PerspectiveCamera(float p_fov, float p_aspectRatio, float p_near, float p_far)
	:m_fov(p_fov), m_aspectRatio(p_aspectRatio), m_near(p_near), m_far(p_far),
	Camera(glm::perspective(glm::radians(p_fov), p_aspectRatio, p_near, p_far))
{
	type = CameraType::Pespective;
	RecalculateViewMatrix();
}

const glm::vec3& PerspectiveCamera::GetPosition() const
{
	return m_pos;
}

void PerspectiveCamera::SetPosition(const glm::vec3& position)
{
	m_pos = position;
	RecalculateViewMatrix();
}

glm::vec3 PerspectiveCamera::GetRotation() const
{
	return glm::vec3(pitch,yaw,roll);
}

void PerspectiveCamera::SetRotation(const glm::vec3& rotation)
{
	pitch = rotation.x;
	yaw = rotation.y;
	roll = rotation.z;
	RecalculateViewMatrix();
}


void PerspectiveCamera::RecalculateViewMatrix()
{
	using namespace glm;
	vec3 camdir;
	camdir.x = cos(radians(yaw)) * cos(radians(pitch));
	camdir.y = sin(radians(pitch));
	camdir.z = sin(radians(yaw)) * cos(radians(pitch));

	forward = normalize(camdir);
	right = normalize(cross(forward, vec3(0.0f, 1.0f, .0f)));
	up = normalize(cross(right, forward));

	m_view = lookAt(m_pos, m_pos + forward, up);
}


MT_NAMESPACE_END

