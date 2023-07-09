#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>

std::vector<SimpleScene> ObjParse(std::string file_path)
{
    std::ifstream r;
    std::string file_line;

    std::vector<float> all_vpos_4f;
    std::vector<float> all_uv;
    std::vector<float> all_norms;
    std::vector<uint32_t> indices;
    std::vector<float> col;
    std::vector<float> vtex2f;
    std::vector<float> tmp_vtex2f;
    //a_geom.vtex2f[2 * vert_indx[ind_ver]] / w;

    std::vector<SimpleScene> res;
    
    r.open(file_path);

    if (!r.is_open()) {
        std::cout << "Cant't open file" << std::endl;
    }

    while (std::getline(r, file_line)) {
        if (file_line[0] == 'v' && file_line[1] == ' ') {
            std::stringstream s(file_line.substr(2));
            Vec4 vertex;

            s >> vertex.x >> vertex.y >> vertex.z;
            vertex.w = 1;

            all_vpos_4f.push_back(vertex.x);
            all_vpos_4f.push_back(vertex.y);
            all_vpos_4f.push_back(vertex.z);
            all_vpos_4f.push_back(vertex.w);
        }
        else if (file_line[0] == 'v' && file_line[1] == 't') {
            std::stringstream s(file_line.substr(2));
            float u = 0, v = 0;
            s >> u >> v;

            tmp_vtex2f.push_back(u);
            tmp_vtex2f.push_back(v);
        }
        else if (file_line[0] == 'f') {
            std::stringstream s(file_line.substr(2));
            int indx = 0, texture_ind = 0, trash2 = 0;
            char a1 = 0, a2 = 0;
            
            s >> indx >> a1 >> texture_ind >> a2 >> trash2;
            indices.push_back(indx - 1);
            if (tmp_vtex2f.size() > 0) {
                vtex2f.push_back(tmp_vtex2f[texture_ind - 1]);
            }
            s >> indx >> a1 >> texture_ind >> a2 >> trash2;
            indices.push_back(indx - 1);
            if (tmp_vtex2f.size() > 0) {
                vtex2f.push_back(tmp_vtex2f[texture_ind - 1]);
            }
            s >> indx >> a1 >> texture_ind >> a2 >> trash2;
            indices.push_back(indx - 1);
            if (tmp_vtex2f.size() > 0) {
                vtex2f.push_back(tmp_vtex2f[texture_ind - 1]);
            }
        }
    }

    for (int i = 0; i < all_vpos_4f.size(); ++i) {
        col.push_back(1);
    }

    res.resize(1);

    res[0].instances.resize(1);

    res[0].geom.vpos4f = all_vpos_4f;
    res[0].geom.indices = indices;

    res[0].geom.vcol4f = col;
    res[0].geom.vtex2f = vtex2f;

    return res;
}