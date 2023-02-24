#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mint{

	enum class ProjectionType 
	{
		Perspective=0, Orthographic
	};
	struct frustum {
		enum class side {
			Left,
			Right,
			Top,
			Bottom,
			Near,
			Far,
			count,
		};
		void construct(const glm::mat4& projview) {

			planes_[int(side::Left)].x = projview[3][0] + projview[0][0];
			planes_[int(side::Left)].y = projview[3][1] + projview[0][1];
			planes_[int(side::Left)].z = projview[3][2] + projview[0][2];
			planes_[int(side::Left)].w = projview[3][3] + projview[0][3];

			planes_[int(side::Right)].x = projview[3][0] - projview[0][0];
			planes_[int(side::Right)].y = projview[3][1] - projview[0][1];
			planes_[int(side::Right)].z = projview[3][2] - projview[0][2];
			planes_[int(side::Right)].w = projview[3][3] - projview[0][3];

			planes_[int(side::Bottom)].x = projview[3][0] + projview[1][0];
			planes_[int(side::Bottom)].y = projview[3][1] + projview[1][1];
			planes_[int(side::Bottom)].z = projview[3][2] + projview[1][2];
			planes_[int(side::Bottom)].w = projview[3][3] + projview[1][3];

			planes_[int(side::Top)].x = projview[3][0] - projview[1][0];
			planes_[int(side::Top)].y = projview[3][1] - projview[1][1];
			planes_[int(side::Top)].z = projview[3][2] - projview[1][2];
			planes_[int(side::Top)].w = projview[3][3] - projview[1][3];

			planes_[int(side::Near)].x = projview[3][0] + projview[2][0];
			planes_[int(side::Near)].y = projview[3][1] + projview[2][1];
			planes_[int(side::Near)].z = projview[3][2] + projview[2][2];
			planes_[int(side::Near)].w = projview[3][3] + projview[2][3];

			planes_[int(side::Far)].x = projview[3][0] - projview[2][0];
			planes_[int(side::Far)].y = projview[3][1] - projview[2][1];
			planes_[int(side::Far)].z = projview[3][2] - projview[2][2];
			planes_[int(side::Far)].w = projview[3][3] - projview[2][3];

			for (int i = 0; i < int(side::count); i++) {
				glm::vec3 normal = glm::vec3(planes_[i]);
				const float length = glm::length(normal);
				planes_[i].x = normal.x / length;
				planes_[i].y = normal.y / length;
				planes_[i].z = normal.z / length;
				planes_[i].w /= length;
			}

		}
		bool is_inside(const glm::vec3& position) const {
			for (int index = 0; index < int(side::count); index++) {
				const glm::vec3 normal(planes_[index]);
				const float d = planes_[index].w;
				const float distance = glm::dot(normal, position) + d;
				if (distance < 0.0f) {
					return false;
				}
			}
			return true;
		}

		// Ax + By + Cz + D = 0
		glm::vec4 planes_[int(side::count)];
	};


	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_proj(projection) {}

		virtual ~Camera() = default;
		virtual const glm::vec3& GetPosition() const { return m_pos; }
		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetProjection() const { return m_proj; }
		const glm::mat4& GetViewProjectionMatrix() const { return std::move(m_proj * m_view);}
		virtual void SetViewMatrix(const glm::mat4 view) { m_view = view; }
	protected:
		glm::vec3 m_pos = { 0.0f, 5.0f, 15.0f };
		glm::mat4 m_proj = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);

	public:
		ProjectionType type = ProjectionType::Perspective;
	};


}