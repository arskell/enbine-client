
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>
#include <memory.h>

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

int main(){
    
    assert(glfwInit());
    glfwSetErrorCallback(error_callback);

    GLFWwindow *wnd = glfwCreateWindow(600, 600, "Scene", nullptr, nullptr);
    assert(wnd);

    std::array<Color, 600*600> image;
    memset(image.data(), 255, image.size() * sizeof(Color));

    glfwMakeContextCurrent(wnd);
    glfwSwapBuffers(wnd);

    int counter  = 0;

    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );

    ViewPort view_port;
    Triangle triangle{
        {-200, 0, 10},{200, 200 , 10},{0, 200, 10}
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
                image[index].r = 255;
                image[index].g = 255;
                image[index].b = 255;
            }
        }
    }

    while(!glfwWindowShouldClose(wnd)){
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels(600, 600, GL_RGB,  GL_UNSIGNED_BYTE, image.data());
        

        //counter = (counter + 1)%image.size();
        glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();   

}