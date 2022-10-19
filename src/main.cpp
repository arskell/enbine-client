
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>
#include <memory.h>
#include <math.h>

#include "enbine/graphics/scene/view_port/view_port.h"
#include "enbine/graphics/scene/primitives/triangle.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

struct Color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} __attribute__((packed));


void render(std::array<Color, 600*600>& image, float tick){
    ViewPortInfo info{};
    
    info.screen_horizontal_size = info.screen_vertical_size = 600;
    info.horizontal_fow = info.vertical_fow = M_PI_2;
    
    info.beta = M_PI_2;
   // info.alpha = 0;
    
    ViewPort view_port(info);
    

    Triangle triangle{
        {-200, 0 - 100, 300},{200, 200 - 100, 300},{0, 200 - 100, 300 /*+ tick*/}
    };

    for(int x = 0; x < 600; ++x){
        for(int y = 0; y < 600; ++y){
            auto index = x + y*600;

            auto ray = view_port.get_ray(x, y);

            auto state = calc_intersection(triangle, ray);

            if(state.is_intersected){
                
                image[index].r = 0;
                image[index].g = 0;
                image[index].b = 0;

            }else{

                image[index].r =  100 + ray.d.x1 * 25;
                image[index].g =  100 + ray.d.x2 * 25;
                image[index].b =  100 + ray.d.x3 * 25;
 
            }
        }
    }

}

int main(){
    
    assert(glfwInit());
    glfwSetErrorCallback(error_callback);

    GLFWwindow *wnd = glfwCreateWindow(600, 600, "Scene", nullptr, nullptr);
    assert(wnd);

    std::array<Color, 600*600> image;
    memset(image.data(), 255, image.size() * sizeof(Color));

    glfwMakeContextCurrent(wnd);
    glfwSwapBuffers(wnd);


    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );

    
    float tick = 0;

    while(!glfwWindowShouldClose(wnd)){
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        render(image, tick);

        glDrawPixels(600, 600, GL_RGB,  GL_UNSIGNED_BYTE, image.data());
        
        glfwSwapBuffers(wnd);
        tick += 0.1;
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();   

}