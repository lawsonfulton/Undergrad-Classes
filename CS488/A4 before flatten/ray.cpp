#include "ray.hpp"

Point3D Intersection::getPoint() const {
	return ray->getPoint(t);
}