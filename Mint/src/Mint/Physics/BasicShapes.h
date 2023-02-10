#pragma once

#include"glm/glm.hpp"

namespace Mint {

    enum class Gshape{none=0,box,sphere,capsule};

	class Geometry {
	public:
        Gshape type{Gshape::none};
		virtual ~Geometry() {}
	};

	class Box : public Geometry {
	public:
		~Box() override {}
		glm::vec3 m_half_extents{ 0.5f, 0.5f, 0.5f };
        Gshape type{ Gshape::box };
	};

    class Sphere: public Geometry
    {
    public:
        ~Sphere() override {}
        float m_radius{ 0.5f };
        Gshape type{ Gshape::sphere };
    };

    class Capsule : public Geometry
    {
    public:
        ~Capsule() override {}
        float m_radius{ 0.3f };
        float m_half_height{ 0.7f };
        Gshape type{ Gshape::capsule };
    };


}