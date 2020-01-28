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

hitable *random_scene() {
    int n = 50000;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000,new lambertian(vec3(random_double()*random_double(),random_double()*random_double(),random_double()*random_double())));
    int i = 1;
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new moving_sphere(center,center+vec3(0, 0.5*random_double(), 0),0.0, 1.0, 0.2,new lambertian(vec3(random_double()*random_double(),random_double()*random_double(),random_double()*random_double())));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(
                        center, 0.2,
                        new metal(
                            vec3(0.5*(1 + random_double()),
                                 0.5*(1 + random_double()),
                                 0.5*(1 + random_double())),
                            0.5*random_double()
                        )
                    );
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
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
    
    hitable *world = random_scene();
    
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;

    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture,dist_to_focus, 0.0, 1.0);
    
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