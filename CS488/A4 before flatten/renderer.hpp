#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <list>

#include "camera.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "algebra.hpp"

using namespace std;

class Renderer {
public:
    Renderer(Camera camera, SceneNode *scene, list<Light*> lights, Colour ambient)
             : mCamera(camera), mScene(scene), mLights(lights), mAmbientColour(ambient) {}
    
    void renderImage(const string &filename);

private:
    Colour traceRay(const Ray &ray, int depth = 0);
    Colour computeColour(const Intersection &intersection, int depth);
    Colour backGroundColour(const Vector3D &u);
    Colour computeLightContribution(const Intersection &i, Light *light);
    bool checkVisibility(const Point3D &a, const Point3D &b);

    Camera mCamera;
    SceneNode *mScene;
    list<Light*> mLights;
    Colour mAmbientColour;
};

#endif
