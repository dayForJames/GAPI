#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdint>
#include <chrono>
#include<sys/wait.h>

using namespace std;

// #include <X11/Xlib.h>
// #include <X11/Xutil.h>
// #include <X11/Xos.h>

#include "Bitmap.h"
#include "lgapi.h"
#include "scene.h"
#include "shader.h"
#include "OBJparsing.h"

// Display *dis;
// int default_screen;
// Window win;
// GC gc;
// Visual *vis;
// int default_depth = 0;

// unsigned long black, white, blue;

// void init()
// {
//   int x = 100, y = 100;
//   dis = XOpenDisplay((char *)0);

//   default_screen = XDefaultScreen(dis);
//   win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 1024, 1024, 5, white, black);
//   vis = DefaultVisual(dis, default_screen);
//   default_depth = XDefaultDepth(dis, default_screen);

//   XSetStandardProperties(dis, win, "h", "w", None, NULL, 0, NULL);
//   XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);

//   gc = XCreateGC(dis, win, 0, 0);
//   XSetBackground(dis, gc, white);
//   XSetForeground(dis, gc, black);
//   XClearWindow(dis, win);
//   XMapRaised(dis, win);
// }

// void close()
// {
//   XFreeGC(dis, gc);
//   XDestroyWindow(dis, win);
//   XCloseDisplay(dis);
//   exit(0);
// }

// void draw()
// {
//   XClearWindow(dis, win);
// }

std::shared_ptr<IRender> MakeReferenceImpl(); ///< reference implementation via OpenGL
std::shared_ptr<IRender> MakeMyImpl();        ///< your implementation

void DrawInstances(const SimpleScene& scn, std::shared_ptr<IRender> pRender, RENDER_MODE a_mode, ShaderContainer *shader_container, float angle = 0.0)
{
  Geom  geom;
  geom.vpos4f   = scn.geom.vpos4f.data();
  geom.vcol4f   = scn.geom.vcol4f.data();
  geom.vtex2f   = scn.geom.vtex2f.data();
  geom.indices  = scn.geom.indices.data();
  geom.vertNum  = uint32_t(scn.geom.vpos4f.size()/4);
  geom.primsNum = uint32_t(scn.geom.indices.size()/3);

  for(size_t instId = 0; instId < scn.instances.size(); instId++)
  {
    PipelineStateObject pso;
    pso.shader_container = shader_container;

    memcpy(pso.worldViewMatrix, scn.instances[instId].worldViewMatrix, sizeof(pso.worldViewMatrix));
    memcpy(pso.projMatrix,      scn.instances[instId].projMatrix,      sizeof(pso.projMatrix));
    pso.mode  = a_mode;
    if(instId < scn.textures.size())     // do we have texture for this instance?
      pso.imgId = scn.textures[instId];  
    else
      pso.imgId = uint32_t(-1);          
    pRender->Draw(pso, geom, angle);
  }
}

uint32_t WIN_WIDTH  = 1024;
uint32_t WIN_HEIGHT = 1024;

int main(int argc, const char** argv)
{
  std::vector<uint32_t> pixelData(WIN_WIDTH*WIN_HEIGHT);
  Image2D fb(WIN_WIDTH, WIN_HEIGHT, pixelData.data());
  size_t img_size = WIN_HEIGHT * WIN_WIDTH;
  
  #ifdef USE_OPENGL
  std::shared_ptr<IRender> pRender = MakeReferenceImpl();
  std::string imgName = "wref_";
  #else
  std::shared_ptr<IRender> pRender = MakeMyImpl();
  std::string imgName = "zout_";
  #endif

  ShaderContainer *shader_container = new ShaderContainer();
  
  uint32_t testTexId, mosaicTexId, bricksTexId, terrainTex, statue1, statue2;

  int w, h;
  std::vector<unsigned> pixels;
  pixels      = LoadBMP("./data/texture1.bmp", &w, &h);
  Image2D A = Image2D(w,h,pixels.data());
  testTexId   = pRender->AddImage(A);
  std::vector<unsigned> pixels1;
  pixels1      = LoadBMP("./data/mosaic.bmp", &w, &h);
  Image2D B = Image2D(w,h,pixels1.data());
  mosaicTexId = pRender->AddImage(B);
  std::vector<unsigned> pixels2;
  pixels2      = LoadBMP("./data/red_brick.bmp", &w, &h);
  Image2D C = Image2D(w,h,pixels2.data());
  bricksTexId = pRender->AddImage(C);
  std::vector<unsigned> pixels3;
  pixels3      = LoadBMP("./data/terrain.bmp", &w, &h);
  Image2D D = Image2D(w,h,pixels3.data());
  terrainTex  = pRender->AddImage(D);
  std::vector<unsigned> pixels4;
  pixels4 = LoadBMP("./data/Default-OBJ_BaseColor.bmp", &w, &h);
  Image2D E = Image2D(w,h,pixels4.data());
  statue1 = pRender->AddImage(E);
  std::vector<unsigned> pixels5;
  pixels5 = LoadBMP("/Users/sergei/Downloads/xenophon/source/Xenophon_C/Xenophon_CN_Xenophon_O_Material_u1_v1.bmp", &w, &h);
  Image2D F = Image2D(w,h,pixels5.data());
  statue2 = pRender->AddImage(E);

  // test #01
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn01_colored_triangle(); 
    auto before  = std::chrono::high_resolution_clock::now();  

    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_VERT_COLOR, shader_container);
    pRender->EndRenderPass(fb);


    std::string name = imgName + "01" + ".bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);

    float time = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f) / 10;
    std::cout << "test_01: " << time << " ms" << std::endl;
  }

  // test #02
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn02_tri_and_quad();
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_VERT_COLOR, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_02: " << time << " ms" << std::endl;

    std::string name = imgName + "02.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #03
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn03_pyr_and_cube();
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_VERT_COLOR, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_03: " << time << " ms" << std::endl;

    std::string name = imgName + "03.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #04
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn04_cube(testTexId);
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_04: " << time << " ms" << std::endl;

    std::string name = imgName + "04.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #05
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    // init();
    // XEvent event;
    // KeySym key;
    // char text[255];
    auto objects = scn05_cubes_many(testTexId, mosaicTexId, bricksTexId);

    char *data = new char [4 * fb.width * fb.height];
    float angle = M_PI / 1000;
    int i = 0;

    pRender->BeginRenderPass(fb);

    // if (!fork()) {
    //   int fps = 0, count = 0;
    //   auto before  = std::chrono::high_resolution_clock::now();
    //   while (1) {
    //     if (XPending(dis)) {
    //       XNextEvent(dis, &event);

    //       if (event.type == KeyPress && XLookupString(&event.xkey, text, 255, &key, 0) == 1) {
    //         if (text[0] == 'q') {
    //           close();
    //         }
    //       }
    //     }
        
    //     for(const auto& obj : objects)
    //       DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container, angle * i);
    //     pRender->EndRenderPass(fb);

    //     for (int i = 0; i < fb.height; ++i) {
    //       for (int j = 0; j < fb.width; ++j) {
    //         unsigned int color = fb.data[fb.width * i + j];

    //         data[4 * (fb.width * (fb.height - i) + j) + 0] = (color & 0x00FF0000) >> 16;
    //         data[4 * (fb.width * (fb.height - i) + j) + 1] = (color & 0x0000FF00) >> 8;
    //         data[4 * (fb.width * (fb.height - i) + j) + 2] = (color & 0x000000FF);
    //         data[4 * (fb.width * (fb.height - i) + j) + 3] = 0;
    //       }
    //     }
        
    //     std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
    //     XImage *ximage = XCreateImage(dis, vis, default_depth, ZPixmap, 24, data, fb.width, fb.height, 32, 0);
    //     XPutImage(dis, win, gc, ximage, 0, 0, 0, 0, fb.width, fb.height);

    //     i = (i + 1) % 1000;

    //     float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    //     if (time > 1000) {
    //       std::cout << "FPS: " << count << std::endl;
    //       count = 0;
    //       before = std::chrono::high_resolution_clock::now();
    //     }
    //     else {
    //       count++;
    //     }
    //   }

    //   delete [] data;
    // }

    // wait(NULL);
    auto before  = std::chrono::high_resolution_clock::now();

    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container, angle * i);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_05: " << time << " ms" << std::endl;

    std::string name = imgName + "05.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #06
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn06_teapot(testTexId, mosaicTexId);
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_06: " << time << " ms" << std::endl;

    std::string name = imgName + "06.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);
    
    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #07
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn07_teapots_many(testTexId, mosaicTexId);
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_07: " << time << " ms" << std::endl;

    std::string name = imgName + "07.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #08
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    auto objects = scn08_terrain(terrainTex);
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    for(const auto& obj : objects)
      DrawInstances(obj, pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_08: " << time << " ms" << std::endl;

    std::string name = imgName + "08.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #09
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorCubeLightSourceShader;
    shader_container->textureShader = ispc::textureShader;

    LightObj Light;

    auto objects = scn04_cube(uint32_t(-1));
    auto before  = std::chrono::high_resolution_clock::now();
    
    auto LightSourse = objects[0];
    auto Cube = objects[1];

    Light.Color[0] = {255, 255, 255};

    Light.x = LightSourse.geom.vpos4f[0];
    Light.y = LightSourse.geom.vpos4f[1];
    Light.z = LightSourse.geom.vpos4f[2];
    Light.w = LightSourse.geom.vpos4f[3];

    pRender->BeginRenderPass(fb, &Light);

    DrawInstances(LightSourse, pRender, MODE_TEXURE_3D, shader_container);
    shader_container->ambientLightShader = nullptr;
    shader_container->colorShader = ispc::CubeStartColorShader;
    
    shader_container->ambientLightShader = ispc::ambientLightShader;
    shader_container->diffusalLightShader = ispc::diffusalLightShader;
    DrawInstances(Cube, pRender, MODE_TEXURE_3D, shader_container);

    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_09: " << time << " ms" << std::endl;

    std::string name = imgName + "09.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);

    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #10
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorShader;
    shader_container->textureShader = ispc::textureShader;
    shader_container->ambientLightShader = nullptr;
    shader_container->diffusalLightShader = nullptr;

    float proj1[16] = {1, 0, 0, 0, 
                     0, 1, 0, 0, 
                     0, 0, 1, 1, 
                     0, 0, 1, 0 };

    float pyr1World[16] = {1, 0, 0, 0, 
                       0, 1, 0, 0, 
                       0, 0, -1, 0, 
                       0, 0, 0, 1 };

    std::string file_name1 = "./data/uploads_files_3425113_VenuDeMilo.obj";
    auto objects = ObjParse(file_name1);
    memcpy(objects[0].instances[0].worldViewMatrix, pyr1World, sizeof(pyr1World));
    memcpy(objects[0].instances[0].projMatrix,      proj1, sizeof(proj1));
    objects[0].textures = {statue1};
    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    DrawInstances(objects[0], pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_10: " << time << " ms" << std::endl;

    std::string name = imgName + "10.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);
    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #11
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->colorShader = ispc::colorBarShader;
    shader_container->textureShader = ispc::textureShader;
    shader_container->ambientLightShader = nullptr;
    shader_container->diffusalLightShader = nullptr;

    float proj1[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

    float pyr1World[16] = {0.761858, -0.323205, 0.561347, 1.5, 
                          0.561347, 0.761858, -0.323205, 0, 
                          -0.323205, 0.561347, 0.761858, -7, 
                          0, 0, 0, 1 };

    std::string file_name2 = "./data/vv.obj";
    auto objects = ObjParse(file_name2);
    memcpy(objects[0].instances[0].worldViewMatrix, pyr1World, sizeof(pyr1World));
    memcpy(objects[0].instances[0].projMatrix,      proj1, sizeof(proj1));

    auto before  = std::chrono::high_resolution_clock::now();
    
    pRender->BeginRenderPass(fb);
    DrawInstances(objects[0], pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_11: " << time << " ms" << std::endl;

    std::string name = imgName + "11.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);
    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  // test #12
  {
    shader_container->vertexShader = ispc::vertexShader;
    shader_container->textureShader = ispc::textureShader;
    shader_container->ambientLightShader = nullptr;
    shader_container->diffusalLightShader = nullptr;

    float proj1[16] = {2.41421, 0, 0, 0, 
                     0, 2.41421, 0, 0, 
                     0, 0, -1.002, -0.2002, 
                     0, 0, -1, 0 };

    float pyr1World[16] = {0.761858, -0.323205, 0.561347, 1.5, 
                          0.561347, 0.761858, -0.323205, 0, 
                          -0.323205, 0.561347, 0.761858, -7, 
                          0, 0, 0, 1 };

    std::string file_name1 = "/Users/sergei/Downloads/xenophon/source/Xenophon_C/Xenophon_CN.obj";
    auto objects = ObjParse(file_name1);
    memcpy(objects[0].instances[0].worldViewMatrix, pyr1World, sizeof(pyr1World));
    memcpy(objects[0].instances[0].projMatrix,      proj1, sizeof(proj1));
    //objects[0].textures = {uint32_t(-1)}; //{statue2};
    auto before  = std::chrono::high_resolution_clock::now();

    pRender->BeginRenderPass(fb);
    DrawInstances(objects[0], pRender, MODE_TEXURE_3D, shader_container);
    pRender->EndRenderPass(fb);

    float time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - before).count()/1000.f;
    std::cout << "test_12: " << time << " ms" << std::endl;

    std::string name = imgName + "12.bmp";  
    SaveBMP(name.c_str(), pixelData.data(), WIN_WIDTH, WIN_HEIGHT);
    std::fill(&fb.data[0], &fb.data[fb.width * fb.height - 1], 0);
  }

  return 0;
}
