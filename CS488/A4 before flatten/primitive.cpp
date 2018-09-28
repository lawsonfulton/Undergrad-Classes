#include "primitive.hpp"
#include "polyroots.hpp"
#include "ray.hpp"

Primitive::~Primitive()
{
}

bool Primitive::rayIntersection(const Ray &ray, double &t, Vector3D &normal) {
	return false;
}

Sphere::~Sphere()
{
}

bool Sphere::rayIntersection(const Ray &ray, double &t, Vector3D &normal) {
	return false;
}

Cube::~Cube()
{
}

bool Cube::rayIntersection(const Ray &ray, double &t, Vector3D &normal) {
	return false;
}

NonhierSphere::~NonhierSphere()
{
}

//TODO idea -- maybe I hsould only return t?
bool NonhierSphere::rayIntersection(const Ray &ray, double &t, Vector3D &normal) {
	Vector3D oMc = ray.origin() - m_pos;

	double a = ray.direction().lengthSquared();
	double b = 2.0 * dot(ray.direction(), oMc);
	double c = oMc.lengthSquared() - m_radius * m_radius;

	double roots[2];
	int numRoots = quadraticRoots(a, b, c, roots);

	if(numRoots == 0) {
		return false;
	}
	else {
		if(numRoots == 2) {
			t = min(roots[0], roots[1]);
		} else {
			t = roots[0];
		}

		Vector3D point = ray.getPoint(t);
		normal = point - m_pos;
		normal.normalize();

		return true;
	} 
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::rayIntersection(const Ray &ray, double &t, Vector3D &normal) {
	return false;
}
