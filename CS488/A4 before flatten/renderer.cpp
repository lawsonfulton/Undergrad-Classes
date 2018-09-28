#include <iomanip>
#include <list>

#include "renderer.hpp"
#include "image.hpp"
#include "material.hpp"
#include "scene.hpp"

static inline void loadbar(unsigned int x, unsigned int n, unsigned int w = 50)
{
	//Source: https://www.ross.click/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app/
    if ( (x != n) && (x % (n/100+1) != 0) ) return;
 
    float ratio  =  x/(float)n;
    int   c      =  ratio * w;
 
    cout << setw(3) << (int)(ratio*100) << "% [";
    for (int x=0; x<c; x++) cout << "=";
    for (int x=c; x<(int)w; x++) cout << " ";
    cout << "]\r" << flush;
}


void Renderer::renderImage(const string &filename) {
	Image img(mCamera.width(), mCamera.height(), 3);

	int totalPixels = mCamera.width() * mCamera.height();
	int currentPixel = 1;
	cout << endl;
	for (int y = 0; y < mCamera.height(); y++) {
		for (int x = 0; x < mCamera.width(); x++) {

			Ray primaryRay = mCamera.makeRay(x,y);
			Colour colour = traceRay(primaryRay, 0);

			img.setColour(x, y, colour);

			loadbar(currentPixel++, totalPixels);
		}
	}
	cout << endl;
	img.savePng(filename);
}

Colour Renderer::traceRay(const Ray &ray, int depth) {
	Intersection closestI;
	closestI.ray = &ray;

	//For all of the nodes in the scene's first level
	const list<SceneNode*> *nodes = mScene->getChildren();
	for (auto nodeIt = nodes->begin(); nodeIt != nodes->end(); ++nodeIt) {
		if((*nodeIt)->is_geometry()){
			GeometryNode *node = (GeometryNode*) (*nodeIt);

			Intersection newI;
			bool intersects = node->computeIntersection(ray, newI);

			if(intersects) {
				if(newI < closestI) {
					closestI = newI;
				}
			}
		}
	}

	return computeColour(closestI, depth);
}

//TODO
//- shadows
//- parameterize background
//- figure out reversed image

Colour Renderer::computeColour(const Intersection &i, int depth) {
	if(i.t == DBL_INF) {
		return backGroundColour(i.ray->direction());
	}

	Colour finalColour = mAmbientColour;
	for (auto lightIt = mLights.begin(); lightIt != mLights.end(); ++lightIt) {
		Light* light = *lightIt;
		finalColour += computeLightContribution(i, light);
	}

	// //Reflection component
	// if(depth < MAX_RECURSION_DEPTH) {
	// 	Ray reflectedRay(i.getPoint(), reflect(-i.ray->direction(), i.normal)); //TODO - Should I preturb using the normal?
	// 	finalColour += traceRay(reflectedRay, depth + 1) * 0.2;
	// }

	return finalColour;	
}

Colour Renderer::backGroundColour(const Vector3D &u) {
	//Sky sphere
	double theta = asin(u.y());
	double t = (theta + M_PI_2)/M_PI;

	Colour horizon(0.8,0.8,0.9);
	Colour bottom(1,1,1);
	Colour top(0,0,1);

	if(t > 0.5) {
		return lin_interpolate(horizon, top, (t - 0.5) * 2.0);
	} else {
		return lin_interpolate(bottom, horizon, t * 2.0);
	}
}

Colour Renderer::computeLightContribution(const Intersection &i, Light *light) {
	Vector3D point = i.getPoint();

	//Check shadow
	bool isVisible = checkVisibility(point, light->position);
	if(!isVisible) {
		return Colour(0.0);
	}

	Vector3D lightVec = light->position - point;
	Vector3D L = (light->position - point).normalized();
	Vector3D R = reflect(L, i.normal);
	Vector3D V = -i.ray->direction();

	PhongMaterial *material = (PhongMaterial*)(i.node->get_material()); //TODO how to handle mutliple materials

	Colour kd = material->getDiffuse();
	Colour ks = material->getSpecular();
	double shininess = material->getShininess();

	double diffuseFact = qMax(dot(L, i.normal), 0.0f);
	double specularFact = qMax(dot(R, V), 0.0f);
	Colour surfaceColour = kd * diffuseFact + ks * pow(specularFact, shininess);

	//Attenuation / Falloff TODO performance check
	double attenuation = 1.0;
	double denom = 0.0;
	double r2 = lightVec.lengthSquared();
	if(light->falloff[0] > MY_EPSILON) {
		denom += light->falloff[0];
	}
	if(light->falloff[1] > MY_EPSILON) {
		denom += light->falloff[1] * sqrt(r2);
	}
	if(light->falloff[2] > MY_EPSILON) {
		denom += light->falloff[2] * r2;
	}
	if(denom > MY_EPSILON) {
		attenuation = 1.0/denom;
	}

	return surfaceColour * light->colour * attenuation;
}


bool Renderer::checkVisibility(const Point3D &a, const Point3D &b) {
	Ray shadowRay(a, (b-a).normalized());

	const list<SceneNode*> *nodes = mScene->getChildren();
	for (auto nodeIt = nodes->begin(); nodeIt != nodes->end(); ++nodeIt) {
		if((*nodeIt)->is_geometry()){
			GeometryNode *newNode = (GeometryNode*) (*nodeIt);

			Intersection newI;
			bool intersects = newNode->computeIntersection(shadowRay, newI);

			if(intersects && newI.t > MIN_INTERSECT_DIST) {
				return false;
			}
		}
	}

	return true;
}





