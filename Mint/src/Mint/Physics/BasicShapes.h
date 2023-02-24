#pragma once
#include"glm/glm.hpp"


namespace Mint {

    enum class Gshape { none = 0, box, sphere, capsule };

    class Geometry {
    public:
        Geometry() = default;
        virtual ~Geometry() = default;
        virtual Gshape GetShape() = 0;
    };

    class Box : public Geometry {
    public:
        Box() = default;
        ~Box() override {}
        virtual Gshape GetShape()override { return Gshape::box; }
        glm::vec3 m_half_extents{ 0.5f, 0.5f, 0.5f };
    };

    class Sphere : public Geometry
    {
    public:
        Sphere() = default;
        ~Sphere() override {}
        virtual Gshape GetShape()override { return Gshape::sphere; }
        float m_radius{ 0.5f };
    };

    class Capsule : public Geometry
    {
    public:
        Capsule() = default;
        ~Capsule() override {}
        virtual Gshape GetShape()override { return Gshape::capsule; }
        float m_radius{ 0.3f };
        float m_half_height{ 0.7f };
    };
}