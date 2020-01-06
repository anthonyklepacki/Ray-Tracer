//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>
#include "sphere.h"
#include "triangle.h"
#include "hitable_list.h"
#include <float.h>
#include "vec3.h"
#include "camera.h"
#include "ray.h"
#include "random.h"
#include "material.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"




vec3 color(const ray& r, hitable *world, int depth){
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
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



int main(){
    int x, y, n;

    //Pixel initializations
    int scaler = 1;
    int nx = scaler*200;
    int ny = scaler*100;
    int ns = 100;
    int pixels = nx*ny;
    
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    
    int counter = 0;
    int counterDashes = 50;

    
    hitable *list[5];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.5));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new triangle(vec3(1,-0.5,-1),vec3(-1,-0.5,-1),vec3(0,-0.5,-0.5), new metal(vec3(0.5, 0.8, 0.25),0.0));
    hitable *world = new hitable_list(list,5);
    
    vec3 lookfrom(3,3,2);
    vec3 lookat(0,0,-1);
    float dist_to_focus = (lookfrom-lookat).length();
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), 0.0, dist_to_focus);
    
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
            
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            
            data[index++] = ir;
            data[index++] = ig;
            data[index++] = ib;
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