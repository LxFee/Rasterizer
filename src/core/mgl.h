#ifndef RASTERIZER_MGL_H_
#define RASTERIZER_MGL_H_

#include "mathbase.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

class Shader;

#define MGL_COLOR (1 << 0)
#define MGL_DEPTH (1 << 1)


/**
 * @brief MGL初始化
 * 
 * @param title 窗口标题
 * @param w 视口宽
 * @param h 视口高
 */
void mgl_init(const char *title, int w, int h);

/**
 * @brief 退出程序
 * 
 */
void mgl_quit();

/**
 * @brief 绘制
 * 
 * @param vbo_ind VBO编号
 * @param ebo_ind EBO编号，当为-1时，按照0,1,2...顺序绘制
 * @param shader shader指针
 */
void mgl_draw(int vbo_ind, int ebo_ind, Shader *shader);

/**
 * @brief 设置深度缓冲初始值
 * 
 * @param depth 
 */
void mgl_clear_depth(float depth);

/**
 * @brief 设置颜色缓冲初始值
 * 
 * @param col 
 */
void mgl_clear_color(vec4 col);

/**
 * @brief 初始化缓冲
 * 
 * @param flag 可选：MGL_COLOR、MGL_DEPTH
 */
void mgl_clear(int flag);

/**
 * @brief 将当前帧更新到屏幕上
 * 
 * @return true 程序退出事件触发
 * @return false 程序退出事件未触发
 */
bool mgl_update();

/**
 * @brief 创建VBO
 * 
 * @param size 顶点属性的总大小，单位：字节
 * @param data 顶点属性数据
 * @param count 顶点个数
 * @return int VBO编号
 */
int mgl_create_vbo(int size, const void *data, int count);

/**
 * @brief 创建EBO
 * 
 * @param data 索引数据，单位：int（4字节）
 * @param count 索引个数
 * @return int EBO编号
 */
int mgl_create_ebo(const int *data, int count);

/**
 * @brief 设置顶点属性格式
 * 
 * @param vbo_ind VBO编号
 * @param location 该属性的位置，从0开始
 * @param size 该属性的大小，单位：float（4字节）
 * @param offset 该属性的偏移，单位：float（4字节）
 * @return int 失败返回-1，否则返回vbo_ind
 */
int mgl_vertex_attrib_pointer(int vbo_ind, int location, int size, int offset);

namespace mgltexture {
    enum SURROUND {REPEAT, FILLED};
    enum INTERPOLATION {BILINEAR, NEAREST};   
}

/**
 * @brief 生成一个四通道float类型的纹理
 * 
 * 
 * @param w 纹理宽
 * @param h 纹理高
 * @return texture 编号
 */
int mgl_gen_texture(int w, int h);


/**
 * @brief 生成一个四通道float类型的图像纹理
 * 
 * @param w 纹理宽
 * @param h 纹理高
 * @param wide 字长，单位4字节。只支持wide为3或4
 * @param data 
 * @return int 
 */


int mgl_gen_texture_image(int w, int h, int wide, const unsigned char* data);

/**
 * @brief 激活纹理
 * 
 * @param texture_id 纹理编号 
 * @param texture_location 纹理激活位置（非编号）
 */
void mgl_active_texture(int texture_id, int texture_location);

/**
 * @brief 采样纹理
 * 
 * @param texture_location 纹理激活位置（非编号），在mgl_active_texture()中的设置
 * @param u 
 * @param v 
 * @return vec4 
 */
const vec4 mgl_texture_sample2d(int texture_location, float u, float v);

/**
 * @brief 设置纹理属性（参数）
 * 
 * @param texture_id 纹理编号 
 * @param surr 环绕模式，可选：mgltexture::REPEAT，mgltexture::FILLED
 * @param intp 插值模式，可选：mgltexture::BILINEAR，mgltexture::NEAREST
 */
void mgl_texture_parameteri(int texture_id, mgltexture::SURROUND surr, mgltexture::INTERPOLATION intp);

/**
 * @brief 设置纹理属性（值）
 * 
 * @param texture_id 纹理编号
 * @param border_color 设置环绕模式为mgltexture::FILLED时采样超范围时的填充颜色
 */
void mgl_texture_parameterv(int texture_id, vec4 filled_color);

#endif // RASTERIZER_MGL_H_