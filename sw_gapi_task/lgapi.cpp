#include "lgapi.h"
#include <memory>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

Vec2 operator - (const Vec2 &a, const Vec2 &B) 
{
  Vec2 res(a);

  res.x -= B.x;
  res.y -= B.y;

  return res;
}

float E(const Vec2 &A, const Vec2 &B, const Vec2 &P) 
{
  return ((P.x - A.x) * (B.y - A.y) - (P.y - A.y) * (B.x - A.x));
}

float E(const Vec2 &V, const Vec2 &VP)
{
  return (V.x * VP.y - V.y * VP.x);
}

long long F(const Vec2 &A, const Vec2 &B, const Vec2 &P)
{
  return ((A.y - B.y) * P.x + (B.x - A.x) * P.y + (A.x * B.y - A.y * B.x));
}

float find_min_3(float a, float b, float c) 
{
  if (a <= b && a <= c) {
    return a;
  }
  else if (b <= a && b <= c) {
    return b;
  }

  return c;
}

float find_max_3(float a, float b, float c) 
{
  if (a >= b && a >= c) {
    return a;
  }
  else if (b >= a && b >= c) {
    return b;
  }

  return c;
}

struct MyRender : public IRender
{
  Image2D fb;
  LightObj *L;
  float *depthBuf = nullptr;
  TextureContainer Textures;

  ~MyRender() override {
    if (depthBuf != nullptr) {
      delete [] depthBuf;
      depthBuf = nullptr;
    }
  }
  
  unsigned int AddImage(const Image2D &a_img) override;
  void block_rasterisation(const Vec2& A, const Vec2& B, const Vec2& C, const float &e, const int &blockSize, const int &j, const int &x, const float p[3][4],
                    const float col[3][4], const float uv[3][2], const PipelineStateObject &a_state, ispc::Vec4 LightDir, ispc::Vec4 Norm);
  void pixel_rasterisation(const Vec2& A, const Vec2& B, const Vec2& C, const float &e, const int &x0, const int &x1, 
                          const int &y0, const int &y1, const float p[3][4], const float col[3][4], const float uv[3][2], 
                          const PipelineStateObject &a_state, ispc::Vec4 LightDir, ispc::Vec4 Norm);
  void BeginRenderPass(Image2D &fb, LightObj *L) override;
  void Draw(PipelineStateObject a_state, Geom a_geom, float angle) override;
  void EndRenderPass(Image2D &fb) override;
};

std::shared_ptr<IRender> MakeMyImpl() 
{ 
  return std::make_shared<MyRender>(); 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MyRender::BeginRenderPass(Image2D &fb, LightObj *L = 0)
{
  this->fb = fb;
  
  size_t s = fb.width * fb.height;
  this->depthBuf = new float [s];
  
  for (size_t i = 0; i < s; i++) {
    depthBuf[i] = 0.f;
  }

  if (L != nullptr) {
    this->L = L;
  }
}

unsigned int MyRender::AddImage(const Image2D &a_img)
{
  size_t ind = Textures.addTexture(a_img);

  return ind;
}

void 
MyRender::block_rasterisation(const Vec2& A, const Vec2& B, const Vec2& C, const float &e, const int &blockSize, const int &j, const int &x, const float p[3][4],
                    const float col[3][4], const float uv[3][2], const PipelineStateObject &a_state, ispc::Vec4 LightDir, ispc::Vec4 Norm)
{
  
  for (int bl_y = j; bl_y <= j + blockSize && bl_y < fb.height; ++bl_y) {
      for (int bl_x = x; bl_x <= x + blockSize && bl_x < fb.width; ++bl_x) {
        Vec2 Point = Vec2((float)bl_x, (float)bl_y);
        float w0 = E(B - A, Point - A), w1 = E(C - B, Point - B), w2 = E(A - C, Point - C);
        
        w0 = abs(w0) / e;
        w1 = abs(w1) / e;
        w2 = abs(w2) / e;

        float w = (p[2][2] * w0 + p[0][2] * w1 + p[1][2] * w2);

        if (1 / w < 1 / depthBuf[fb.width * bl_y + bl_x]) {
          depthBuf[fb.width * bl_y + bl_x] = w;

          struct ispc::Vec4 colorV[4];
          struct ispc::Vec4 BarCord[4];

          BarCord[0].x = w0;
          BarCord[0].y = w1;
          BarCord[0].z = w2;
          BarCord[0].w = w;

          if (a_state.imgId != uint32_t(-1)) {
            Image2D texture = Textures.textures[a_state.imgId];
            a_state.shader_container->textureShader(texture.data, &texture.width, &texture.height, uv, BarCord, &fb.data[fb.width * bl_y + bl_x]);
          }
          else {
            a_state.shader_container->colorShader(col, BarCord, colorV);
      
            if (a_state.shader_container->ambientLightShader != nullptr) {
              ispc::Vec4 new_ambient_color[4];

              a_state.shader_container->ambientLightShader(colorV, L->Color, new_ambient_color);
              a_state.shader_container->diffusalLightShader(new_ambient_color, L->Color, &LightDir, &Norm, colorV);  
            }
            
            uint32_t color = ((unsigned char)(colorV[0].x) << 16) + ((unsigned char)(colorV[0].y) << 8) + ((unsigned char)(colorV[0].z));
            fb.data[fb.width * bl_y + bl_x] = color;
          }
        }
      }
    }
}

void
MyRender::pixel_rasterisation(const Vec2& A, const Vec2& B, const Vec2& C, const float &e, const int &x0, const int &x1, 
                          const int &y0, const int &y1, const float p[3][4], const float col[3][4], const float uv[3][2], 
                          const PipelineStateObject &a_state, ispc::Vec4 LightDir, ispc::Vec4 Norm)
{
  for (uint32_t x = x0; x <= x1 && x < fb.width; x++) {
        for (uint32_t y = y0; y <= y1 && y < fb.height; y++) {
          Vec2 Point = Vec2((float)x, (float)y);
          float w0 = E(B - A, Point - A), w1 = E(C - B, Point - B), w2 = E(A - C, Point - C);

          if ((w0 > 0 && w1 > 0 && w2 > 0) || (w0 < 0 && w1 < 0 && w2 < 0)) {
            w0 = abs(w0) / e;
            w1 = abs(w1) / e;
            w2 = abs(w2) / e;

            float w = (p[2][2] * w0 + p[0][2] * w1 + p[1][2] * w2);

            if (1 / w < 1 / depthBuf[fb.width * y + x]) {
              depthBuf[fb.width * y + x] = w;

              struct ispc::Vec4 colorV[4];
              struct ispc::Vec4 BarCord[4];

              BarCord[0].x = w0;
              BarCord[0].y = w1;
              BarCord[0].z = w2;
              BarCord[0].w = w;

              if (a_state.imgId != uint32_t(-1)) {
                Image2D texture = Textures.textures[a_state.imgId];
                a_state.shader_container->textureShader(texture.data, &texture.width, &texture.height, uv, BarCord, &fb.data[fb.width * y + x]);
              }
              else {
                a_state.shader_container->colorShader(col, BarCord, colorV);
                
                if (a_state.shader_container->ambientLightShader != nullptr) {
                  ispc::Vec4 new_ambient_color[4];

                  a_state.shader_container->ambientLightShader(colorV, L->Color, new_ambient_color);
                  a_state.shader_container->diffusalLightShader(new_ambient_color, L->Color, &LightDir, &Norm, colorV);  
                }
                
                uint32_t color = ((unsigned char)(colorV[0].x) << 16) + ((unsigned char)(colorV[0].y) << 8) + ((unsigned char)(colorV[0].z));
                fb.data[fb.width * y + x] = color;
              }
            }
          }
        }
      }  
}

void MyRender::Draw(PipelineStateObject a_state, Geom a_geom, float angle = 0.0)
{
  float worldViewProj[16];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      worldViewProj[i * 4 + j] = 0.f;

      for (int k = 0; k < 4; k++) {
        worldViewProj[i * 4 + j] += a_state.projMatrix[i * 4 + k] * a_state.worldViewMatrix[k * 4 + j];
      }
    }
  }

  ispc::Vec4 vertex_vec[4];

  float rotatedWorldViewProj[16];
  float rotateMtrx[16] = {
    cos(angle), 0, sin(angle), 0,
    0,          1,          0, 0,
    -sin(angle),0, cos(angle), 0,
    0,          0,          0, 1,
  };

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      rotatedWorldViewProj[i * 4 + j] = 0.f;
      for (int k = 0; k < 4; k++) {
        rotatedWorldViewProj[i * 4 + j] += worldViewProj[i * 4 + k] * rotateMtrx[k * 4 + j];
      }
    }
  }

  for (unsigned int tr_num = 0; tr_num < a_geom.primsNum; tr_num++) {
    unsigned vert_indx[3] = {
      a_geom.indices[tr_num * 3 + 0],
      a_geom.indices[tr_num * 3 + 1],
      a_geom.indices[tr_num * 3 + 2]
    };

    float p[3][4] = {{}};
    float col[3][4] = {{}};
    float uv[3][2] = {{}};
    float p_n[3][4] = {{}};

    for (int ind_ver = 0; ind_ver < 3; ++ind_ver) {
      float vpos4f_i[4] = {
        a_geom.vpos4f[4 * vert_indx[ind_ver]],
        a_geom.vpos4f[4 * vert_indx[ind_ver] + 1],
        a_geom.vpos4f[4 * vert_indx[ind_ver] + 2],
        a_geom.vpos4f[4 * vert_indx[ind_ver] + 3],
      };

      if (a_state.shader_container->ambientLightShader != nullptr) {
        for (int i = 0; i < 4; ++i) {
          p_n[ind_ver][i] = 0;
          for (int j = 0; j < 4; ++j) {
            p_n[ind_ver][i] += a_state.worldViewMatrix[4 * i + j] * vpos4f_i[j];
          }
        }
      }

      a_state.shader_container->vertexShader
        (rotatedWorldViewProj, vpos4f_i, vertex_vec);

      p[ind_ver][0] = vertex_vec[0].x;
      p[ind_ver][1] = vertex_vec[0].y;
      p[ind_ver][2] = vertex_vec[0].z;
      p[ind_ver][3] = vertex_vec[0].w;

      float w = vertex_vec[0].w;

      for (int i = 0; i < 2; i++) {
        p[ind_ver][i] *= 0.5f;
        p[ind_ver][i] += 0.5f;
      }

      p[ind_ver][0] *= (float)fb.width;
      p[ind_ver][1] *= (float)fb.height;

      for (int i = 0; i < 2; i++) {
        p[ind_ver][i] -= 0.5f;
      }

      p[ind_ver][2] = 1 / w;

      if (a_state.imgId != uint32_t(-1)) {
        uv[ind_ver][0] = a_geom.vtex2f[2 * vert_indx[ind_ver]] / w;
        uv[ind_ver][1] = a_geom.vtex2f[2 * vert_indx[ind_ver] + 1] / w;
      }
      else {
        col[ind_ver][0] = a_geom.vcol4f[4 * vert_indx[ind_ver]] / w;
        col[ind_ver][1] = a_geom.vcol4f[4 * vert_indx[ind_ver] + 1] / w;
        col[ind_ver][2] = a_geom.vcol4f[4 * vert_indx[ind_ver] + 2] / w;
        col[ind_ver][3] = a_geom.vcol4f[4 * vert_indx[ind_ver] + 3] / w;
      }
    }

    Vec2 A(p[0][0], p[0][1]), B(p[1][0], p[1][1]), C(p[2][0], p[2][1]);
    ispc::Vec4 LightDir;
    ispc::Vec4 Norm;

    if (a_state.shader_container->ambientLightShader != nullptr) {
      Vec4 a = {p_n[1][0] - p_n[0][0], p_n[1][1] - p_n[0][1], p_n[1][2] - p_n[0][2], 0};
      Vec4 b = {p_n[2][0] - p_n[1][0], p_n[2][1] - p_n[1][1], p_n[2][2] - p_n[1][2], 0};

      Norm = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
      float norm_len = sqrt(pow(Norm.x, 2) + pow(Norm.y, 2) + pow(Norm.z, 2));

      Norm.x /= norm_len;
      Norm.y /= norm_len;
      Norm.z /= norm_len;

      LightDir = {L->x - p_n[0][0], L->y - p_n[0][1], L->z - p_n[0][2], 0};
      float lightDir_len = sqrt(pow(LightDir.x, 2) + pow(LightDir.y, 2) + pow(LightDir.z, 2));

      LightDir.x /= lightDir_len;
      LightDir.y /= lightDir_len;
      LightDir.z /= lightDir_len;
    }

    float xmin = find_min_3(p[0][0], p[1][0], p[2][0]), xmax = find_max_3(p[0][0], p[1][0], p[2][0]); 
    float ymin = find_min_3(p[0][1], p[1][1], p[2][1]), ymax = find_max_3(p[0][1], p[1][1], p[2][1]);

    int x0 = std::max(0, (int32_t)(xmin) - 1), y0 = std::max(0, (int32_t)(ymin) - 1);
    int x1 = std::max(0, (int32_t)(xmax) + 1), y1 = std::max(0, (int32_t)(ymax) + 1);

    float e = E(A, B, C);
    int m = (e > 0) ? 1 : -1;
    e = abs(e);

    int q = 8;
    int blockSize = q;
    
    double orientation = (double)(x1 - x0) / (double)(y1 - y0);

    // if (orientation >= 0.4 && orientation <= 1.6) {
    //   //  the highest block
    //   Vec2 D1 = (A.getY() == y0) ? A : (B.getY() == y0) ? B : C;
    //   long long left_bound = (long long)D1.getX() - (long long)D1.getX() % blockSize, right_bound = left_bound + 2 * blockSize;
    //   long long halfY = y0 + ((y1 - y0) >> 1);

    //   const Vec2 P1 = B - A, P2 = C - B, P3 = A - C;

    //   for (int64_t j = y0; j <= halfY + blockSize; ++j) {
    //     long long midPoint = left_bound + ((right_bound - left_bound) >> 1);
    //     int64_t x = midPoint;
    //     float C1y = j, C2y = j + blockSize - 1;
        
    //     for (int k = 0; k < 2; ++k) {
    //       for (; (q > 0 ? x < x1 : x > x0 - blockSize); x += q) {
    //         float C1x = x, C2x = x + blockSize - 1;
    //         Vec2 M1(C1x, C1y), M2(C2x, C2y), M3(C1x, C2y), M4(C2x, C1y);
    //         float w01 = E(P1, M1 - A) * m, w11 = E(P2, M1 - B) * m, w21 = E(P3, M1 - C) * m;
    //         float w02 = E(P1, M2 - A) * m, w12 = E(P2, M2 - B) * m, w22 = E(P3, M2 - C) * m;
    //         float w03 = E(P1, M3 - A) * m, w13 = E(P2, M3 - B) * m, w23 = E(P3, M3 - C) * m;
    //         float w04 = E(P1, M4 - A) * m, w14 = E(P2, M4 - B) * m, w24 = E(P3, M4 - C) * m;

    //         if ((w01 > 0 && w02 > 0 && w03 > 0 && w04 > 0) || 
    //             (w11 > 0 && w12 > 0 && w13 > 0 && w14 > 0) || 
    //             (w21 > 0 && w22 > 0 && w23 > 0 && w24 > 0)) {
    //               continue;
    //             }
    //         else if ((w01 < 0 && w02 < 0 && w03 < 0 && w04 < 0) && 
    //             (w11 < 0 && w12 < 0 && w13 < 0 && w14 < 0) && 
    //             (w21 < 0 && w22 < 0 && w23 < 0 && w24 < 0)) {
    //               block_rasterisation(A, B, C, e, blockSize, j, x, p, col, uv, a_state, LightDir, Norm);
    //               continue;
    //             }

    //         pixel_rasterisation(A, B, C, e, x, x + blockSize, j, j + blockSize, p, col, uv, a_state, LightDir, Norm);  
    //       }

    //       q = -q;

    //       if (k == 0) {
    //         right_bound = x - blockSize;
    //       }
    //       else {
    //         left_bound = x + blockSize;
    //       }

    //       x = midPoint - blockSize;
    //     }
    //   }
      
    //   //  the lowwest block
    //   Vec2 D2 = (A.getY() == y1) ? A : (B.getY() == y1) ? B : C;
    //   left_bound = (long long)D2.getX() - (long long)D2.getX() % blockSize, right_bound = left_bound + 2 * blockSize;

    //   for (int64_t j = y1; j >= halfY; j -= blockSize) {
    //     long long midPoint = (left_bound + ((right_bound - left_bound) >> 1));
    //     int64_t x = midPoint;
    //     float C1y = j, C2y = j + blockSize - 1;

    //     for (int k = 0; k < 2; ++k) {
    //       for (; (q > 0 ? x < x1 : x > x0 - blockSize); x += q) {
    //         float C1x = x, C2x = x + blockSize - 1;
    //         Vec2 M1(C1x, C1y), M2(C2x, C2y), M3(C1x, C2y), M4(C2x, C1y);
    //         float w01 = E(P1, M1 - A) * m, w11 = E(P2, M1 - B) * m, w21 = E(P3, M1 - C) * m;
    //         float w02 = E(P1, M2 - A) * m, w12 = E(P2, M2 - B) * m, w22 = E(P3, M2 - C) * m;
    //         float w03 = E(P1, M3 - A) * m, w13 = E(P2, M3 - B) * m, w23 = E(P3, M3 - C) * m;
    //         float w04 = E(P1, M4 - A) * m, w14 = E(P2, M4 - B) * m, w24 = E(P3, M4 - C) * m;

    //         if ((w01 > 0 && w02 > 0 && w03 > 0 && w04 > 0) || 
    //             (w11 > 0 && w12 > 0 && w13 > 0 && w14 > 0) || 
    //             (w21 > 0 && w22 > 0 && w23 > 0 && w24 > 0)) {
    //               continue;
    //             }
    //         else if ((w01 < 0 && w02 < 0 && w03 < 0 && w04 < 0) && 
    //             (w11 < 0 && w12 < 0 && w13 < 0 && w14 < 0) && 
    //             (w21 < 0 && w22 < 0 && w23 < 0 && w24 < 0)) {
    //               block_rasterisation(A, B, C, e, blockSize, j, x, p, col, uv, a_state, LightDir, Norm);
    //               continue;
    //             }

    //         pixel_rasterisation(A, B, C, e, x, x + blockSize, j, j + blockSize, p, col, uv, a_state, LightDir, Norm);  
    //       }

    //       q = -q;

    //       if (k == 0) {
    //         right_bound = x - blockSize;
    //       }
    //       else {
    //         left_bound = x + blockSize;
    //       }

    //       x = midPoint - blockSize;
    //     }
    //   }

    // }
    // else {
    //   pixel_rasterisation(A, B, C, e, x0, x1, y0, y1, p, col, uv, a_state, LightDir, Norm);
    // }

    const Vec2 P1 = B - A, P2 = C - B, P3 = A - C;

    for (int j = y0; j <= y1 && j < fb.height; j += blockSize) {
      float C1y = j, C2y = j + blockSize - 1;
      
      for (int i = x0; i <= x1 && i < fb.width; i += blockSize) {
        bool flag = true;
        float C1x = i, C2x = i + blockSize - 1;
        Vec2 M1(C1x, C1y), M2(C2x, C2y), M3(C1x, C2y), M4(C2x, C1y);
        float w01 = E(P1, M1 - A) * m, w11 = E(P2, M1 - B) * m, w21 = E(P3, M1 - C) * m;
        float w02 = E(P1, M2 - A) * m, w12 = E(P2, M2 - B) * m, w22 = E(P3, M2 - C) * m;
        float w03 = E(P1, M3 - A) * m, w13 = E(P2, M3 - B) * m, w23 = E(P3, M3 - C) * m;
        float w04 = E(P1, M4 - A) * m, w14 = E(P2, M4 - B) * m, w24 = E(P3, M4 - C) * m;

        if ((w01 > 0 && w02 > 0 && w03 > 0 && w04 > 0) || 
            (w11 > 0 && w12 > 0 && w13 > 0 && w14 > 0) || 
            (w21 > 0 && w22 > 0 && w23 > 0 && w24 > 0)) {
          flag = false;
        }
        else if ((w01 < 0 && w02 < 0 && w03 < 0 && w04 < 0) && 
            (w11 < 0 && w12 < 0 && w13 < 0 && w14 < 0) && 
            (w21 < 0 && w22 < 0 && w23 < 0 && w24 < 0)) {
          block_rasterisation(A, B, C, e, blockSize, j, i, p, col, uv, a_state, LightDir, Norm);
          flag = false;
        }

        if (flag) {
          pixel_rasterisation(A, B, C, e, i, i + blockSize, j, j + blockSize, p, col, uv, a_state, LightDir, Norm);
        }
      }
    }
  }
}

void MyRender::EndRenderPass(Image2D &fb)
{
   std::fill(&depthBuf[0], &depthBuf[fb.width * fb.height - 1], 0);
}


