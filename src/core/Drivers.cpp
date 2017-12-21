namespace Kvant {
    template <typename DRIVABLE>
    void HoverDriver<DRIVABLE>::dolly(float d) {
        auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
        auto deltaPos = cam_dir * d;
        *_driven_pos += deltaPos;
        *_driven_tgt += deltaPos;
        retransform();
    }

    template <typename DRIVABLE>
    void HoverDriver<DRIVABLE>::truck(float d) {
        auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
        auto side_dir = glm::cross(cam_dir, _up);
        auto deltaPos = side_dir * d;
        *_driven_pos += deltaPos;
        *_driven_tgt += deltaPos;
        retransform();
    }

    template <typename DRIVABLE>
    void HoverDriver<DRIVABLE>::follow(float a1, float a2) {
        auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
        cam_dir.y += -a2;
        auto side_dir = glm::cross(cam_dir, _up);
        cam_dir += side_dir * -a1;
        cam_dir = glm::normalize(cam_dir);
        *_driven_tgt = *_driven_pos + cam_dir;
        retransform();
    }

    template <typename DRIVABLE>
    void HoverDriver<DRIVABLE>::pan(float a1) {
        auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
        auto side_dir = glm::cross(cam_dir, _up);
        cam_dir += (side_dir * -a1);
        cam_dir = glm::normalize(cam_dir);
        *_driven_tgt = *_driven_pos + cam_dir;
        retransform();
    }

    template <typename DRIVABLE>
    void HoverDriver<DRIVABLE>::strafe(float d) {
        auto cam_dir = glm::normalize(*_driven_tgt - *_driven_pos);
        auto side_dir = glm::cross(cam_dir, _up);
        auto delta = side_dir * d;
        *_driven_pos += delta;
        *_driven_tgt += delta;
        retransform();
    }
}