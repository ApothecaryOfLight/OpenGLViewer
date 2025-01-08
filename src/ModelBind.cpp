#include "ModelBind.hpp"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tinygltf/tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "FilepathManager.hpp"
#include "SceneLoader.hpp"
#include "ConfigManager.hpp"

#include "ModelType.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

ModelBind::ModelBind() {
    
}

void ModelBind::bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    std::cout << "Binding mesh with name " << mesh.name << std::endl;

    // Iterate through each of the the bufferViews.
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {

        // Get a handle to the bufferview.
        const tinygltf::BufferView& bufferView = model.bufferViews[i];

        //Skip unsupported bufferViews
        if (bufferView.target == 0) {
            std::cerr << "Unsupported buffer view error!" << std::endl;
            continue;
        }

        //Get a pointer to the buffer.
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        // Log the byte offset for diagnostic purposes.
        //std::cout << "BufferView.byteOffset: " << bufferView.byteOffset << std::endl;

        // Get the VBO indice
        GLuint vbo;
        glGenBuffers(1, &vbo);
        std::cout << "Setting vbo indice at " << i << std::endl;
        vbos[i] = vbo;

        // Bind the buffer for these vertices.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);

        // Actually buffer the vertex data into the pipeline.
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            bufferView.byteLength,
            &buffer.data.at(bufferView.byteOffset),
            GL_STATIC_DRAW
        );
    }

    //Another diagnositc logging for BufferViews.
    for (const auto& [bufferViewIndex, vbo] : vbos) {
        std::cout << "VBO for BufferView " << bufferViewIndex << ": " << vbo << std::endl;
    }

    //Iterate through each of the mesh's primitives.
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        // Get a pointer to the primitive and the accessors.
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        //Iterate through the primitive's attributes.
        for (auto& attrib : primitive.attributes) {
            // Get a pointer to the accessor.
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            //Diagnostic logging for attributes.
            std::cout << "Setting attribute for: " << attrib.first
                << " with bufferView: " << accessor.bufferView
                << " type: " << accessor.componentType
                << " stride: " << byteStride << std::endl << std::endl;
            std::cout << "Accessor.byteOffset: " << accessor.byteOffset << std::endl;

            // Check for the vertex data type, set it accordingly. (more info needed here)
            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            // Get the attributes and set them in the data structure.
            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (attrib.first.compare("TANGENT") == 0) vaa = 3;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }

        // Process the textures of the mesh.
        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1) {

                GLuint texid;
                glGenTextures(1, &texid);

                tinygltf::Image& image = model.images[tex.source];

                glBindTexture(GL_TEXTURE_2D, texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1) {
                    format = GL_RED;
                }
                else if (image.component == 2) {
                    format = GL_RG;
                }
                else if (image.component == 3) {
                    format = GL_RGB;
                }
                else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                }
                else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                }
                else {
                    // ???
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                    format, type, &image.image.at(0));
            }
        }
    }
}

void ModelBind::bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node) {
    std::cout << "  Processing non-root node with index: " << &node - &model.nodes[0] << std::endl;
    std::cout << "  Node name: " << node.name << std::endl;

    // If there is a mesh in this node, bind it.
    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "    Binding mesh with index: " << node.mesh << std::endl;
        bindMesh(vbos, model, model.meshes[node.mesh]);
    }

    // If this node has children nodes, recursively hand them back to this function for processing.
    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((node.children[i] >= 0) && (node.children[i] < static_cast<int>(model.nodes.size())));
        std::cout << "    Processing child node with index: " << childIndex << std::endl;
        bindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<std::unordered_map<int, GLuint>, std::map<int, GLuint>> ModelBind::bindModel(tinygltf::Model& model) {
    std::cout << "Binding model..." << std::endl;

    // Create the objects where we'll store the model data.
    std::unordered_map<int, GLuint> vaos; // Store a VAO for each mesh/node
    std::map<int, GLuint> vbos;          // Store VBOs for buffer views

    // Get a pointer to the scene.
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    std::cout << "Model's default scene contains " << scene.nodes.size() << " root nodes." << std::endl;

    // Iterate through all the nodes in the scene.
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        // Get a pointer to the node.
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;

        // Create a VAO for this node
        GLuint vao;
        glGenVertexArrays(1, &vao);
        vaos[nodeIndex] = vao; // Store the VAO in the unordered_map

        // Bind the VAO
        glBindVertexArray(vao);

        // Bind and configure the VBOs for this node (and its children recursively)
        bindModelNodes(vbos, model, model.nodes[nodeIndex]);

        // Unbind the VAO after configuration
        glBindVertexArray(0);
    }

    // Cleanup the VBOs but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }

    // Return all the VAOs and VBOs
    return { vaos, vbos };
}


/*
std::pair<GLuint, std::map<int, GLuint>> ModelBind::bindModel(tinygltf::Model& model) {
    std::cout << "Binding model..." << std::endl;

    // Create the objects where we'll store the model data.
    std::map<int, GLuint> vbos;
    GLuint vao;

    // Bind the vertex attribute object.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Get a pointer to the scene.
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    std::cout << "Model's default scene contains " << scene.nodes.size() << " root nodes." << std::endl;

    // Iterate through all the nodes in the scene.
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        // Get a pointer to the node.
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;

        // Bind each node.
        bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    // Unbind the Vertex Array Object.
    glBindVertexArray(0);

    // Cleanup the VBOs but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }
    return { vao, vbos };
}
*/





void ModelBind::bindModelNodes_m(std::vector<std::pair<GLuint, std::map<int, GLuint>>>& vbos, tinygltf::Model& model, tinygltf::Node& node) {
    std::cout << "  Processing non-root node with index: " << (&node - &model.nodes[0]) << std::endl;
    std::cout << "  Node name: " << node.name << std::endl;

    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "    Binding mesh with index: " << node.mesh << std::endl;

        // Ensure the vector has an entry for this mesh
        if (node.mesh >= static_cast<int>(vbos.size())) {
            vbos.resize(node.mesh + 1); // Resize vector to accommodate the mesh index
        }

        // Get a reference to the pair for this mesh
        std::pair<GLuint, std::map<int, GLuint>>& vboPair = vbos[node.mesh];

        // Initialize the VAO if it hasn't been created
        if (vboPair.first == 0) {
            glGenVertexArrays(1, &vboPair.first);
        }

        // Bind the mesh using the corresponding pair
        glBindVertexArray(vboPair.first);
        bindMesh(vboPair.second, model, model.meshes[node.mesh]);
        glBindVertexArray(0); // Unbind the VAO after binding the mesh
    }

    // Process child nodes recursively
    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((node.children[i] >= 0) && (node.children[i] < static_cast<int>(model.nodes.size())));
        std::cout << "    Processing child node with index: " << childIndex << std::endl;
        bindModelNodes_m(vbos, model, model.nodes[childIndex]);
    }
}


std::vector<std::pair<GLuint, std::map<int, GLuint>>> ModelBind::bindModel_m(tinygltf::Model& model) {
    std::cout << "Binding model..." << std::endl;

    // Create a vector to hold the VAO and VBO map for each mesh
    std::vector<std::pair<GLuint, std::map<int, GLuint>>> vbos;

    // Get the default scene
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    std::cout << "Model's default scene contains " << scene.nodes.size() << " root nodes." << std::endl;

    // Process each root node
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;

        // Bind the node and its children
        tinygltf::Node& rootNode = model.nodes[scene.nodes[i]];

        // Recursively bind the model nodes for this root
        bindModelNodes_m(vbos, model, rootNode);
    }

    // Cleanup VBOs that are not GL_ELEMENT_ARRAY_BUFFER
    for (auto& pair : vbos) {
        auto& vao = pair.first;
        auto& bufferMap = pair.second;

        for (auto it = bufferMap.cbegin(); it != bufferMap.cend();) {
            tinygltf::BufferView bufferView = model.bufferViews[it->first];
            if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
                glDeleteBuffers(1, &bufferMap[it->first]);
                bufferMap.erase(it++);
            } else {
                ++it;
            }
        }
    }

    return vbos;
}













ModelData ModelBind::bindOGLModel(tinygltf::Model& model) {
    ModelData myModelData;
    myModelData.gltfModel = model;

    std::cout << "Model's default scene contains " << model.scenes[model.defaultScene].nodes.size() << " root nodes." << std::endl;

    // Bind all nodes in the default scene
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        int nodeIndex = scene.nodes[i];
        std::cout << "Processing root node with index: " << nodeIndex << std::endl;
        bindOGLModelNodes(myModelData.nodeMeshMap, model, model.nodes[nodeIndex]);
    }

    // Cleanup VBOs but do not delete index buffers yet
    for (auto it = myModelData.nodeMeshMap.begin(); it != myModelData.nodeMeshMap.end(); ++it) {
        for (auto vboIt = it->second.vbos.begin(); vboIt != it->second.vbos.end();) {
            tinygltf::BufferView bufferView = model.bufferViews[vboIt->first];
            if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
                glDeleteBuffers(1, &vboIt->second);
                vboIt = it->second.vbos.erase(vboIt);
            } else {
                ++vboIt;
            }
        }
    }

    return myModelData;
}

void ModelBind::bindOGLModelNodes(std::map<int, NodeMeshData>& nodeMeshMap, tinygltf::Model& model, tinygltf::Node& node) {
    NodeMeshData nodeMeshData;

    // Generate a VAO for this node
    glGenVertexArrays(1, &nodeMeshData.vao);
    glBindVertexArray(nodeMeshData.vao);

    std::cout << "Processing node with index: " << &node - &model.nodes[0] << std::endl;

    if ((node.mesh >= 0) && (node.mesh < static_cast<int>(model.meshes.size()))) {
        std::cout << "Binding mesh with index: " << node.mesh << std::endl;
        bindMesh(nodeMeshData.vbos, model, model.meshes[node.mesh]);
    }

    // Store the NodeMeshData in the map
    int nodeIndex = &node - &model.nodes[0];
    nodeMeshMap[nodeIndex] = nodeMeshData;

    // Traverse child nodes
    for (size_t i = 0; i < node.children.size(); i++) {
        int childIndex = node.children[i];
        assert((childIndex >= 0) && (childIndex < static_cast<int>(model.nodes.size())));
        std::cout << "Processing child node with index: " << childIndex << std::endl;
        bindOGLModelNodes(nodeMeshMap, model, model.nodes[childIndex]);
    }

    // Unbind the VAO to avoid accidental modification
    glBindVertexArray(0);
}
