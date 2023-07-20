#include "pch.h"
#include "Camera.h"
#include "Common/Input.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Acg::Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_fov(fov), m_aspectRatio(aspectRatio), m_near(nearClip), m_far(farClip), m_proj(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
{
    UpdateView();
    UpdateProjection();
}

void Acg::Camera::OnUpdate(float dt)
{
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
    KeyPress(dt);
    UpdateView();
    UpdateProjection();

}

void Acg::Camera::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(ACG_BIND_EVENT_FN(Camera::OnMouseScroll));
    dispatcher.Dispatch<WindowResizeEvent>(ACG_BIND_EVENT_FN(Camera::OnWindowResize));
}

bool Acg::Camera::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() != m_viewportWidth || e.GetHeight() != m_viewportHeight) {
        SetViewportSize(e.GetWidth(), e.GetHeight());
    }
    return false;
}

glm::vec3 Acg::Camera::GetUpDirection() const
{
    return up;
}

glm::vec3 Acg::Camera::GetRightDirection() const
{
    return right;
}

glm::vec3 Acg::Camera::GetForwardDirection() const
{
    return forward;
}

glm::quat Acg::Camera::GetOrientation() const
{
    return glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
}

std::vector<glm::vec4> Acg::Camera::GetFrustumCorners(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

void Acg::Camera::UpdateProjection()
{
    m_aspectRatio = (float)m_viewportWidth / (float)m_viewportHeight;
    m_proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
}

void Acg::Camera::UpdateView()
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

bool Acg::Camera::OnMouseScroll(MouseScrolledEvent& e)
{
    float delta = e.GetYOffset() * 0.1f;
    MouseZoom(delta);
    return false;
}

void Acg::Camera::MouseZoom(float delta)
{
    m_fov -= delta;
    m_fov = std::max(30.0f, m_fov);
    m_fov = std::min(60.0f, m_fov);
}

void Acg::Camera::MouseRotation(const glm::vec2& delta, bool constrain)
{
    yaw += delta.x;
    pitch -= delta.y;

    if (constrain)
    {
        pitch = std::max(-80.0f, pitch);
        pitch = std::min(80.0f, pitch);
    }
}

void Acg::Camera::KeyPress(float dt)
{
    bool ispressed = Input::IsKeyPressed(Key::LeftShift);
    float velocity = ispressed ? 2.0f * cameraSpeed * dt : cameraSpeed * dt;

    if (Input::IsKeyPressed(Key::W))
        m_pos += forward * velocity;
    else if (Input::IsKeyPressed(Key::S))
        m_pos -= forward * velocity;
    if (Input::IsKeyPressed(Key::A))
        m_pos -= right * velocity;
    else if (Input::IsKeyPressed(Key::D))
        m_pos += right * velocity;
}

