#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#define VOXELIZER_IMPLEMENTATION
#include "voxelizer.h"

#define SQR(x) ((x)*(x))

// Get the vertex id of a given position
int get_vertex_id(std::vector<float>& vx, std::vector<float>& vy, std::vector<float>& vz, 
                  float x, float y, float z) {
  const float threshold = 1e-7;
  int n = vx.size();
  for(int i=0;i<n;++i) {
    float diff = SQR(vx[i]-x) + SQR(vy[i]-y) + SQR(vz[i]-z);
    if(diff < threshold) return i;
  }

  // add the new point to the array
  vx.push_back(x);
  vy.push_back(y);
  vz.push_back(z);

  return n;
}

void voxelization(const std::string& input_path, const double res, const double precision) {
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  bool ret = tinyobj::LoadObj(shapes, materials, err, input_path.c_str(), NULL);
  
  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
  
  if (!ret) {
    return exit(-1);
  }
  
  std::ofstream file("mesh_voxelized.obj");
  
  for (size_t i = 0; i < shapes.size(); i++) {
    vx_mesh_t* mesh;
    vx_mesh_t* result;
    
    mesh = vx_mesh_alloc(shapes[i].mesh.positions.size(), shapes[i].mesh.indices.size());
    
    for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
      mesh->indices[f] = shapes[i].mesh.indices[f];
    }
    for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
      mesh->vertices[v].x = shapes[i].mesh.positions[3*v+0];
      mesh->vertices[v].y = shapes[i].mesh.positions[3*v+1];
      mesh->vertices[v].z = shapes[i].mesh.positions[3*v+2];
    }
    
    result = vx_voxelize(mesh, res, res, res, precision);
    
    printf("Number of vertices: %ld\n", result->nvertices);
    printf("Number of indices: %ld\n", result->nindices);

    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<float> vz;

    // org vid -> new vid
    std::vector<int> vv(result->nvertices, -1);
    
    if (file.is_open()) {
      file << " " << "\n";
      for (int i = 0; i < result->nvertices; ++i) {
        int n = vx.size();
        int vid = get_vertex_id(vx, vy, vz, result->vertices[i].x, result->vertices[i].y, result->vertices[i].z);
        vv[i] = vid;
        // merge duplicate vertices
        if (vid != n) continue;
        file << "v " << result->vertices[i].x << " "
        <<  result->vertices[i].y << " "
        <<  result->vertices[i].z << "\n";
      }

      printf("Number of unique vertices: %ld\n", vx.size());
      
      /* vertex normal */
      // for (int i = 0; i < result->nnormals; ++i) {
      //   file << "vn " << result->normals[i].x << " "
      //   << result->normals[i].y << " "
      //   << result->normals[i].z << "\n";
      // }
      
      // for (int i = 0; i < result->nindices; i += 3) {
      //   file << "f " << result->indices[i] + 1 << "//"
      //   << result->normalindices[i] + 1;
      //   file << " ";
      //   file << result->indices[i+1] + 1 << "//"
      //   << result->normalindices[i+1] + 1;
      //   file << " ";
      //   file << result->indices[i+2] + 1 << "//"
      //   << result->normalindices[i+2] + 1 << "\n";
      // }

      for (int i = 0; i < result->nindices; i += 3) {
        file << "f " << vv[result->indices[i]] + 1;
        file << " ";
        file << vv[result->indices[i+1]] + 1;
        file << " ";
        file << vv[result->indices[i+2]] + 1 << "\n";
      }
    }
    
    vx_mesh_free(result);
    vx_mesh_free(mesh);
  }
}

int main(int argc, char** argv) {
  if(argc<4) {
    std::cerr << "Usage: "<< argv[0] << " *.obj res precision" << std::endl;
    exit(1);
  }
  
  double res = std::atof(argv[2]);
  double precision = std::atof(argv[3]);

  voxelization(std::string(argv[1]), res, precision);
}
