#include "a4.hpp"
#include "renderer.hpp"

using namespace std;

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  cout << "Rendering: " << filename << "  " << width << " x " << height << endl;
  cout << "Camera Location: " << toString(eye) << " Look at: " << toString(view) << " Up: " << toString(up) << " FOV: " << fov << endl; 

  Camera camera(eye, view, up, fov, width, height);
  Renderer renderer(camera, root, lights, ambient); 

  renderer.renderImage(filename);
}
