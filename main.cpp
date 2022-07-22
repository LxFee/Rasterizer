/*
 * @Author: limil gravitylmlml@gmail.com
 * @Description: SDL_demo
 */

#include <iostream>
#include<SDL2/SDL.h>
using namespace std;
int main(int argc, char* argv[])
{
    ::SDL_Init(SDL_INIT_VIDEO);//初始化SDL
    ::SDL_Window* window = ::SDL_CreateWindow("SDL test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);//创建窗体
    ::SDL_Delay(3000);//延时显示窗体
    ::SDL_DestroyWindow(window);//销毁窗体
    ::SDL_Quit();//退出SDL
    getchar();
    return 0;
}