#include "core/Loader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "utils/tiny_obj_loader.hpp"

namespace Kvant {

    std::unique_ptr<Kvant::Model> read_obj(const char* filename) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;

        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, NULL, &err, filename);

        if (!err.empty()) {
            if (ret) LOG_WARNING << err;
            else     throw std::runtime_error(err);
        }

        auto model = std::make_unique<Kvant::Model>();
        auto mesh = std::make_unique<Kvant::Mesh>();
        std::unordered_map<Kvant::Vertex, int> vs;
        int vi = 0;
        
        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                int fv = shapes[s].mesh.num_face_vertices[f];
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                    tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

                    // TODO: implement vertex color
                    /*tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                    tinyobj::real_t alpha = attrib.colors[3*idx.vertex_index+3];*/

                    Vertex vert;
                    vert.position = {vx, vy, vz};
                    vert.normal = {nx, ny, nz};
                    vert.uvs = {tx, ty};

                    // Try find this vertex
                    if (vs.find(vert) == vs.end()) {
                        // if it hasn't been added, add it
                        vs[vert] = vi;
                        mesh->vertices.push_back(vert);
                        mesh->triangles.push_back(vi);
                        vi++;
                    }
                    else {
                        // If it's already been seen, get it's indice
                        mesh->triangles.push_back(vs[vert]);
                    }

                    // vert.color = {red, green, blue, alpha};
                }
                index_offset += fv;

                // per-face material
                shapes[s].mesh.material_ids[f];
            }
        }

        LOG_DEBUG << "teached me";
        model->meshes.push_back(std::move(mesh));
        model->generate_tangents();
        return std::move(model);
    }


    class token_reader {
        public:
            virtual void read(std::ifstream& f) = 0;
    };

    class version_reader : public token_reader {
        public:
            void read(std::ifstream& f) override {
                std::uint32_t version;
                f >> version;
            }
    };

    class nodes_reader : public token_reader {
        public:
            std::vector<int> parents;
            Skeleton _skeleton;
            void read(std::ifstream& f) override {
                std::string ids, name;
                int id, id2;
                f >> ids;
                while (ids != "end") {
                    id = std::stoi(ids);
                    f >> name >> id2;

                    parents.push_back(id2);
                    _skeleton.bones.push_back(Bone{name, id2});
                    f >> ids;
                }
            }
    };

    class skeleton_reader : public token_reader {
        public:
            std::vector<Frame> _frames;
            nodes_reader* _nr;
            skeleton_reader(nodes_reader* nr) : _nr(nr) {}

            void read(std::ifstream& f) override {
                std::string time_token, ids;
                int time, id;
                float x, y, z, xx, yy, zz;
                f >> ids;
                Frame fr;
                while (ids != "end") {
                    if (ids == "time") {
                        f >> time;
                        if (fr.bone_positions.size() > 0) {
                            fr.bake_transforms();
                            _frames.push_back(fr);
                        }
                        fr.bone_parents.clear();
                        fr.bone_positions.clear();
                        fr.bone_rotations.clear();
                        fr.bone_transforms.clear();
                        fr.bone_inv_transforms.clear();
                    } else {
                        id = std::stoi(ids);
                        fr.bone_parents.push_back(_nr->parents[id]);
                        f >> x >> y >> z >> xx >> yy >> zz;
                        fr.bone_positions.push_back(glm::vec3(x, y, z));
                        fr.bone_rotations.push_back(glm::eulerAngleXYZ(xx, yy, zz));
                    }
                    f >> ids;
                    if (ids == "end") {
                    fr.bake_transforms();
                       _frames.push_back(fr);
                    }
                }
            }
    };

    class triangles_reader : public token_reader {
        public:
            std::unordered_map<Kvant::Vertex, int> vs;
            std::vector<uint32_t> ts;
            std::vector<Kvant::Vertex> vsl;
            std::vector<Kvant::Vertex_weights> vwl;
            int vi = 0;

            void read(std::ifstream& f) override {
                std::string material;
                int root, nlinks;
                float x, y, z, nx, ny, nz, u, v;
                f >> material;
                while (material != "end") {
                    for (int j = 0; j < 3; j++) {
                        Kvant::Vertex vx;
                        Kvant::Vertex_weights vw;
                        f >> root >> x >> y >> z >> nx >> ny >> nz >> u >> v >> nlinks;
                        if (nlinks == 0) {
                            LOG_WARNING << "Mesh has vertex with no bones, material is: " << material;
                        }
                        vx.position = {x, y, z};
                        vx.normal = {nx, ny, nz};
                        vx.uvs = {u, v};
                        float total = 0;
                        float excess = 0.0;
                        for (int wi = 0; wi < nlinks; wi++) {
                            int id;
                            float weight;
                            f >> id >> weight;
                            if (wi <= 2) {
                                vw.bone_ids[wi] = id;
                                vw.bone_weights[wi] = weight;
                                total += weight;
                            } else {
                                excess += weight;
                            }
                        }
                        if (excess > 0.0) {
                            vw.bone_weights[0] += excess * (vw.bone_weights[0] / total);
                            vw.bone_weights[1] += excess * (vw.bone_weights[1] / total);
                            vw.bone_weights[2] += excess * (vw.bone_weights[2] / total);
                            total = vw.bone_weights[0] + vw.bone_weights[1] + vw.bone_weights[2];
                        }
                        if (total > 1.00001 || total < 0.99999) {
                            LOG_ERROR << "Total : " << total;
                        }
                        if (vs.find(vx) == vs.end()) {
                            vsl.push_back(vx);
                            vwl.push_back(vw);
                            ts.push_back(vi);
                            vs[vx] = vi;
                            vi++;
                        }
                        else {
                            ts.push_back(vs[vx]);
                        }
                    }
                    f >> material;
                }
            }
    };

    std::unique_ptr<Kvant::Model> read_smd(const char* filename) {
        LOG_DEBUG << "Reading Model from: " << filename;
        std::map<std::string, token_reader*> readers;

        auto vr = std::make_unique<version_reader>();
        auto nr = std::make_unique<nodes_reader>();
        auto sr = std::make_unique<skeleton_reader>(nr.get());
        auto tr = std::make_unique<triangles_reader>();

        readers["version"] = vr.get();
        readers["nodes"] = nr.get();
        readers["skeleton"] = sr.get();
        readers["triangles"] = tr.get();

        std::ifstream f(filename, std::ios::in);
        if (!f) throw std::runtime_error("File not found");

        std::string token;

        f >> token;
        while (f.eof() == false) {
            if (readers.find(token) == readers.end()) throw std::runtime_error("Error reading SMD, token " + token + " not found");
            readers[token]->read(f);
            f >> token;
        }
        std::unique_ptr<Kvant::Model> Model = std::make_unique<Kvant::Model>();
        std::unique_ptr<Kvant::Mesh> Mesh = std::make_unique<Kvant::Mesh>();

        Mesh->vertices = tr->vsl;
        Mesh->triangles = tr->ts;
        if (nr->_skeleton.bones.size()==1 && nr->_skeleton.bones[0].name=="\"root\"")
            Mesh->is_rigged = false;
        else {
            Mesh->is_rigged = true;
            throw std::runtime_error("rigged model not supported yet");
        }
        Mesh->weights = tr->vwl;
        Model->meshes.push_back(std::move(Mesh));
        Model->generate_tangents();
        return std::move(Model);
    }

    std::vector<Frame> read_animation(const char* filename) {
        LOG_DEBUG << "Reading animation file: " << filename;
        std::map<std::string, token_reader*> readers;

        auto vr = std::make_unique<version_reader>();
        auto nr = std::make_unique<nodes_reader>();
        auto sr = std::make_unique<skeleton_reader>(nr.get());

        readers["version"] = vr.get();
        readers["nodes"] = nr.get();
        readers["skeleton"] = sr.get();

        std::ifstream f(filename, std::ios::in);
        if (!f) throw std::runtime_error("File not found");

        std::string token;

        f >> token;
        while (f.eof() == false) {
            if (readers.find(token) == readers.end()) throw std::runtime_error("Error reading SMD, token " + token + " not found");
            readers[token]->read(f);
            f >> token;
        }

        return sr->_frames;
    }

    bool load_png_for_texture(unsigned char** img, unsigned int* width,
                              unsigned int* height, const char* filename) {
        bool success = false;
        unsigned error = lodepng_decode32_file(img, width, height, filename);
        if (error) {
            std::runtime_error("Error loading image");
        } else {
            // Flip and invert the image
            unsigned char* imagePtr = *img;
            int halfTheHeightInPixels = *height / 2;
            int heightInPixels = *height;
            // Assume RGBA for 4 components per pixel
            int numColorComponents = 4;
            // Assuming each color component is an unsigned char
            int widthInChars = *width * numColorComponents;
            unsigned char* top = NULL;
            unsigned char* bottom = NULL;
            unsigned char temp = 0;
            for (int h = 0; h < halfTheHeightInPixels; ++h) {
                top = imagePtr + h * widthInChars;
                bottom = imagePtr + (heightInPixels - h - 1) * widthInChars;
                for (int w = 0; w < widthInChars; ++w) {
                    // Swap the chars around.
                    temp = *top;
                    *top = *bottom;
                    *bottom = temp;
                    ++top;
                    ++bottom;
                }
            }
            success = true;
        }

        return success;
    }

}
