#pragma once 

#include <vector>
#include <cmath>
#include <cstring>
#include "shader.h"
#include "Vec4.h"

enum RENDER_MODE { MODE_VERT_COLOR = 0,
                   MODE_TEXURE_3D  = 1, };

class Vec2 {
  float x;
  float y;
public:
  Vec2(float x = 0, float y = 0) 
  {
    this->x = x;
    this->y = y;
  }

  Vec2(const Vec2 &obj)
  {
    this->x = obj.x;
    this->y = obj.y;
  }

  Vec2 & operator = (const Vec2 &obj) {
    this->x = obj.x;
    this->y = obj.y;

    return *this;
  }

  float getX() const
  {
    return x;
  }

  float getY() const
  {
    return y;
  }

  friend Vec2 operator - (const Vec2 &a, const Vec2 &B);

  friend float E(const Vec2 &A, const Vec2 &B, const Vec2 &P);
  friend float E(const Vec2 &V, const Vec2 &VP);
  friend long long F(const Vec2 &A, const Vec2 &B, const Vec2 &P);
  friend void RenderPartiallyCoveredBlock(int j, int x, int blockSize, Vec2 A, Vec2 B, Vec2 C);
};

struct LightObj {
    float x;
    float y;
    float z;
    float w;
    
    ispc::Vec4 Color[4];
};

enum GEOM_TYPE { GEOM_TRIANGLES = 1, GEOM_QUADS = 2 };

struct Geom
{
  const float*        vpos4f  = nullptr; ///< quads of vertex coordinates for vertex positions x,y,z,w; w is unused
  const float*        vcol4f  = nullptr; ///< quads of vertex color attributes r,g,b,a; a is unused
  const float*        vtex2f  = nullptr; ///< pairs of x and y coordinates
  const unsigned int* indices = nullptr; ///< index buffer of size 3*primsNum for triangle meshes and 4*primsNum for qual meshes

  unsigned int vertNum  = 0;
  unsigned int primsNum = 0;
  GEOM_TYPE    geomType = GEOM_TRIANGLES;
};

struct Image2D
{
  Image2D(){}
  Image2D(unsigned int w, unsigned int h, unsigned int* a_data) : data(a_data), width(w), height(h) {}

  unsigned int* data;   ///< access pixel(x,y) as data[y*width+x]
  unsigned int  width;
  unsigned int  height; 
};

struct TextureContainer {
  std::vector<Image2D> textures;
  // struct Image2D *textures; 
  // size_t len = 0;

  size_t size()
  {
    return textures.size();
    //return len;
  }

  size_t addTexture(const Image2D &tex)
  {
    textures.push_back(tex);

    return textures.size() - 1;

    // Image2D *tmp = new Image2D[len + 1];
    
    // memcpy(tmp, textures, sizeof(Image2D) * len);
    // tmp[len++] = tex;

    // if (textures != nullptr) {
    //   delete [] textures;
    // }

    // textures = new Image2D[len + 1];

    // for (int i = 0; i < len; ++i) {
    //   textures[i] = tmp[i];
    // }

    // delete[] tmp;

    // return len - 1;
  }
};

struct ShaderContainer {
  void (*vertexShader)(const float *, const float *, ispc::Vec4 r[4]);
  void (*textureShader)(const uint32_t [], const uint32_t [], const uint32_t [], const float [3][2], const ispc::Vec4 [4], uint32_t [4]);
  void (*colorShader)(const float [3][4], ispc::Vec4 BarCord[4], ispc::Vec4 r[4]);
  //uint32_t (*lightShader)(const float col[3][4], const float &w0, const float &w1, const float &w2, const float &w);
  void (*ambientLightShader)(const ispc::Vec4 ObjColor[4], const ispc::Vec4 LightSourceColor[4], ispc::Vec4 r[4]);
  void (*diffusalLightShader)(const ispc::Vec4 *ObjColor, const ispc::Vec4 *LightColor, const ispc::Vec4 *LightDir, const ispc::Vec4 *Norm, ispc::Vec4 *r);
};

struct PipelineStateObject
{
  float worldViewMatrix[16]; ///< assume row-major layout, i.e. M[0], M[1], M[2], M[3] is the first row of the matrix
  float projMatrix[16];      ///< assume row-major layout, i.e. M[0], M[1], M[2], M[3] is the first row of the matrix
  RENDER_MODE  mode  = MODE_VERT_COLOR;
  unsigned int imgId = 0;

  ShaderContainer *shader_container;
};

struct IRender
{
  IRender(){}
  virtual ~IRender(){}
  
  virtual unsigned int AddImage(const Image2D &a_img) = 0;

  virtual void BeginRenderPass(Image2D &fb, LightObj *L = 0) = 0;
  virtual void Draw(PipelineStateObject a_state, Geom a_geom, float angle = 0.0) = 0;
  virtual void EndRenderPass(Image2D &fb) = 0;
};