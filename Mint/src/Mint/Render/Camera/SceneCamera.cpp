#include"pch.h"
#include"SceneCamera.h"


MT_NAMESPACE_BEGIN
SceneCamera::SceneCamera()
{
}
void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
{
	m_projType = ProjectionType::Perspective;
	m_PerspectiveFOV = verticalFOV;
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;
	RecalculateProjection();
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
	m_projType = ProjectionType::Orthographic;
	m_OrthographicSize = size;
	m_OrthographicNear = nearClip;
	m_OrthographicFar = farClip;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	MT_ASSERT(width > 0 && height > 0)
	m_AspectRatio = (float)width / (float)height;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	if (m_projType == ProjectionType::Perspective)
	{
		m_proj = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
	}
	else
	{
		float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicSize * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;

		m_proj = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
	}
}

MT_NAMESPACE_END