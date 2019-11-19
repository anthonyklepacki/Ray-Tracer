//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int main(){
    int x, y, n;

    //Pixel initializations
    int nx = 200;
    int ny = 100;
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    
    // write header for PPM images
    std::cout << "P3\n" << nx << " " << ny << "\n255\n"; 
    
    // for loop for outputting image information to file
    for (int j = ny-1; j>= 0; j--){
        for (int i = 0; i < nx; i++) {
            float r = float(i)/float(nx);
            float g = float(j)/float(ny);
            float b = 0.2;
            
            int ir = int(255.99*r);
            int ig = int(255.99*g);
            int ib = int(255.99*b);
            
            data[index++] = ir;
            data[index++] = ig;
            data[index++] = ib;
        }
    }
    stbi_write_jpg("foo_out.jpg",nx, ny,3,data,100);
}