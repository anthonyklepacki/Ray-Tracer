//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>
#include "sphere.h"
#include "moving_sphere.h"
#include "triangle.h"
#include "hitable_list.h"
#include <float.h>
#include "vec3.h"
#include "camera.h"
#include "ray.h"
#include "random.h"
#include "material.h"
#include "aabb.h"
#include "bvh.h"
#include "perlin.h"
#include "surface_texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"




vec3 color(const ray& r, hitable *world, int depth){
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 5 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation;
        }
        else {
            return vec3(0,0,0);
        }
    }else{
        //std::cout << "background" << std::endl;
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
    }
}


    hitable *two_perlin_spheres() {
        texture *pertext = new noise_texture(4);
        hitable **list = new hitable*[2];
        list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
        list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
        return new hitable_list(list, 2);
    }
    
    hitable *earth() {
        int nx1, ny1, nn1;
        //unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
        unsigned char *tex_data = stbi_load("earthmap.jpg", &nx1, &ny1, &nn1, 0);
        material *mat =  new lambertian(new image_texture(tex_data, nx1, ny1));
        return new sphere(vec3(0,0, 0), 2, mat);
    }


int main(){
    int x, y, n;

    //Pixel initializations
    int scaler = 2;
    int nx = scaler*200;
    int ny = scaler*100;
    int ns = 25;
    int pixels = nx*ny;
    
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    
    int counter = 0;
    int counterDashes = 50;

    /*
    hitable *list[5];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.5));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new triangle(vec3(1,-0.5,-1),vec3(-1,-0.5,-1),vec3(0,-0.5,-0.5), new metal(vec3(0.5, 0.8, 0.25),0.0));
    hitable *world = new hitable_list(list,5);
    */
    
    hitable *world = earth();
    

    
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
    
    // for loop for outputting image information to file
    for (int j = ny-1; j>= 0; j--){
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            
                for (int s = 0; s < ns; s++) {
                    float u = float(i+drand48()) / float(nx);
                    float v = float(j+drand48()) / float(ny);
                    ray r = cam.get_ray(u, v);
                    col += color(r,world, 0);
                    //std::cout << col << std::endl;
                }
                col /= float(ns);
                //Perform gamma correction
                col = vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
            
            
            data[index++] = int(255.99*col[0]);
            data[index++] = int(255.99*col[1]);
            data[index++] = int(255.99*col[2]);
            counter++;
        }
        

        //std::cout << "Progress:[";
        printf("Progress:[");
        double progress = (double)counter/pixels;
        for (int d = 0; d < counterDashes; d++){
            
            if (progress <= (double)d/counterDashes){
                //std::cout << "*";
                printf("#");
            }else{
                printf("-");
            }
            //std::cout << progress << std::endl;
        }
        printf("] \n");
        //std::cout << "]  " << progress*100 << "%"<< std::endl;
    }
    // write data to jpg image using stb library
    stbi_write_jpg("foo_out.jpg",nx, ny,3,data,100);
}