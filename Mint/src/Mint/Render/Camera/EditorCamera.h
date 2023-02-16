#pragma once
#include "PerspectiveCamera.h"
#include "Mint/Core/TimeStep.h"
#include "Mint/Events/MouseEvent.h"
#include "Mint/Events/KeyEvent.h"
#include "Mint/Events/Event.h"

MT_NAMESPACE_BEGIN

class EditorCamera : public PerspectiveCamera {
public:
	EditorCamera() ;
	EditorCamera(float p_fov, float p_aspectRatio, float p_near, float p_far);

	void OnUpdate(Timestep ts);
	void OnEvent(Event& e);
	inline void SetViewportSize(float width, float height) 
	{ m_viewportWidth = width; m_viewportHeight = height; RecalculateProjMatrix(); }
	inline void SetControlActive(bool blocked) { m_blocked = blocked; }

private:
	bool OnMouseScroll(MouseScrolledEvent& e);
	void MouseZoom(float delta);
	void RecalculateProjMatrix();
	void MouseRotation(const glm::vec2& delta,bool constrain =true);
	void KeyPress(float dt);

private:
	bool firstmouse =true;
	glm::vec2 previousmouse = {0.0f,0.0f};
	float cameraSpeed=2.5f;
	float m_viewportWidth = 1280, m_viewportHeight = 720;
	bool m_blocked = false;
	
};

MT_NAMESPACE_END


