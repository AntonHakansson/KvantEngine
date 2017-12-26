#ifndef ANIMATION_HPP_INCLUDED
#define ANIMATION_HPP_INCLUDED

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Kvant {

    /**
     * The Frame stores the all the bone matrices for a frame
     * Frames have to be "baked" before use
     * Baking converts the raw bone data into bone tranformation matrices
     */
    struct Frame {
        std::vector<int> bone_parents; // Value of -1 equals no parent
        std::vector<glm::vec3> bone_positions;
        std::vector<glm::quat> bone_rotations;

        // The transform that moves the bone, from it's resting frame, to the new pose
        // This is what gets baked and pushed to the vertex shader
        std::vector<glm::mat4> bone_transforms;
        std::vector<glm::mat4> bone_inv_transforms;
        bool is_baked{false};

        glm::mat4 bone_transform(int i) {
            glm::mat4 parent;
            glm::mat4 ret;
            // Recursively get the parents bone transform
            if (this->bone_parents[i] != -1) {
                glm::mat4 parent = this->bone_transform(this->bone_parents[i]);
                ret *= parent; // Move bone to parent
            }
            // pos and rot are relative to parent
            glm::mat4 pos = glm::translate(glm::mat4(), this->bone_positions[i]);
            glm::mat4 rot = glm::mat4_cast(this->bone_rotations[i]);
            ret *= pos; // Add position offset from parent
            ret *= rot; // Add rotation offset from parent
            return ret;
        }

        void bake_transforms() {
            auto bones = bone_parents.size();
            this->bone_transforms.clear();
            this->bone_inv_transforms.clear();

            // iterate over all bones
            for (auto i = 0; i < bones; i++) {
                glm::mat4 t = bone_transform(i);
                this->bone_transforms.push_back(t);
                this->bone_inv_transforms.push_back(glm::inverse(t));
            }
            this->is_baked = true;
        }
    };

    /**
     * Represents a bone/joint in the skeleton
     */
    struct Bone {
        std::string name;
        int parent;       // The parent bone transforms it's child's transform

        Bone(std::string n, int p) : name(n), parent(p) {}
    };

    /**
     * The skeleton consists of a list of bones and the skeleton's resting keyframe
     */
    struct Skeleton {
        std::vector<Bone> bones;
        Frame rest;

        int n_bones() const {
            return bones.size();
        }
    };

    class Animation {
        public:
            Animation(std::string filename, const Skeleton& s, bool loop = true);
            
            void update(const Kvant::CoreContext& ctx) {
                _animation_time += ctx.elapsed;
            }

            void reset() {
                _animation_time = 0;
            }

            static std::unique_ptr<Frame> interpolate(const Frame& f0, const Frame& f1, float amount) {
                auto interpolated = std::make_unique<Frame>();
                interpolated.get()->bone_parents = f0.bone_parents;
                int n_bones = f0.bone_positions.size();
                for (int i = 0; i < n_bones; i++) {
                    interpolated.get()->bone_positions.push_back(
                        glm::lerp(f0.bone_positions[i], f1.bone_positions[i], amount)
                    );
                    interpolated.get()->bone_rotations.push_back(
                        glm::slerp(f0.bone_rotations[i], f1.bone_rotations[i], amount)
                    );
                }
                interpolated.get()->bake_transforms();
                return std::move(interpolated);
            }

            Frame current_frame() const {

            }

        private:
            Skeleton _skeleton;
            std::vector<Frame> _frames;
            mutable float _animation_time = 0;
            bool _loop{true};
    };

}

#endif // ANIMATION_HPP_INCLUDED
