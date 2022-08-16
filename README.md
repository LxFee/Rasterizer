# MGL - 软光栅器

这是一个学习项目，为了更好的理解渲染流水线。

模仿opengl接口制作的CPU软光栅器。核心代码在src/impl/mgl.cpp中。

通过继承Shader类，重写fragment_shader和vertex_shader来实现可编程shader。

## 如何使用

使用到了SDL2和imgui库。SDL2需要安装运行时环境。所有的库都是在window下编译使用mingw编译器编译，因此只支持window环境。

`make`（或`mingw32-make`）可以编译所有src/demo下的所有示例。`make <文件名>`可以指定编译src/demo中的示例。

## Feature

+ 近平面裁剪
+ 自定义shader
+ 纹理采样支持双线性插值
+ 使用imgui，更加便于调试

## Demo

![image-20220816222610007](https://minioapi.limil.top:9000/images/2022/08/16/image-20220816222610007_repeat_1660659975603__726553.png)



![image-20220816222720100](https://minioapi.limil.top:9000/images/2022/08/16/image-20220816222720100_repeat_1660660045391__775806.png)



## Todo

- [ ] 实现mipmapping
- [ ] 多线程优化运行效率