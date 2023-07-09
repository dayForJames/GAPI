// #include "Vec4.h"

uniform struct Vec4 {
    float x, y, z, w;
};

uniform struct Vec2 {
    float x, y;
};

export uniform float getX(uniform struct Vec2 * uniform obj)
{
    return obj->x;
}

export uniform float getY(uniform struct Vec2 * uniform obj)
{
    return obj->y;
}

uniform struct Vec2 
operator- (uniform struct Vec2 * uniform A, uniform struct Vec2 * uniform B)
{
    uniform struct Vec2 res;

    res.x = A->x;
    res.y = A->y;

    res.x -= B->x;
    res.y -= B->y;

    return res;
}

export uniform float 
E(uniform struct Vec2 * uniform V, uniform struct Vec2 * uniform VP)
{
  return (V->x * VP->y - V->y * VP->x);
}

export void
colorShader(const uniform float col[3][4], uniform struct Vec4 * uniform BarCord, uniform struct Vec4 * uniform r)
{
    uniform float<3> color;

    foreach (i = 0 ... 3) {
        color[i] = (col[2][i] * BarCord->x + col[0][i] * BarCord->y + col[1][i] * BarCord->z) / BarCord->w;
    }

    r->x = 255 * color.x;
    r->y = 255 * color.y;
    r->z = 255 * color.z;
    r->w = 0;
}

export void
vertexShader(const uniform float rotatedWorldViewProj[16], const uniform float vpos4f_i[4], uniform struct Vec4 * uniform r)
{
    uniform float<4> vNDS_i;

    foreach (i = 0 ... 4) {
        vNDS_i[i] = 0;
        for (uniform int j = 0; j < 4; j++) {
            vNDS_i[i] += rotatedWorldViewProj[i * 4 + j] * vpos4f_i[j];
        }
    } 

    uniform float w = vNDS_i[3];

    r->x = vNDS_i[0] / w;
    r->y = vNDS_i[1] / w;
    r->z = vNDS_i[2] / w;
    r->w = w;
}

export void
ambientLightShader(const uniform struct Vec4 * uniform ObjColor, const uniform struct Vec4 * uniform LightSourceColor, 
                    uniform struct Vec4 * uniform Color)
{
    uniform float ambientStrength = 0.1f;

    Color->x = ObjColor->x * LightSourceColor->x * ambientStrength;
    Color->y = ObjColor->y * LightSourceColor->y * ambientStrength;
    Color->z = ObjColor->z * LightSourceColor->z * ambientStrength;
}

export void
 diffusalLightShader(const uniform struct Vec4 * uniform ObjColor, const uniform struct Vec4 * uniform LightColor, 
                    const uniform struct Vec4 * uniform LightDir, 
                    const uniform struct Vec4 * uniform Norm, uniform struct Vec4 * uniform r)
{
    uniform float diff = (LightDir->x * Norm->x + LightDir->y * Norm->y + LightDir->z * Norm->z > 0) ? 
                        LightDir->x * Norm->x + LightDir->y * Norm->y + LightDir->z * Norm->z : 0;

    r->x = diff * LightColor->x * ObjColor->x;
    r->y = diff * LightColor->y * ObjColor->y;
    r->z = diff * LightColor->z * ObjColor->z;
}

export void
colorCubeLightSourceShader(const uniform float col[3][4], uniform struct Vec4 * uniform BarCord, uniform struct Vec4 * uniform r)
{
    r->x = 255;
    r->y = 255;
    r->z = 255;
}

export void
CubeStartColorShader(const uniform float col[3][4], uniform struct Vec4 * uniform BarCord, uniform struct Vec4 * uniform r) 
{
    r->x = 255;
    r->y = 0;
    r->z = 0;
}

export void
colorBarShader(const uniform float col[3][4], uniform struct Vec4 * uniform BarCord, uniform struct Vec4 * uniform r)
{
    r->x = 255 * BarCord->x;
    r->y = 255 * BarCord->y;
    r->z = 255 * BarCord->z;
}

export void
textureShader(const uniform uint data[], const uniform uint * uniform width, const uniform uint * uniform height, const uniform float uv[3][2], 
                const uniform struct Vec4 * uniform BarCord, uniform uint * uniform color)
{
    uniform uint64 s = (*width) * (*height);

    uniform int32<2> coords;

    foreach (i = 0 ... 2) {
        coords[i] = (BarCord->x * uv[2][i] + BarCord->y * uv[0][i] + BarCord->z * uv[1][i]) / BarCord->w * (*width);
    }

    uniform uint32 texture_color = data[((*width) * coords[1] + coords[0]) % s];
    uniform uint32 red = (texture_color & 0x00FF0000) >> 16;
    uniform uint32 green = (texture_color & 0x0000FF00) >> 8;
    uniform uint32 blue = texture_color & 0x00000FF;
    
    *color = ((red) << 16) | 
                    ((green) << 8) | ((blue));
}