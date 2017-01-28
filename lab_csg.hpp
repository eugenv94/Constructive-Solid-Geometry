#pragma once

#include "lab_vertex_format.hpp"

#include <vector>
#include <iostream>
#include <stack>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include "dependente\glew\glew.h"
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#else
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

#define EPSILON 0.001

struct Plane {
	// The plane equation is:
	// A(x - x0) + B(y - y0) + C(z - z0) = 0
	// or
	// Ax + By + Cz = D
	double A, B, C, D;
};

Plane getPlaneFromPoints(lab::VertexFormat &P1, lab::VertexFormat &P2,
		lab::VertexFormat &P3) {
	// Construct the plane equation from 3 points P1, P2 and P3.
	//
	// We first need to get the normal of the plane (cross product of
	// two vectors in the plane) and then get the coordinates A, B, C
	// and D.

	glm::vec3 V1 = P2.get_position() - P1.get_position();
	glm::vec3 V2 = P3.get_position() - P1.get_position();
	glm::vec3 normal = glm::normalize(glm::cross(V1, V2));

	Plane p;
	p.A = normal.x;
	p.B = normal.y;
	p.C = normal.z;
	p.D = glm::dot(normal, P1.get_position());
	
	return p;
}

class Polygon {
	public:
		std::vector<lab::VertexFormat> points;

		Polygon() {}

		Polygon(std::vector<lab::VertexFormat> points) {
			if (points.size() < 3) {
				std::cout << "A polygon should have at least 3 vertices!";
				return;
			}
			this->points = points;
		}
};

Plane getPlaneFromPolygon(Polygon p) {
	// Construct a plane from a poligon. All vertices on this polygon should be
	// on the same plane.

	auto points = p.points;
	if (points.size() < 3) {
		std::cout << "A poligon should have at least 3 points to calculate its plane";
	}

	return getPlaneFromPoints(points[0], points[1], points[2]);
}

double distancePointPlane(Plane &plane, glm::vec3 point) {
	glm::vec3 planeNormal = glm::vec3(plane.A, plane.B, plane.C);
	return glm::dot(planeNormal, point) - plane.D;
}


// Split the polygon by a given plane. We will save the parts of the polygon
// that are on the front side, on the back side and the coplanar once. We will
// also add new points by interpolating if needed.
void splitByPlane(Plane &plane, Polygon polygon, std::vector<Polygon> &coplanar,
		std::vector<Polygon> &frontFace, std::vector<Polygon> &backFace) {

	std::vector<lab::VertexFormat> points = polygon.points;
	glm::vec3 planeNormal = glm::vec3(plane.A, plane.B, plane.C);

	std::vector<double> distances;
	for (int i = 0; i < points.size(); i++) {
		distances.push_back(distancePointPlane(plane,
					points[i].get_position()));
	}
 
	// check if all distances are within a range (so the polygon is coplanar).
	bool ok = true;
	for (const auto dist : distances) {
		if (dist < -EPSILON || dist > EPSILON) {
			ok = false;
		}
	}
	if (ok) {
		coplanar.push_back(polygon);
		return;
	}

	// check if all polygons are in the front side of the plane.
	ok = true;
	for (const auto dist : distances) {
		if (dist > EPSILON) {
			ok = false;
		}
	}
	if (ok) {
		backFace.push_back(polygon);
		return;
	}

	// check if all polygons are in the back side of the plane.
	ok = true;
	for (const auto dist : distances) {
		if (dist < -EPSILON) {
			ok = false;
		}
	}
	if (ok) {
		frontFace.push_back(polygon);
		return;
	}

	// we need to split the polygon into 2.
	std::vector<lab::VertexFormat> frontAux;
	std::vector<lab::VertexFormat> backAux;
	for (int i = 0; i < points.size(); i++) {
		int nextPoint = (i + 1) % points.size();

		if (distances[i] > -EPSILON) frontAux.push_back(points[i]);
		if (distances[i] < EPSILON) backAux.push_back(points[i]);

		if ( (distances[i] < -EPSILON && distances[nextPoint] > EPSILON) ||
				(distances[i] > EPSILON && distances[nextPoint] < -EPSILON)) {

			float alfa = (plane.D - glm::dot(planeNormal,
						points[i].get_position())) / (glm::dot(planeNormal,
						points[nextPoint].get_position() -
						points[i].get_position()));
			auto newPoint = points[i].interpolate(points[nextPoint], alfa);

			// This new point will be both on the front side and on the back
			// side.
			frontAux.push_back(newPoint);
			backAux.push_back(newPoint);
		}
	}

	if (frontAux.size() >= 3) frontFace.push_back(Polygon(frontAux));
	if (backAux.size() >= 3) backFace.push_back(Polygon(backAux));
}

