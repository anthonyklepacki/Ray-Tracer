//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include <float.h>
#include "vec3.h"
#include "camera.h"
#include "ray.h"
#include "random.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"


vec3 random_in_unit_sphere(){
    vec3 p(2,2,2);
    while (p.squared_length() >= 1.0){
        p = 2.0*vec3(random_double(),random_double(),random_double()) - vec3(1,1,1);
    }
    return p;
}

vec3 color(const ray& r, hitable *world){
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX  , rec)){
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5*color(ray(rec.p, target - rec.p),world);
        
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
    int nx = 200;
    int ny = 100;
    int ns = 100;
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    

    
    hitable *list[2];
    list[0] = new sphere(vec3(0,0,-1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);
    
    camera cam;
    
    // for loop for outputting image information to file
    for (int j = ny-1; j>= 0; j--){
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            
                for (int s = 0; s < ns; s++) {
                    float u = float(i+drand48()) / float(nx);
                    float v = float(j+drand48()) / float(ny);
                    ray r = cam.get_ray(u, v);
                    col += color(r,world);
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
            
            
        }
        
    }
    // write data to jpg image using stb library
    stbi_write_jpg("foo_out.jpg",nx, ny,3,data,100);
}