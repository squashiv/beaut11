#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include <iostream>
#include <unordered_map>
#include "mesh.h"
#include "tiny_obj_loader.h"

class MeshLoader {
 public:
  static bool load_obj(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<VertexIndex>& indices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file_path.c_str());

    if (!warn.empty()) {
      std::cout << warn << std::endl;
    }

    if (!err.empty()) {
      std::cerr << err << std::endl;
    }

    if (!ret) {
      return false;
    }

    std::unordered_map<size_t, VertexIndex> unique_vertices;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = shapes[s].mesh.num_face_vertices[f];
        for (int v = 0; v < fv; v++) {
          // Access to vertex
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

          // Generate a unique key for each vertex (position, normal, uv)
          size_t vertex_key = generate_vertex_key(attrib, idx);

          // Check if the vertex is unique
          if (unique_vertices.find(vertex_key) == unique_vertices.end()) {
            // Add the vertex if it's unique
            Vertex new_vertex(
                glm::vec3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1],
                          attrib.vertices[3 * idx.vertex_index + 2]),
                glm::vec3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1],
                          attrib.normals[3 * idx.normal_index + 2]),
                glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]),
                glm::vec3(1.0f, 1.0f, 1.0f)  // You can change the color as needed
            );
            vertices.push_back(new_vertex);
            unique_vertices[vertex_key] = vertices.size() - 1;
          }

          // Add the index of the unique vertex
          indices.push_back(unique_vertices[vertex_key]);
        }
        index_offset += fv;
      }
    }

    return true;
  }

 private:
  static size_t generate_vertex_key(const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx) {
    size_t h1 = std::hash<float>{}(attrib.vertices[3 * idx.vertex_index + 0]);
    size_t h2 = std::hash<float>{}(attrib.vertices[3 * idx.vertex_index + 1]);
    size_t h3 = std::hash<float>{}(attrib.vertices[3 * idx.vertex_index + 2]);
    size_t h4 = std::hash<float>{}(attrib.normals[3 * idx.normal_index + 0]);
    size_t h5 = std::hash<float>{}(attrib.normals[3 * idx.normal_index + 1]);
    size_t h6 = std::hash<float>{}(attrib.normals[3 * idx.normal_index + 2]);
    size_t h7 = std::hash<float>{}(attrib.texcoords[2 * idx.texcoord_index + 0]);
    size_t h8 = std::hash<float>{}(attrib.texcoords[2 * idx.texcoord_index + 1]);

    return h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7 ^ h8;
  }
};
