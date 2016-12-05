#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

void voxelization(const std::string& input_path) {
  std::string inputfile = input_path;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

  if (!err.empty()) { // `err` may contain warning message.
    std::cerr << err << std::endl;
  }

  if (!ret) {
    exit(1);
  }

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
        float vx = attrib.vertices[3*idx.vertex_index+0];
        float vy = attrib.vertices[3*idx.vertex_index+1];
        float vz = attrib.vertices[3*idx.vertex_index+2];
      }
      index_offset += fv;

      // per-face material
      shapes[s].mesh.material_ids[f];
    }
  }
}

int main(int argc, char** argv) {
  if(argc<2) {
    std::cerr << "Usage: "<< argv[0] << " *.obj" << std::endl;
    exit(1);
  }

  voxelization(std::string(argv[1]));
}
