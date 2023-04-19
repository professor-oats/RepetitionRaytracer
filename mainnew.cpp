#include "rtweekend.h"
#include "camera.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "moving_sphere.h"
#include "constant_medium.h"
#include "bvh.h"

#include "box.h"

#include <iostream>

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) {
    return color(0,0,0);
  }

  // If the ray hits nothing, return the background color.
  if(!world.hit(r, 0.001, infinity, rec)) {
	return background;
  }

  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  double pdf;
  color albedo;

  if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf)) {
    return emitted;
  }

  auto on_light = point3(random_double(213,343), 554, random_double(227,332));
  auto to_light = on_light - rec.p;
  auto distance_squared = to_light.length_squared();
  to_light = unit_vector(to_light);

  if (dot(to_light, rec.normal) < 0) {
    return emitted;
  }

  double light_area = (343-213)*(332-227);
  auto light_cosine = fabs(to_light.y());
    
  if (light_cosine < 0.000001) {
        return emitted;
  }

  pdf = distance_squared / (light_cosine * light_area);
  scattered = ray(rec.p, to_light, r.time());


  return emitted
    + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered)
    * ray_color(scattered, background, world, depth-1) / pdf;
}


hittable_list cornell_box() {
  hittable_list objects;

  auto red = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(15, 15, 15));

  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

  shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  objects.add(box1);

  shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130, 0, 65));
  objects.add(box2);



  return objects;
}

int main() {
  // Image

  auto aspect_ratio = 1.0/1.0;
  int image_width = 400;
  int samples_per_pixel = 100;
  const int max_depth = 100;

  // World
  
  hittable_list world;

  point3 lookfrom;
  point3 lookat;
  auto vfov = 40.0;
  auto aperture = 0.0;
  color background(0, 0, 0);

  world = cornell_box();
	aspect_ratio = 1.0;
	image_width = 600;
	samples_per_pixel = 100;
	background = color(0, 0, 0);
	lookfrom = point3(278, 278, -800);
	lookat = point3(278, 278, 0);
	vfov = 40.0;

// Camera

vec3 vup(0,1,0);
auto dist_to_focus = 10.0;
int image_height = static_cast<int>(image_width / aspect_ratio);

camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  // Render

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height-1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
      for (int i = 0; i < image_width; ++i) {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
          auto u = (i + random_double()) / (image_width-1);
          auto v = (j + random_double()) / (image_height-1);
          ray r = cam.get_ray(u,v);
          pixel_color += ray_color(r, background, world, max_depth);
        }
        write_color(std::cout, pixel_color, samples_per_pixel);
      }
    }

    std::cerr << "\nDone.\n";

}

/* Extras */
/* As long as the weights are positive and add up to one, any such mixture of PDFs is a PDF. Remember, we can use any PDF: all PDFs eventually converge to the correct answer. So, the game is to figure out how to make the PDF larger where the product s(direction)⋅color(direction)
 is large. For diffuse surfaces, this is mainly a matter of guessing where color(direction)
 is high.

For a mirror, s()
 is huge only near one direction, so it matters a lot more. Most renderers in fact make mirrors a special case, and just make the s/p
 implicit — our code currently does that. */