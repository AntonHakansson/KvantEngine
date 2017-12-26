#include "core/Loader.hpp"

namespace Kvant {

    static std::vector<std::string> process_material_texture(aiMaterial* mat, aiTextureType type, std::string typeName) {
        std::vector<std::string> textures;
        
        for (unsigned int t = 0; t < mat->GetTextureCount(type); t++) {
            aiString str;
            mat->GetTexture(type, t, &str);
            textures.push_back(str.C_Str());
            LOG_DEBUG << str.C_Str() << " " << type << ", " << aiTextureType_SPECULAR;
        }
        return textures;
    }

    static void process_mesh(RawModelData* model_data, const aiScene* scene, aiMesh* mesh) {
        auto mesh_cpy = std::make_unique<Mesh>();
        std::unordered_map<Kvant::Vertex, int> vs;
        unsigned int vi = 0;
        
        // Store Vertex data
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vert;
            vert.position = { mesh->mVertices[i].x,
                              mesh->mVertices[i].y,
                              mesh->mVertices[i].z };
            vert.normal   = { mesh->mNormals[i].x,
                              mesh->mNormals[i].y,
                              mesh->mNormals[i].z };

            if (mesh->mTextureCoords[0]) {
                vert.uvs = { mesh->mTextureCoords[0][i].x,
                             mesh->mTextureCoords[0][i].y };
            }
            else {
                vert.uvs = {0.0f, 0.0f};
            }

            mesh_cpy->vertices.push_back(vert);
        }
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int m = 0; m < mesh->mNumFaces; m++) {
            auto face = mesh->mFaces[m];
            // Retrieve all indices of the face and store them in the indices vector
            for (unsigned int i = 0; i < face.mNumIndices; i++) {
                mesh_cpy->triangles.push_back(face.mIndices[i]);
            }
        }
        // push our mesh to the model
        model_data->model.meshes.push_back(std::move(mesh_cpy));

        // Try read materials
        if (mesh->mMaterialIndex < 0) return; // No material exists
        auto* material = scene->mMaterials[mesh->mMaterialIndex];
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // Diffuse: texture_diffuseN
        // Specular: texture_specularN
        // Normal: texture_normalN

        // diffuse
        model_data->diffuses = process_material_texture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // normal
        model_data->normals = process_material_texture(material, aiTextureType_NORMALS, "texture_normal");
        // specular
        model_data->speculars = process_material_texture(material, aiTextureType_SPECULAR, "texture_specular");
    }

    static void process_node(RawModelData* model_data, const aiScene* scene, aiNode* node) {
        // Process each mesh located at the current node
        LOG_DEBUG << "Processing node " << node->mName.C_Str() << " with meshes = " << node->mNumMeshes << " children = " << node->mNumChildren;
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            process_mesh(model_data, scene, mesh);
        }

        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node(model_data, scene, node->mChildren[i]);
        }
    }
    RawModelData read_model(const char* filename) {
        RawModelData model_data;

        using namespace Assimp;

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene) {
            throw std::runtime_error("Failed to load model " + std::string(filename));
        }

        process_node(&model_data, scene, scene->mRootNode);

        model_data.model.generate_tangents();
        return model_data;
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
