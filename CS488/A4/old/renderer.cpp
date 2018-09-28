#include <iomanip>
#include <list>
#include <thread>

#include "renderer.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "options.hpp"


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
	cout << endl;//formattting
	Image img(mCamera.width(), mCamera.height(), 3);

	//Flatten the DAG
	mGeometryList = mScene->getFlattened();

	vector<thread> threads;
	atomic_int pixelCounter(0);
	atomic_int currentSlice(0); 
	int totalSlices = 8 * NUM_THREADS;
	cout << "Number of Threads: " << NUM_THREADS << endl;
	cout << "Number of Image Slices: " << totalSlices << endl;

	for (int i = 0; i < NUM_THREADS; i++) {
		threads.push_back(thread(&Renderer::renderSlicesThread,
								 this, std::ref(img), totalSlices, std::ref(currentSlice), std::ref(pixelCounter)));
	}

	for (auto& th : threads) th.join();

	img.savePng(filename);
	cout << endl;//formatting
}

void Renderer::renderSlicesThread(Image &img, int totalSlices, atomic_int &currentSlice, atomic_int &pixelCounter) {
	int slice;

	while((slice = currentSlice++) < totalSlices) {
		renderSlice(img, slice, totalSlices, pixelCounter);
	}
}

void Renderer::renderSlice(Image &img, int slice, int totalSlices, atomic_int &pixelCounter) {
	int ssLevel = SUPER_SAMPLE_LEVEL;
	double numSamples = (double)(ssLevel * ssLevel);
	double spacing = 1.0/(double)ssLevel;

	int totalPixels = mCamera.width() * mCamera.height();

	int sliceWidth = mCamera.width() / totalSlices;
	int start = slice * sliceWidth;
	int end = start + sliceWidth;

	//Get any extra pixels
	if(slice == totalSlices - 1) {
		end += mCamera.width() % totalSlices;
	}

	PhongMaterial sourceMaterial(Colour(0.0), Colour(0.0), 0, 1.0); //Air
	for (int y = 0; y < mCamera.height(); y++) {
		for (int x = start; x < end; x++) {

			Colour finalColour(0.0);
			for(int yi = 0; yi < ssLevel; yi++) {
				for(int xi = 0; xi < ssLevel; xi++) {
					Ray primaryRay = mCamera.makeRay(Point2D((double)x + xi * spacing, (double)y + yi * spacing));
					Colour colour = traceRay(primaryRay, 0, (Material*)&sourceMaterial);

					finalColour += colour / numSamples;
				}
			}
			img.setColour(x, y, finalColour);
			
			//Update the loading bar
			pixelCounter++;
			loadbar(pixelCounter, totalPixels);
		}
	}
}

Colour Renderer::traceRay(const Ray &ray, int depth, Material *sourceMaterial) {
	Intersection closest = findClosestIntersection(ray);
	closest.sourceMaterial = sourceMaterial;

	return computeColour(closest, depth);
}

Intersection Renderer::findClosestIntersection(const Ray &ray) {
	Intersection closestI;
	closestI.ray = &ray;

	for (auto nodeIt = mGeometryList.begin(); nodeIt != mGeometryList.end(); ++nodeIt) {
		Intersection newI;
		bool intersects = nodeIt->computeIntersection(ray, newI); //TODO should I perturb here? //ray.perturbed(MY_EPSILON)

		if(intersects) {
			if(newI.t < closestI.t && newI.t > MIN_INTERSECT_DIST) {
				closestI = newI;
			}
		}
	}

	return closestI;
}

Colour Renderer::computeColour(const Intersection &i, int depth) {
	if(i.t == DBL_INF) {
		return backGroundColour(i.ray->direction());
	}

	Colour finalColour = mAmbientColour * ((PhongMaterial*)(i.node->get_material()))->getDiffuse();
	for (auto lightIt = mLights.begin(); lightIt != mLights.end(); ++lightIt) {
		Light* light = *lightIt;
		finalColour += computeLightContribution(i, light);
	}

	//Reflection component
	
	if(depth < MAX_RECURSION_DEPTH) {
		#if USE_REFLECTIONS
		finalColour += computeReflectedContribution(i, depth);
		#endif
		#if USE_REFRACTIONS
		finalColour += computeRefractionContribution(i, depth); 
		#endif
	}
	

	return finalColour;	
}

Colour Renderer::backGroundColour(const Vector3D &u) {
	//Sky sphere
	double theta = asin(u.y());
	double t = (theta + M_PI_2)/M_PI;

	Colour horizon(1,1,1);///(0.8,0.8,0.9);
	Colour bottom(1,1,1);
	Colour top(0,0,1);

	if(t > 0.5) {
		return lin_interpolate(horizon, top, clamp((t - 0.5) * 8.0, 0.0, 1.0));
	} else {
		return lin_interpolate(bottom, horizon, t * 2.0);
	}
}

Colour Renderer::computeLightContribution(const Intersection &i, Light *light) {
	Vector3D point = i.getPoint();
	PhongMaterial *material = (PhongMaterial*)(i.node->get_material()); //TODO how to handle mutliple materials

	//Check shadow
	bool isVisible = checkVisibility(point, light->position);
	if(!isVisible) {
		return Colour(0.0);
	}

	Vector3D lightVec = light->position - point;
	Vector3D L = (light->position - point).normalized();
	Vector3D R = reflect(L, i.normal);
	Vector3D V = -i.ray->direction();

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

	return surfaceColour * light->colour * attenuation * (1.0 - material->getReflectivity())* (1.0 - material->getTransparency());
}

Colour Renderer::computeReflectedContribution(const Intersection &i, int depth) {
	PhongMaterial *material = (PhongMaterial*)(i.node->get_material()); //TODO how to handle mutliple materials
	double reflectivity = material->getReflectivity();

	if(reflectivity > MY_EPSILON) {
		Ray reflectedRay(i.getPoint(), reflect(-i.ray->direction(), i.normal)); //TODO - Should I preturb using the normal?
		return traceRay(reflectedRay.perturbed(0.01), depth + 1, i.node->get_material()) * reflectivity * material->getDiffuse();// * (1.0 - material->getTransparency()); //TODO should Ihave this term at the end?
	} else {
		return Colour(0.0);
	}
}

Colour Renderer::computeRefractionContribution(const Intersection &i, int depth) {
	//http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
	PhongMaterial *material = (PhongMaterial*)(i.node->get_material()); //TODO how to handle mutliple materials
	PhongMaterial *sourceMaterial = (PhongMaterial*)(i.sourceMaterial);
	double transparency = material->getTransparency();

	if(transparency > MY_EPSILON) {
		double n1 = sourceMaterial->getIor(), n2 = material->getIor();

		//If same source and current, then assume going into air
		if(material == sourceMaterial){
			n2 = 1.0;
		}

		const Vector3D &l = i.ray->direction();
		double n = n1/n2;
		double c = -dot(i.normal, l);
		double sinsq = n * n * (1.0 - c * c);

		if(n1 < n2 && sinsq > 1.0){
			return Colour(0.0);
		}

		Vector3D refr = n * l + (n * c - c) * i.normal;
		Ray refrRay(i.getPoint(), refr); //TODO - Should I preturb using the normal?material->getTransparency();

		return traceRay(refrRay, depth + 1, i.node->get_material()) * transparency * material->getDiffuse(); //TODO properly * material->getDiffuse();
	}

	return Colour(0.0);
}

//TODO should I also preturb here? Should I preturb in find closest intersection?
// or is it already taken care of with the MIN_DIST component
bool Renderer::checkVisibility(const Point3D &a, const Point3D &b) {
	double dist = (b-a).length();

	Ray shadowRay(a, (b-a)/dist);
	Intersection i = findClosestIntersection(shadowRay.perturbed(0.01)); //TODO this perturbment is a bit of a hack, should really be doing proer calculation of t in world space

	if(i.t == DBL_INF) {
		return true;
	} else {
		return i.t > dist; //Return true if intersection is past the light
	}
}



// void Renderer::renderImage(const string &filename) {
// 	Image img(mCamera.width(), mCamera.height(), 3);

// 	//Flatten the DAG
// 	mGeometryList = mScene->getFlattened();

// 	int ssLevel = SUPER_SAMPLE_LEVEL;
// 	double numSamples = (double)(ssLevel * ssLevel);
// 	double spacing = 1.0/(double)ssLevel;

// 	int totalPixels = mCamera.width() * mCamera.height();
// 	int currentPixel = 1;
// 	cout << endl;
// 	for (int y = 0; y < mCamera.height(); y++) {
// 		for (int x = 0; x < mCamera.width(); x++) {

// 			Colour finalColour(0.0);
// 			for(int yi = 0; yi < ssLevel; yi++) {
// 				for(int xi = 0; xi < ssLevel; xi++) {
// 					Ray primaryRay = mCamera.makeRay(Point2D((double)x + xi * spacing, (double)y + yi * spacing));
// 					Colour colour = traceRay(primaryRay, 0);

// 					finalColour += colour / numSamples;
// 				}
// 			}
			

// 			img.setColour(x, y, finalColour);

// 			loadbar(currentPixel++, totalPixels);
// 		}
// 	}
// 	cout << endl;

// 	cout << "Colour correcting..." << endl;
// 	//img.gammaCorrect(1.0);
// 	//img.normalize();
// 	img.savePng(filename);
// }
