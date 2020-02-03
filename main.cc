//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "pdf.h"
#include "random.h"
#include "sphere.h"

#include <chrono>
#include "omp.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "surface_texture.h"
#include "texture.h"

#include <float.h>
#include <iostream>

#include<opencv2/opencv.hpp>
using namespace cv;


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

inline vec3 de_nan(const vec3& c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}



vec3 color(const ray& r, hittable *world, hittable *light_shape, int depth) {
	world->rays++;
    hit_record hrec;
    if (world->hit(r, 0.001, FLT_MAX, hrec)) {
        scatter_record srec;
        vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
        if (depth < 5 && hrec.mat_ptr->scatter(r, hrec, srec)) {
            if (srec.is_specular) {
                return srec.attenuation * color(srec.specular_ray, world, light_shape, depth+1);
            }
            else {
                hittable_pdf plight(light_shape, hrec.p);
                mixture_pdf p(&plight, srec.pdf_ptr);
                ray scattered = ray(hrec.p, p.generate(), r.time());
                float pdf_val = p.value(scattered.direction());
                delete srec.pdf_ptr;
                return emitted
                     + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered)
                                        * color(scattered, world, light_shape, depth+1)
                                        / pdf_val;
            }
        }
        else
            return emitted;
    }
    else
        return vec3(0,0,0);
}

void cornell_box(hittable **scene, camera **cam, float aspect) {
    int i = 0;
    hittable **list = new hittable*[8];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    list[i++] = new translate(new rotate_y(
                    new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    //*scene = new hittable_list(list,i);
	*scene = new bvh_node(list, i, 0.0f, 1.0f);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

int main() {
	

    const int nx = 400;
    const int ny = 400;

    //int ns = 1000;
	int index = 0;

	int counter = 0;
	int counterDashes = 50;
	int pixels = nx * ny;

	int nsInit = 1;
	int nsStep = 10;
	int nsMaxSteps = 5;
	int ns = 10000;
	

	static unsigned char data[nx * ny * 3];
	//int *dataArray{ new int[nx][ny][3]};
	//double* a = new double[n]
	//int *dataArray = new int[nx][ny][3];
	static unsigned char outPixel[nx][ny][3];

    //std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hittable *world;
    camera *cam;
    float aspect = float(ny) / float(nx);
    cornell_box(&world, &cam, aspect);
    hittable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
    hittable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
    hittable *a[2];
    a[0] = light_shape;
    a[1] = glass_sphere;
    hittable_list hlist(a,2);

	namedWindow("image", WINDOW_NORMAL);

	// for loop for outputting image information to file

		for (int iterRenderSamp = 0; iterRenderSamp < nsMaxSteps; iterRenderSamp++) {
			if (iterRenderSamp == 0) {
				ns = nsInit;
			}
			else {
				ns *= nsStep;
			}

			for (int j = ny - 1; j >= 0; j--) {
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				for (int i = 0; i < nx; i++) {
					vec3 col(0, 0, 0);
					//#pragma omp parallel for
					for (int s = 0; s < ns; s++) {
						float u = float(i + random_double()) / float(nx);
						float v = float(j + random_double()) / float(ny);
						ray r = cam->get_ray(u, v);
						vec3 p = r.point_at_parameter(2.0);
						col += de_nan(color(r, world, &hlist, 0));
					}
					col /= float(ns);
					col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
					int ir = int(255.99*col[0]);
					int ig = int(255.99*col[1]);
					int ib = int(255.99*col[2]);
					//std::cout << ir << " " << ig << " " << ib << "\n";

					data[index++] = int(255.99*col[0]);
					data[index++] = int(255.99*col[1]);
					data[index++] = int(255.99*col[2]);

					outPixel[ny + 1 - j][i][0] = int(255.99*col[0]);
					outPixel[ny + 1 - j][i][1] = int(255.99*col[1]);
					outPixel[ny + 1 - j][i][2] = int(255.99*col[2]);
					counter++;
				}

				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				int time = int(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
				//std::cout << "Progress:[";
				printf("Progress:[");
				double progress = (double)counter / pixels;
				for (int d = 0; d < counterDashes; d++) {

					if (progress <= (double)d / counterDashes) {
						//std::cout << "*";
						printf("#");
					}
					else {
						printf("-");
					}
					//std::cout << progress << std::endl;
				}
				printf("]Rays/s: %f \n", float(world->rays) / float(time) * 1000);
				world->rays = 0;
				//std::cout << "]  " << progress*100 << "%"<< std::endl;

				Mat out_img = Mat(nx, ny, CV_8UC3, outPixel);
				imshow("image", out_img);
				waitKey(1);
			}
			counter = 0;
			index = 0;
		}
		

	stbi_write_jpg("foo_out.jpg", nx, ny, 3, data, 100);
	waitKey(0);
}

