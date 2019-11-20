//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>
#include "vec3.h"
#include "ray.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

bool hit_sphere(const vec3& center, float radius, const ray&r){
    vec3 oc = r.origin() - center;
    
    float a = dot(r.direction(),r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc)-radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant>0);
}

vec3 color(const ray& r){
    if (hit_sphere(vec3(0,0,-1),0.5, r)){
        return vec3(1,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
}

int main(){
    int x, y, n;

    //Pixel initializations
    int nx = 200;
    int ny = 100;
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    
    vec3 lower_left_corner(-2.0,-1.0,-1.0);
    vec3 horizontal(4.0,0.0,0.0);
    vec3 vertical(0.0,2.0,0.0);
    vec3 origin(0.0,0.0,0.0);
    
    // for loop for outputting image information to file
    for (int j = ny-1; j>= 0; j--){
        for (int i = 0; i < nx; i++) {
            
            float u = float(i)/float(nx);
            float v = float(j)/float(ny);

            ray r(origin, lower_left_corner + (u*horizontal) + (v*vertical));
            vec3 colorAtRay = color(r);
            
            int ir = int(255.99*colorAtRay[0]);
            int ig = int(255.99*colorAtRay[1]);
            int ib = int(255.99*colorAtRay[2]);
            
            data[index++] = ir;
            data[index++] = ig;
            data[index++] = ib;
        }
    }
    // write data to jpg image using stb library
    stbi_write_jpg("foo_out.jpg",nx, ny,3,data,100);
}