
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>
#include <memory.h>

#include <enbine/graphics/angle/euler_angles.h>

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
    
    EulerAngles a{},b{};
    auto c = a + b;
    
    assert(glfwInit());
    glfwSetErrorCallback(error_callback);

    GLFWwindow *wnd = glfwCreateWindow(500, 600, "Scene", nullptr, nullptr);
    assert(wnd);

    std::array<Color, 600*600> image;
    memset(image.data(), 255, image.size() * sizeof(Color));

    glfwMakeContextCurrent(wnd);
    glfwSwapBuffers(wnd);

    int counter  = 0;

    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );

    while(!glfwWindowShouldClose(wnd)){
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels(600, 600, GL_RGB,  GL_UNSIGNED_BYTE, image.data());
        
        image[counter].r = 0;
        image[counter].g = 0;
        image[counter].b = 0;
        counter = (counter + 1)%image.size();
        glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();   

}