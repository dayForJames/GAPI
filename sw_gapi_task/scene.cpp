#include "scene.h"
#include "Bitmap.h"
#include <cstring>
#include <fstream>


std::vector<SimpleScene> scn01_colored_triangle()
{
  SimpleScene res;
  res.instances.resize(1);

  float identity[16] ={1, 0, 0, 0, 
                       0, 1, 0, 0, 
                       0, 0, 1, 0, 
                       0, 0, 0, 1 };

  memcpy(res.instances[0].worldViewMatrix, identity, sizeof(identity));
  memcpy(res.instances[0].projMatrix,      identity, sizeof(identity));

  res.geom.vpos4f = {-0.5,-0.5,0,1, 
                     0,0.5,0,1, 
                     0.5,-0.5,0,1};
  
  res.geom.vcol4f = {1,0,0,1, 
                     0,1,0,1, 
                     0,0,1,1};

  res.geom.vtex2f  = {0,0, 0,0, 0,0};
  res.geom.indices = {0, 1, 2};

  //std::vector<SimpleScene> res_v = {res};
  //res_v[0] = res;
  return {res};
}

std::vector<SimpleScene> scn02_tri_and_quad()
{
  std::vector<SimpleScene> res;
  res.resize(2);

  float identity[16] = {1, 0, 0, 0, 
                        0, 1, 0, 0, 
                        0, 0, 1, 0, 
                        0, 0, 0, 1 };

  float trans1[16] = {0.5, 0, 0, -0.5, 
                      0, 0.5, 0, +0.25, 
                      0, 0, 1, 0, 
                      0, 0, 0, 1};

  float trans2[16] = {1, 0, 0, +0.5, 
                      0, 1, 0, -0.25, 
                      0, 0, 1, 0, 
                      0, 0, 0, 1};                     

  float trans3[16] = {0.5, 0, 0, -0.5, 
                      0, 0.5, 0, -0.25, 
                      0, 0, 1, 0, 
                      0, 0, 0, 1};

  float trans4[16] = {0.5, 0, 0, -0.5, 
                      0, 0.5, 0, -0.75, 
                      0, 0, 1, 0, 
                      0, 0, 0, 1};
  
  float trans5[16] = {0.5, 0, 0, +0.5, 
                      0, 0.5, 0, +0.5, 
                      0, 0, 1, 0, 
                      0, 0, 0, 1};            

  // triangle
  //
  res[0].instances.resize(3);
  {
    memcpy(res[0].instances[0].worldViewMatrix, trans1,   sizeof(trans1));
    memcpy(res[0].instances[0].projMatrix,      identity, sizeof(identity));
  
    memcpy(res[0].instances[1].worldViewMatrix, trans3,   sizeof(trans1));
    memcpy(res[0].instances[1].projMatrix,      identity, sizeof(identity));

    memcpy(res[0].instances[2].worldViewMatrix, trans4,   sizeof(trans1));
    memcpy(res[0].instances[2].projMatrix,      identity, sizeof(identity));
  }
  
  res[0].geom.vpos4f  = {0,0.4,0,1, -0.4,-0.4,0,1, 0.4,-0.4,0,1};
  res[0].geom.vcol4f  = {1,1,1,1, 1,1,1,1, 1,1,1,1};
  res[0].geom.vtex2f  = {0,0, 0,0, 0,0};
  res[0].geom.indices = {0, 1, 2};
  
  // quad
  //
  res[1].instances.resize(2);
  {
    memcpy(res[1].instances[0].worldViewMatrix, trans2,   sizeof(trans2));
    memcpy(res[1].instances[0].projMatrix,      identity, sizeof(identity));

    memcpy(res[1].instances[1].worldViewMatrix, trans5,   sizeof(trans2));
    memcpy(res[1].instances[1].projMatrix,      identity, sizeof(identity));
  }

  res[1].geom.vpos4f  = {-0.4, 0.4, 0, 1, 
                          0.4, 0.4, 0, 1, 
                          0.4,-0.4, 0, 1, 
                         -0.4,-0.4, 0, 1};
                    
  res[1].geom.vcol4f  = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
  res[1].geom.vtex2f  = {0,0, 0,0, 0,0, 0,0};
  res[1].geom.indices = {0, 1, 2, 0, 2, 3};

  return res;
}

GeomStorage make_plane()
{
  GeomStorage geom;
  geom.vpos4f  = {-1,-1,1,1, 1,-1,1,1, 1,-1,-1,1, -1,-1,-1,1};
  geom.vcol4f  = {1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0};
  geom.vtex2f  = {1,0, 0,0, 0,1, 1,1};
  geom.indices = {0, 1, 2, 0, 2, 3};
  return geom;
}

GeomStorage make_pyramid()
{
  GeomStorage geom;
  geom.vpos4f  = {0,1,0,1, -1,-1,1,1, 1,-1,1,1, 0,1,0,1, 1,-1,1,1, 1,-1,-1,1, 0,1,0,1, 1,-1,-1,1, -1,-1,-1,1, 0,1,0,1, -1,-1,-1,1, -1,-1,1,1};
  geom.vcol4f  = {1,0,0,0, 0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,1,0, 0,1,0,0, 1,0,0,0, 0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,1,0, 0,1,0,0};
  geom.vtex2f  = {0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0};
  geom.indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return geom;
}

GeomStorage make_cube()
{
  GeomStorage geom;
  geom.vpos4f  = {-1,-1,1,1, 1,-1,1,1, 1,1,1,1, -1,1,1,1, -1,-1,-1,1, -1,1,-1,1, 1,1,-1,1, 1,-1,-1,1, -1,1,-1,1, -1,1,1,1, 1,1,1,1, 1,1,-1,1, -1,-1,-1,1, 1,-1,-1,1, 1,-1,1,1, -1,-1,1,1, 1,-1,-1,1, 1,1,-1,1, 1,1,1,1, 1,-1,1,1, -1,-1,-1,1, -1,-1,1,1, -1,1,1,1, -1,1,-1,1};
  geom.vcol4f  = {1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0, 1,1,1,0};
  geom.vtex2f  = {0,0, 1,0, 1,1, 0,1, 1,0, 1,1, 0,1, 0,0, 0,1, 0,0, 1,0, 1,1, 1,1, 0,1, 0,0, 1,0, 1,0, 1,1, 0,1, 0,0, 0,0, 1,0, 1,1, 0,1};
  geom.indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};
  return geom;
}

std::vector<SimpleScene> scn03_pyr_and_cube()
{
  std::vector<SimpleScene> res;
  res.resize(2);
  
  // pyramid
  //
  float proj1[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float worldView1[16] = {0.866025, 0, 0.5, -1.5, 
                          0, 1, 0, 0, 
                          -0.5, 0, 0.866025, -6, 
                          0, 0, 0, 1};
  
  res[0].instances.resize(1);
  memcpy(res[0].instances[0].worldViewMatrix, worldView1, sizeof(worldView1));
  memcpy(res[0].instances[0].projMatrix,      proj1, sizeof(proj1));
  
  res[0].geom = make_pyramid();

  // cube
  //
  float proj2[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float worldView2[16] = {0.761858, -0.323205, 0.561347, 1.5, 
                          0.561347, 0.761858, -0.323205, 0, 
                          -0.323205, 0.561347, 0.761858, -7, 
                          0, 0, 0, 1 };
  

  res[1].instances.resize(1);
  memcpy(res[1].instances[0].worldViewMatrix, worldView2, sizeof(worldView2));
  memcpy(res[1].instances[0].projMatrix,      proj2, sizeof(proj2));
  
  // Note: don't use make_cube() here because we want to paint cube faces in different colors
  //
  res[1].geom.vpos4f  = {1,1,-1,1, -1,1,-1,1, -1,1,1,1, 1,1,1,1, 1,-1,1,1, -1,-1,1,1, -1,-1,-1,1, 1,-1,-1,1, 1,1,1,1, -1,1,1,1, -1,-1,1,1, 1,-1,1,1, 1,-1,-1,1, -1,-1,-1,1, -1,1,-1,1, 1,1,-1,1, -1,1,1,1, -1,1,-1,1, -1,-1,-1,1, -1,-1,1,1, 1,1,-1,1, 1,1,1,1, 1,-1,1,1, 1,-1,-1,1};
  res[1].geom.vcol4f  = {0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0};
  res[1].geom.vtex2f  = {0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0};
  res[1].geom.indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

  return res;
}


std::vector<SimpleScene> scn04_cube(uint32_t a_texId)
{
  std::vector<SimpleScene> res;
  if (a_texId == uint32_t(-1)) {
    res.resize(2);
  }
  else {
    res.resize(1);
  }
  
  // pyramid
  //
  float proj1[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float worldView1[16] = {0.884825, 0.412601, 0.21644, 0, 
                          -0.315381, 0.872324, -0.373612, 0, 
                          -0.342958, 0.262321, 0.90198, -5, 
                          0, 0, 0, 1 };
  
  res[0].instances.resize(1);
  memcpy(res[0].instances[0].worldViewMatrix, worldView1, sizeof(worldView1));
  memcpy(res[0].instances[0].projMatrix,      proj1,      sizeof(proj1));

  if (a_texId == uint32_t(-1)) {
    res[0].geom.vpos4f  = {1,1,-1,1, -1,1,-1,1, -1,1,1,1, 1,1,1,1, 1,-1,1,1, -1,-1,1,1, -1,-1,-1,1, 1,-1,-1,1, 1,1,1,1, -1,1,1,1, -1,-1,1,1, 1,-1,1,1, 1,-1,-1,1, -1,-1,-1,1, -1,1,-1,1, 1,1,-1,1, -1,1,1,1, -1,1,-1,1, -1,-1,-1,1, -1,-1,1,1, 1,1,-1,1, 1,1,1,1, 1,-1,1,1, 1,-1,-1,1};
    res[0].geom.vcol4f  = {0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0};
    res[0].geom.vtex2f  = {0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0};
    res[0].geom.indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

    for (int i = 0; i < res[0].geom.vpos4f.size(); i += 4) {
      res[0].geom.vpos4f[i] = res[0].geom.vpos4f[i] / 5 + 1;
      res[0].geom.vpos4f[i + 1] = res[0].geom.vpos4f[i + 1] / 5 + 1.7;
      res[0].geom.vpos4f[i + 2] = res[0].geom.vpos4f[i + 2] / 5 + 0;
    }

    res[1].instances.resize(1);
    memcpy(res[1].instances[0].worldViewMatrix, worldView1, sizeof(worldView1));
    memcpy(res[1].instances[0].projMatrix,      proj1,      sizeof(proj1));
    res[1].geom.vpos4f  = {1,1,-1,1, -1,1,-1,1, -1,1,1,1, 1,1,1,1, 1,-1,1,1, -1,-1,1,1, -1,-1,-1,1, 1,-1,-1,1, 1,1,1,1, -1,1,1,1, -1,-1,1,1, 1,-1,1,1, 1,-1,-1,1, -1,-1,-1,1, -1,1,-1,1, 1,1,-1,1, -1,1,1,1, -1,1,-1,1, -1,-1,-1,1, -1,-1,1,1, 1,1,-1,1, 1,1,1,1, 1,-1,1,1, 1,-1,-1,1};
    res[1].geom.vcol4f  = {0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0.5,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 1,1,0,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0};
    res[1].geom.vtex2f  = {0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0};
    res[1].geom.indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};
  }
  else {
    res[0].geom     = make_cube();
    res[0].textures = {a_texId};
  }
  
  return res;
}

std::vector<SimpleScene> scn05_cubes_many(uint32_t a_texId1, uint32_t a_texId2, uint32_t a_texId3)
{
  float projM[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };
  
  float pyr1World[16] = {1.35946, 0, 0.633927, 0, 
                         0, 1.5, 0, 6.5, 
                         -0.633927, 0, 1.35946, -20, 
                         0, 0, 0, 1 };

  float pyr2World[16] = {0.906308, 0, 0.422618, -7.97356, 
                         0, 1, 0, -1, 
                         -0.422618, 0, 0.906308, -22.9021, 
                         0, 0, 0, 1 };

  float pyr3World[16] = {0.906308, 0, 0.422618, -2.90214, 
                         0, 1, 0, -1, 
                         -0.422618, 0, 0.906308, -12.0264, 
                         0, 0, 0, 1 };

  float pyr4World[16] = {0.906308, 0, 0.422618, 2.90214, 
                         0, 1, 0, -1, 
                         -0.422618, 0, 0.906308, -27.9736, 
                         0, 0, 0, 1 };

  float pyr5World[16] = {0.906308, 0, 0.422618, 7.97356, 
                         0, 1, 0, -1, 
                         -0.422618, 0, 0.906308, -17.0979, 
                         0, 0, 0, 1 };

  std::vector<SimpleScene> res;
  res.resize(3);

  res[0].geom = make_pyramid();
  res[0].instances.resize(5);

  memcpy(res[0].instances[0].worldViewMatrix, pyr1World,  sizeof(pyr1World));
  memcpy(res[0].instances[0].projMatrix,      projM,      sizeof(projM));

  memcpy(res[0].instances[1].worldViewMatrix, pyr2World,  sizeof(pyr2World));
  memcpy(res[0].instances[1].projMatrix,      projM,      sizeof(projM));

  memcpy(res[0].instances[2].worldViewMatrix, pyr3World,  sizeof(pyr3World));
  memcpy(res[0].instances[2].projMatrix,      projM,      sizeof(projM));

  memcpy(res[0].instances[3].worldViewMatrix, pyr4World,  sizeof(pyr4World));
  memcpy(res[0].instances[3].projMatrix,      projM,      sizeof(projM));

  memcpy(res[0].instances[4].worldViewMatrix, pyr5World,  sizeof(pyr5World));
  memcpy(res[0].instances[4].projMatrix,      projM,      sizeof(projM));

  res[1].geom = make_plane();
  res[1].textures = {a_texId1};
  res[1].instances.resize(1);

  float planeWorld[16] = {9.06308, 0, 4.22618, 0, 
                          0, 10, 0, 6, 
                          -4.22618, 0, 9.06308, -20, 
                          0, 0, 0, 1};

  memcpy(res[1].instances[0].worldViewMatrix, planeWorld, sizeof(planeWorld));
  memcpy(res[1].instances[0].projMatrix,      projM,      sizeof(projM));

  res[2].geom     = make_cube();
  res[2].textures = {a_texId3, a_texId2, a_texId2, a_texId2, a_texId2, a_texId2};

  float cube0World[16] = {2.71892, 0, 1.26785, 0, 
                          0, 3, 0, -1, 
                          -1.26785, 0, 2.71892, -20, 
                          0, 0, 0, 1 };

  float cube1World[16] = {1.35946, 0, 0.633927, 0, 
                          0, 1.5, 0, 3.5, 
                          -0.633927, 0, 1.35946, -20, 
                          0, 0, 0, 1};

  float cube2World[16] = {0.906308, 0, 0.422618, -7.97356, 
                          0, 1, 0, -3, 
                          -0.422618, 0, 0.906308, -22.9021, 
                          0, 0, 0, 1 };

  float cube3World[16] = {0.906308, 0, 0.422618, -2.90214, 
                          0, 1, 0, -3, 
                          -0.422618, 0, 0.906308, -12.0264, 
                          0, 0, 0, 1 };

  float cube4World[16] = {0.906308, 0, 0.422618, 2.90214, 
                          0, 1, 0, -3, 
                          -0.422618, 0, 0.906308, -27.9736, 
                          0, 0, 0, 1 };

  float cube5World[16] = {0.906308, 0, 0.422618, 7.97356, 
                          0, 1, 0, -3, 
                          -0.422618, 0, 0.906308, -17.0979, 
                          0, 0, 0, 1 };

  res[2].instances.resize(6);

  memcpy(res[2].instances[0].worldViewMatrix, cube0World, sizeof(cube0World));
  memcpy(res[2].instances[0].projMatrix,      projM,      sizeof(projM));

  memcpy(res[2].instances[1].worldViewMatrix, cube1World, sizeof(cube1World));
  memcpy(res[2].instances[1].projMatrix,      projM,      sizeof(projM));

  memcpy(res[2].instances[2].worldViewMatrix, cube2World, sizeof(cube2World));
  memcpy(res[2].instances[2].projMatrix,      projM,      sizeof(projM));

  memcpy(res[2].instances[3].worldViewMatrix, cube3World, sizeof(cube3World));
  memcpy(res[2].instances[3].projMatrix,      projM,      sizeof(projM));

  memcpy(res[2].instances[4].worldViewMatrix, cube4World, sizeof(cube4World));
  memcpy(res[2].instances[4].projMatrix,      projM,      sizeof(projM));

  memcpy(res[2].instances[5].worldViewMatrix, cube5World, sizeof(cube5World));
  memcpy(res[2].instances[5].projMatrix,      projM,      sizeof(projM));

  return res;
}

GeomStorage make_teapot()
{
  GeomStorage geom;
  geom.vpos4f.resize(12288*4);  
  geom.vcol4f.resize(12288*4);  
  geom.vtex2f.resize(12288*2);  
  geom.indices.resize(12288);

  std::ifstream fin;
  fin.open("data/chunk_1.bin", std::ios::binary);
  fin.read((char*)geom.vpos4f.data(), geom.vpos4f.size() * sizeof(float));
  fin.close();

  fin.open("data/chunk_2.bin", std::ios::binary);
  fin.read((char*)geom.vcol4f.data(), geom.vcol4f.size() * sizeof(float));
  fin.close();

  fin.open("data/chunk_3.bin", std::ios::binary);
  fin.read((char*)geom.vtex2f.data(), geom.vtex2f.size() * sizeof(float));
  fin.close();

  fin.open("data/chunk_4.bin", std::ios::binary);
  fin.read((char*)geom.indices.data(), geom.indices.size() * sizeof(unsigned));
  fin.close();

  return geom;
}


std::vector<SimpleScene> scn06_teapot(uint32_t a_texId1, uint32_t a_texId2)
{
  float projM[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float planeWorld[16] = {3, 0, 0, 0, 
                          0, 2.81908, -1.02606, 2.31908, 
                          0, 1.02606, 2.81908, -3.97394, 
                          0, 0, 0, 1};
  
  float teapotWorld[16] = {1, 0, 0, 0, 
                           0, 0.939693, -0.34202, -0.5, 
                           0, 0.34202, 0.939693, -5, 
                           0, 0, 0, 1 };

  std::vector<SimpleScene> res;
  res.resize(2);
  
  res[0].geom     = make_plane();
  res[0].textures = {a_texId1};
  res[0].instances.resize(1);

  memcpy(res[0].instances[0].worldViewMatrix, planeWorld, sizeof(planeWorld));
  memcpy(res[0].instances[0].projMatrix,      projM,      sizeof(projM));

  res[1].geom     = make_teapot();
  res[1].textures = {a_texId2};
  res[1].instances.resize(1);

  memcpy(res[1].instances[0].worldViewMatrix, teapotWorld, sizeof(teapotWorld));
  memcpy(res[1].instances[0].projMatrix,      projM,       sizeof(projM));

  return res;
}

std::vector<SimpleScene> scn07_teapots_many(uint32_t a_texId1, uint32_t a_texId2)
{
  float projM[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float planeWorld[16] = {-2.59808, 0, 1.5, 0, 
                          0.51303, 2.81908, 0.888594, 3.31908, 
                          -1.40954, 1.02606, -2.44139, -3.97394, 
                          0, 0, 0, 1};
  
  float teapotWorld0[16] = {-0.433013, 0, 0.25, 0.732051, 
                            0.085505, 0.469846, 0.148099, -0.434416, 
                            -0.234923, 0.17101, -0.406899, -2.43271, 
                            0, 0, 0, 1 };

  float teapotWorld1[16] = {-0.433013, 0, 0.25, 1.73205, 
                             0.085505, 0.469846, 0.148099, 0.15798, 
                             -0.234923, 0.17101, -0.406899, -4.06031, 
                             0, 0, 0, 1};
  
  float teapotWorld2[16] = {-0.433013, 0, 0.25, 2.73205, 
                             0.085505, 0.469846, 0.148099, 0.750376, 
                             -0.234923, 0.17101, -0.406899, -5.6879, 
                             0, 0, 0, 1 };
  
  float teapotWorld3[16] = {-0.433013, 0, 0.25, -1, 
                            0.085505, 0.469846, 0.148099, -0.0923962, 
                            -0.234923, 0.17101, -0.406899, -3.3724, 
                            0, 0, 0, 1 };

  float teapotWorld4[16] = {-0.433013, 0, 0.25, 0, 
                             0.085505, 0.469846, 0.148099, 0.5, 
                             -0.234923, 0.17101, -0.406899, -5, 
                             0, 0, 0, 1};

  float teapotWorld5[16] = {-0.433013, 0, 0.25, 1, 
                             0.085505, 0.469846, 0.148099, 1.0924, 
                             -0.234923, 0.17101, -0.406899, -6.62759, 
                             0, 0, 0, 1 };

  float teapotWorld6[16] = {-0.433013, 0, 0.25, -2.73205, 
                            0.085505, 0.469846, 0.148099, 0.249624, 
                            -0.234923, 0.17101, -0.406899, -4.3121, 
                            0, 0, 0, 1 };
 
  float teapotWorld7[16] = {-0.433013, 0, 0.25, -1.73205, 
                            0.085505, 0.469846, 0.148099, 0.84202, 
                            -0.234923, 0.17101, -0.406899, -5.93969, 
                            0, 0, 0, 1 };

  float teapotWorld8[16] = {-0.433013, 0, 0.25, -0.732051, 
                            0.085505, 0.469846, 0.148099, 1.43442, 
                            -0.234923, 0.17101, -0.406899, -7.56729, 
                            0, 0, 0, 1 };

  std::vector<SimpleScene> res;
  res.resize(2);
  
  res[0].geom     = make_plane();
  res[0].textures = {a_texId1};
  res[0].instances.resize(1);

  memcpy(res[0].instances[0].worldViewMatrix, planeWorld, sizeof(planeWorld));
  memcpy(res[0].instances[0].projMatrix,      projM,      sizeof(projM));

  res[1].geom     = make_teapot();
  res[1].textures = {a_texId2, a_texId2, a_texId2, a_texId2, a_texId2, a_texId2, a_texId2, a_texId2, a_texId2};
  res[1].instances.resize(9);

  memcpy(res[1].instances[0].worldViewMatrix, teapotWorld0, sizeof(teapotWorld0));
  memcpy(res[1].instances[0].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[1].worldViewMatrix, teapotWorld1, sizeof(teapotWorld1));
  memcpy(res[1].instances[1].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[2].worldViewMatrix, teapotWorld2, sizeof(teapotWorld2));
  memcpy(res[1].instances[2].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[3].worldViewMatrix, teapotWorld3, sizeof(teapotWorld3));
  memcpy(res[1].instances[3].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[4].worldViewMatrix, teapotWorld4, sizeof(teapotWorld4));
  memcpy(res[1].instances[4].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[5].worldViewMatrix, teapotWorld5, sizeof(teapotWorld5));
  memcpy(res[1].instances[5].projMatrix,      projM,        sizeof(projM));
  
  memcpy(res[1].instances[6].worldViewMatrix, teapotWorld6, sizeof(teapotWorld6));
  memcpy(res[1].instances[6].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[7].worldViewMatrix, teapotWorld7, sizeof(teapotWorld7));
  memcpy(res[1].instances[7].projMatrix,      projM,        sizeof(projM));

  memcpy(res[1].instances[8].worldViewMatrix, teapotWorld8, sizeof(teapotWorld8));
  memcpy(res[1].instances[8].projMatrix,      projM,        sizeof(projM));

  return res;
}

inline static void color_unpack_bgra(uint32_t packedColor, float outColor[4])
{
  const uint32_t red   = (packedColor & 0x00FF0000) >> 16;
  const uint32_t green = (packedColor & 0x0000FF00) >> 8;
  const uint32_t blue  = (packedColor & 0x000000FF) >> 0;
  const uint32_t alpha = (packedColor & 0xFF000000) >> 24;
  
  outColor[0] = (1.0f / 255.0f)*(float)red;
  outColor[1] = (1.0f / 255.0f)*(float)green;
  outColor[2] = (1.0f / 255.0f)*(float)blue;
  outColor[3] = (1.0f / 255.0f)*(float)alpha;
}

GeomStorage make_terrain()
{
  int w,h;
  std::vector<uint32_t> heighpMap = LoadBMP("data/heightmap.bmp", &w, &h);

  GeomStorage geom;
  geom.vpos4f.resize(w*h*4);  
  geom.vcol4f.resize(w*h*4);  
  geom.vtex2f.resize(w*h*2);  
  geom.indices.resize(w*h*6);

  float invWidth  = 1.0f/float(w);
  float invHeight = 1.0f/float(h);

  float cornerLeftX = float(w) / 2.0f;
  float cornerLeftY = 0.0;
  float cornerLeftZ = float(h) / 2.0f;
  
  size_t triTop = 0;
  for (int y = 0; y < h; y++)
  {
      int y0 = y;
      int y1 = y + 1;
      if (y1 >= h) y1 = y;

      for (int x = 0; x < w; x++)
      {
        int x0 = x;
        int x1 = x + 1;
        if (x1 >= w) x1 = x;

        const int id0 = y0*w + x0;
        const int id1 = y0*w + x1;
        const int id2 = y1*w + x0;
        const int id3 = y1*w + x1;

        const int px0 = heighpMap[id0];
        const int px1 = heighpMap[id1];
        const int px2 = heighpMap[id2];
        const int px3 = heighpMap[id3];
        
        float px0f[4];
        float px1f[4];
        float px2f[4];
        float px3f[4];
        color_unpack_bgra(px0, px0f);
        color_unpack_bgra(px1, px1f);
        color_unpack_bgra(px2, px2f);
        color_unpack_bgra(px3, px3f);

        const float h0 = px0f[0] + px0f[1] + px0f[2];
        const float h1 = px1f[0] + px1f[1] + px1f[2];
        const float h2 = px2f[0] + px2f[1] + px2f[2];
        const float h3 = px3f[0] + px3f[1] + px3f[2];

        geom.vpos4f[4*id0 + 0] = (float)x0 - cornerLeftX;
        geom.vpos4f[4*id0 + 1] = h0        - cornerLeftY;
        geom.vpos4f[4*id0 + 2] = (float)y0 - cornerLeftZ;
        geom.vpos4f[4*id0 + 3] = 1.0f;

        geom.vpos4f[4*id1 + 0] = (float)x1 - cornerLeftX;
        geom.vpos4f[4*id1 + 1] = h1        - cornerLeftY;  
        geom.vpos4f[4*id1 + 2] = (float)y0 - cornerLeftZ;
        geom.vpos4f[4*id1 + 3] = 1.0f;

        geom.vpos4f[4*id2 + 0] = (float)x0 - cornerLeftX;
        geom.vpos4f[4*id2 + 1] = h2        - cornerLeftY;  
        geom.vpos4f[4*id2 + 2] = (float)y1 - cornerLeftZ;
        geom.vpos4f[4*id2 + 3] = 1.0f;

        geom.vpos4f[4*id3 + 0] = (float)x1 - cornerLeftX;
        geom.vpos4f[4*id3 + 1] = h3        - cornerLeftY;  
        geom.vpos4f[4*id3 + 2] = (float)y1 - cornerLeftZ;
        geom.vpos4f[4*id3 + 3] = 1.0f;
        
        for(int i=0;i<4;i++)
        {
          geom.vcol4f[4*id0 + i] = 1.0f;
          geom.vcol4f[4*id1 + i] = 1.0f;
          geom.vcol4f[4*id2 + i] = 1.0f;
          geom.vcol4f[4*id3 + i] = 1.0f;
        }

        geom.vtex2f[id0*2+0] = ((float)x0)*invWidth;
        geom.vtex2f[id0*2+1] = ((float)y0)*invWidth;

        geom.vtex2f[id1*2+0] = ((float)x1)*invWidth;
        geom.vtex2f[id1*2+1] = ((float)y0)*invWidth;

        geom.vtex2f[id2*2+0] = ((float)x0)*invWidth;
        geom.vtex2f[id2*2+1] = ((float)y1)*invWidth;

        geom.vtex2f[id3*2+0] = ((float)x1)*invWidth;
        geom.vtex2f[id3*2+1] = ((float)y1)*invWidth;
        
        if (x != w - 1 && y != h - 1)
        {
          geom.indices[triTop + 0] = id0;
          geom.indices[triTop + 1] = id2;
          geom.indices[triTop + 2] = id1;

          geom.indices[triTop + 3] = id1;
          geom.indices[triTop + 4] = id2;
          geom.indices[triTop + 5] = id3;
          triTop += 6;
        }
      }
  }


  return geom;
}

std::vector<SimpleScene> scn08_terrain(uint32_t a_texId1)
{
  float projM[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

  float terrainWorld[16] = {0.0625, 0, 0, 0, 
                            0, 0.906308, -0.0264136, 0, 
                            0, 0.422618, 0.0566442, -15, 
                            0, 0, 0, 1 };

  std::vector<SimpleScene> res;
  res.resize(1);
  
  res[0].geom     = make_terrain();
  res[0].textures = {a_texId1};
  res[0].instances.resize(1);

  memcpy(res[0].instances[0].worldViewMatrix, terrainWorld, sizeof(terrainWorld));
  memcpy(res[0].instances[0].projMatrix,      projM,        sizeof(projM));

  return res;
}