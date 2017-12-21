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


    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::update() {
        _transform = glm::translate(_transform, _translation);
        _transform *= _rotation;
        _transform = glm::scale(_transform, _scale);
        *_driven_transform = _transform;
    }

    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::reset() {
        _translation = glm::vec3();
        _rotation = glm::mat4_cast(glm::quat(1, 0, 0, 0));
        _scale = glm::vec3(1, 1, 1);
        update();
    }

    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::rotate(glm::vec3 v) {
        auto quat = glm::quat(v);
        _rotation = glm::mat4_cast(quat);
        update();
    }

    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::rotate(glm::quat v) {
        _rotation = glm::mat4_cast(v);
        update();
    }

    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::look_at(glm::vec3 pos, glm::vec3 tgt, glm::vec3 up) {
        _rotation = glm::lookAt(pos, tgt, up);
        update();
    }
    
    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::scale(glm::vec3 v) {
        _scale = v;
        update();
    }

    template <typename DRIVABLE>
    void DirectDriver<DRIVABLE>::translate(glm::vec3 v) {
        _translation = v;
        update();
    }
}