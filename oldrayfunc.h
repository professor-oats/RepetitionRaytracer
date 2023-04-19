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
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  double pdf;
  color albedo;

  if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf)) {
    return emitted;
  }

  return emitted
    + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered)
    * ray_color(scattered, background, world, depth-1) / pdf;
}
