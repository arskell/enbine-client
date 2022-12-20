
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <array>
#include <memory.h>
#include <memory>

#include <math.h>
#include <algorithm>
#include <mutex>
#include "enbine/graphics/scene/view_port/view_port.h"
#include "enbine/graphics/scene/primitives/triangle.h"
#include "enbine/graphics/scene/scene.h"
#include "enbine/graphics/scene/light/point_light.h"
#include "enbine/graphics/scene/light/point_directional_light.h"

#include "enbine/graphics/material/metal.h"

#include "enbine/graphics/scene/primitives/square.h"
#include <chrono>
#include <thread>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

struct Color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} __attribute__((packed));


ViewPort view_port;
Scene scene;

template<size_t Size>
void do_post_processing_linear(const std::array<LightComponentT, Size>& in, std::array<Color, Size>& out){

    static std::array<float, Size> in_dist;

    float mean {};
    for(size_t i = 0; i < in.size(); ++i){
        in_dist[i] = (in[i].x1 + in[i].x2 + in[i].x3)/3;
        mean+=in_dist[i]/Size;
    }

    float sigma_2{};

    for(size_t i  = 0; i < in.size(); ++i){
        sigma_2 += pow(in_dist[i] - mean, 2);
    }
    sigma_2/=Size;
    auto sigma = sqrt(sigma_2);
    
    const auto k = 200/(mean + sigma * 4);
  
    for(size_t i  = 0; i < in.size(); ++i){
        out[i].r = round(std::min(k * in[i].x1, 255.0f));
        out[i].g = round(std::min(k * in[i].x2, 255.0f));
        out[i].b = round(std::min(k * in[i].x3, 255.0f));
    }

}

void render(std::array<Color, 600*600>& image, float tick){
    
    static std::array<LightComponentT, 600 * 600> raw_image;
    auto start =  std::chrono::high_resolution_clock::now();

    for(int x = 0; x < 600; ++x){
        for(int y = 0; y < 600; ++y){
            auto index = x + y*600;

            auto ray = view_port.get_ray(x, y);

            auto state = scene.get_light(ray);

            raw_image[index].x1 = state.x1;
            raw_image[index].x2 = state.x2;
            raw_image[index].x3 = state.x3;

        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //std::cout<<dur.count()<< "ms" <<std::endl;

    do_post_processing_linear(raw_image, image);

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{   
    
    float alpha_rotate_delta{};
    float beta_rotate_delta{};
    float pos_delta{};
    switch(key){
        case GLFW_KEY_UP:
        beta_rotate_delta+=0.1;    
        break;
        case GLFW_KEY_DOWN:
        beta_rotate_delta-=0.1;    
        break;
        case GLFW_KEY_RIGHT:
        alpha_rotate_delta-=0.1;    
        break;
        case GLFW_KEY_LEFT:
        alpha_rotate_delta+=0.1;    
        break;
        case GLFW_KEY_W:
        pos_delta+=5;    
        break;
        case GLFW_KEY_S:
        pos_delta-=5;    
        break;
        default:;
    }

    auto info = view_port.get_info();
    info.alpha+=alpha_rotate_delta;
    info.beta+=beta_rotate_delta;
    info.pos = info.pos + view_port.get_look_at() * pos_delta;
    view_port.set(info);

}

int main(){
    
    ViewPortInfo info{};
    
    auto default_metal = std::make_shared<MetalMaterial>(RGB{0.5f,0.5f,0.5f});

    auto default_metal_2 = std::make_shared<MetalMaterial>(RGB{0.05f,0.5f,0.05f});

    auto triangle = std::make_shared<Triangle>(Triangle{
        {-500, 0 - 100, 300},{500, 200 - 100, 300},{0, 500 - 100, 300}, default_metal
    });


    auto floor = std::make_shared<Square>( Vec3{-3000, 10, -3000}, Vec3{1,0,0}, Vec3{0,0,1}, 6000.0f, 6000.0f, default_metal);


    
    auto wall_1 = std::make_shared<Square>( Vec3{-10, 10, 25}, Vec3{0.707f, 0, 0.707f}, Vec3{0,-1,0}, 10.0f, 2.0f, default_metal_2);
    auto wall_2 = std::make_shared<Square>( Vec3{-10, 6, 25}, Vec3{0.707f, 0, 0.707f}, Vec3{0,-1,0}, 10.0f, 2.0f, default_metal_2);
    auto wall_3 = std::make_shared<Square>( Vec3{-10, 2, 25}, Vec3{0.707f, 0, 0.707f}, Vec3{0,-1,0}, 10.0f, 2.0f, default_metal_2);


    auto square_front = std::make_shared<Square>( Vec3{-50, -50, 50}, Vec3{1,0,0}, Vec3{0,1,0}, 50.0f, 50.0f, default_metal_2);


    auto light = std::make_shared<PointLight>();
    light->set_flux({1, 1, 1});
    light->set_position({0, 0, 22});
    light->set_n({0,1,1});

    auto light_2 = std::make_shared<PointLight>();
    light_2->set_flux({1, 1, 1});
    light_2->set_position({0, 0, 0});
    light_2->set_n({0,1,0});

    scene.add("floor", floor);
    scene.add("wall_1", wall_1);
    scene.add("wall_2", wall_2);
    scene.add("wall_3", wall_3);
 //   scene.add("triangle_2", square_front);

    scene.add("point light", light);
    //scene.add("point light _2", light_2);


    info.screen_horizontal_size = info.screen_vertical_size = 600;
    info.horizontal_fow = info.vertical_fow = M_PI_2;
    
    info.pos = {-10,3,5};

    info.beta = M_PI_2;
    
   // info.alpha = 0;
    
    view_port.set(info);

    assert(glfwInit());
    glfwSetErrorCallback(error_callback);

    GLFWwindow *wnd = glfwCreateWindow(600, 600, "Scene", nullptr, nullptr);
    assert(wnd);
    
    glfwSetKeyCallback(wnd, key_callback);

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