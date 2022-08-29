#ifndef RASTERIZER_TEXTURE_H_
#define RASTERIZER_TEXTURE_H_

#include <vector>
#include <string>
#include <memory>

#include "mathbase.h"


class Texture {
public:
    enum SURROUND {REPEAT, FILLED};
    enum INTERPOLATION {BILINEAR, NEAREST};
    
    /**
     * @brief Construct a new Texture object
     * 
     * @param w 纹理宽
     * @param h 纹理高
     */
    Texture(int w, int h);

    /**
     * @brief 从文件中读取纹理
     * 
     * @param image 文件路径
     * @return std::shared_ptr<Texture> 
     */

    vec4 sample(float u, float v);

    /**
     * @brief 设置填充颜色，只有surround设置为FILLED才有效
     * 
     * @param color 填充颜色
     */
    void set_filled_color(vec4 color);

    /**
     * @brief 设置环绕模式颜色
     * 
     * @param surr 可选：REPEAT，FILLED
     */
    void set_surround(SURROUND surr);

    /**
     * @brief 设置采样模式
     * 
     * @param intp 可选：NEAREST，BILINEAR 
     */
    void set_interpolation(INTERPOLATION intp);
private:
    int w, h;
    std::vector<vec4> data;
    SURROUND surround = FILLED;
    INTERPOLATION interpolation = BILINEAR;
    vec4 filled_color;
    friend std::shared_ptr<Texture> texture_from_file(std::string image);
};

/**
 * @brief 从文件中读取纹理
 * 
 * @param image 文件路径
 * @return std::shared_ptr<Texture> 
 */
std::shared_ptr<Texture> texture_from_file(std::string image);

#endif // RASTERIZER_TEXTURE_H_