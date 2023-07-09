#pragma once

#include <cstdint>
#include <vector>

struct InstanceInfo
{
  float worldViewMatrix[16]; ///< assume row-major layout, i.e. M[0], M[1], M[2], M[3] is the first row of the matrix
  float projMatrix[16];      ///< assume row-major layout, i.e. M[0], M[1], M[2], M[3] is the first row of the matrix
};

struct GeomStorage
{
  std::vector<float>    vpos4f;
  std::vector<float>    vcol4f;
  std::vector<float>    vtex2f;
  std::vector<uint32_t> indices;
};

struct SimpleScene
{
  GeomStorage               geom;
  std::vector<InstanceInfo> instances;
  std::vector<uint32_t>     textures;
};

std::vector<SimpleScene> scn01_colored_triangle();
std::vector<SimpleScene> scn02_tri_and_quad();
std::vector<SimpleScene> scn03_pyr_and_cube();
std::vector<SimpleScene> scn04_cube(uint32_t a_texId);
std::vector<SimpleScene> scn05_cubes_many(uint32_t a_texId1, uint32_t a_texId2, uint32_t a_texId3);
std::vector<SimpleScene> scn06_teapot(uint32_t a_texId1, uint32_t a_texId2);
std::vector<SimpleScene> scn07_teapots_many(uint32_t a_texId1, uint32_t a_texId2);
std::vector<SimpleScene> scn08_terrain(uint32_t a_texId1);

GeomStorage make_plane();
GeomStorage make_cube();
GeomStorage make_pyramid();
GeomStorage make_teapot();
GeomStorage make_terrain();
