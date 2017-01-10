#pragma once

#include "lab_vertex_format.hpp"

#include <vector>
#include <iostream>

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


class Polygon;

class Plane {
	public:
		Plane();
		Plane(lab::VertexFormat &P1, lab::VertexFormat &P2,
				lab::VertexFormat &P3);
		glm::vec3 getNormal();
		void flip();
		void splitPolygon(Polygon polygon, std::vector<Polygon> &coplanarFront,
			std::vector<Polygon> &coplanarBack, std::vector<Polygon> &frontFace,
			std::vector<Polygon> &backFace);

	private:
		double A, B, C, D;
};


class Polygon {
	public:
		Polygon();
		Polygon(std::vector<lab::VertexFormat> points);
		std::vector<lab::VertexFormat> getPoints();
		Plane getPlane();
		void flip();
	private:
		std::vector<lab::VertexFormat> points;
		Plane plane;
};

Plane::Plane() {}

Plane::Plane(lab::VertexFormat &P1, lab::VertexFormat &P2,
		lab::VertexFormat &P3) {
	// Construct the plane equation from 3 points P1, P2 and P3.
	//
	// The plane equation is:
	// A(x - x0) + B(y - y0) + C(z - z0) = 0
	// or
	// Ax + By + Cz = D
	//
	// We first need to get the normal of the plane (cross product of
	// two vectors in the plane) and then get the coordinates A, B, C
	// and D.

	glm::vec3 V1 = P3.get_position() - P1.get_position();
	glm::vec3 V2 = P2.get_position() - P1.get_position();
	glm::vec3 normal = glm::normalize(glm::cross(V1, V2));

	this->A = normal.x;
	this->B = normal.y;
	this->C = normal.z;
	this->D = glm::dot(normal, P1.get_position());
}

glm::vec3 Plane::getNormal() {
	return glm::vec3(A, B, C);
}

void Plane::flip() {
	A *= -1;
	B *= -1;
	C *= -1;
	D *= -1;
}

void Plane::splitPolygon(Polygon polygon, std::vector<Polygon> &coplanarFront,
	std::vector<Polygon> &coplanarBack, std::vector<Polygon> &frontFace,
	std::vector<Polygon> &backFace) {

	int COPLANAR = 0;
	int FRONT = 1;
	int BACK = 2;
	int SPANNING = 3;

	int polygonType = 0;
	std::vector<lab::VertexFormat> points = polygon.getPoints();
	std::vector<int> types;

	for (unsigned int i = 0; i < points.size(); i++) {
		glm::vec3 normal = getNormal();
		double t = glm::dot(normal, points[i].get_position()) - D;

		int type;
		if (t < -0.0001) {
			type = BACK;
		} else if (t > 0.0001) {
			type = FRONT;
		} else {
			type = COPLANAR;
		}

		polygonType |= type;
		types.push_back(type);
	}

	if (polygonType == COPLANAR) {
		glm::vec3 normal = getNormal();
		glm::vec3 polygonNormal = polygon.getPlane().getNormal();
		double k = glm::dot(normal, polygonNormal);

		if (k > 0) {
			coplanarFront.push_back(polygon);
		} else {
			coplanarBack.push_back(polygon);
		}
	}

	if (polygonType == FRONT) {
		frontFace.push_back(polygon);
	}

	if (polygonType == BACK) {
		backFace.push_back(polygon);
	}

	if (polygonType == SPANNING) {

		std::vector<lab::VertexFormat> f;
		std::vector<lab::VertexFormat> b;

		for (unsigned int i = 0; i < points.size(); i++) {
			int j = (i + 1) % points.size();
			int ti = types[i];
			int tj = types[j];

			auto vi = points[i];
			auto vj = points[j];
			if (ti != BACK) f.push_back(vi);
			if (ti != FRONT) b.push_back(vi);

			if ( (ti | tj) == SPANNING) {
				glm::vec3 normal = getNormal();
				double t = (D - glm::dot(normal, vi.get_position())) / (
				glm::dot(normal, vj.get_position() - vi.get_position()));
				auto v = vi.interpolate(vj, t);
				//auto v = vi;

				f.push_back(v);
				b.push_back(v);
			}
		}

		if (f.size() >= 3) frontFace.push_back(Polygon(f));
		if (b.size() >= 3) backFace.push_back(Polygon(b));
	}
}

Polygon::Polygon(){}

Polygon::Polygon(std::vector<lab::VertexFormat> points) {

	if (points.size() < 3) {
		std::cout << "A polygon should have at least 3 vertices!";
		return;
	}

	this->points = points;
	this->plane = Plane(points[0], points[1], points[2]);
}

void Polygon::flip() {
	std::reverse (this->points.begin(), this->points.end());
	for (auto &point : this->points) {
		point.flip();
	}
	plane.flip();
}

Plane Polygon::getPlane() {
	return plane;
}

std::vector<lab::VertexFormat> Polygon::getPoints() {
	return points;
}

//};


//class Node {
//	public:
//		std::vector<Polygon> coplanarPolygons;
//}
//
//class BSP_Tree {
//
//	public:
//
//		BSP_Tree(std::vector<Polygon> &polygons) {
//			std::copy(polygons.begin(), polygons.end(), this->polygons().begin());
//		}
//
//		build() {
//			build_aux(0);
//		}
//
//	private:
//
//		build_aux(int node, std::vector<Polygon> &polygons) {
//
//			if (polygons.size() == 0) {
//				return;
//			}
//
//			if (node > bsp_tree.size() - 1) {
//				bsp_tree.resize(node + 1);
//			}
//
//			auto plane = polygons[0].getPlane();
//
//			std::vector<Polygon> frontFace;
//			std::vector<Polygon> backFace;
//			std::vector<Polygon> coplanarFace;
//
//			for (std::vector<Polygon> polygon : polygons) {
//				plane.splitPolygon(polygon, bsp_tree[node].coplanarPolygons,
//						bsp_tree[node].coplanarPolygons, frontFace, backFace);
//			}
//
//			build_aux(2 * node + 1, frontFace);
//			build_aux(2 * node + 2, backFace);
//		}
//
//
//		std::vector<Node> bsp_tree
//		std::vector<Polygon> polygons;
//};
//

class Node {
	public:

		Node(std::vector<Polygon> polygons) {
			this->initialized = false;
			this->front = NULL;
			this->back = NULL;
			if (polygons.size()) build(polygons);
		}

		void build(std::vector<Polygon> polygons) {
			if (polygons.size() == 0) return;
			if (this->initialized == false) {
				this->initialized = true;
				this->plane = polygons[0].getPlane();
			}

			std::vector<Polygon> f;
			std::vector<Polygon> b;

			for (auto polygon : polygons) {
				this->plane.splitPolygon(polygon, this->polygons, this->polygons, f, b);
			}
			std::cout << this->polygons.size() << " " << f.size() << ' ' << b.size() << std::endl;

			if (f.size() > 0) {
				if (this->front == NULL) this->front = new Node(std::vector<Polygon>{});
				this->front->build(f);
			}

			if (b.size() > 0) {
				if (this->back == NULL) this->back = new Node(std::vector<Polygon>{});
				this->back->build(b);
			}
		}

		void invert() {
			for (auto &polygon : this->polygons) {
				polygon.flip();
			}
			this->plane.flip();

			if (this->front != NULL) this->front->invert();
			if (this->back != NULL) this->back->invert();

			Node* aux = this->front;
			this->front = this->back;
			this->back = aux;	
		}

		std::vector<Polygon> clipPolygons(std::vector<Polygon> polygons) {
			if (initialized == false) {
				return polygons;
			}

			std::vector<Polygon> f, b;
			for (auto &polygon : polygons) {
				this->plane.splitPolygon(polygon, f, b, f, b);
			}

			if (this->front != NULL) {
				f = this->front->clipPolygons(f);
			}

			if (this->back != NULL) {
				b = this->back->clipPolygons(b);
			} else {
				b.clear();
			}
			
			for (auto p: b) {
				f.push_back (p);
			}

			return f;
		}

		void clipTo(Node* bsp) {
			this->polygons = bsp->clipPolygons(this->polygons);
			
			if (this->front != NULL) {
				this->front->clipTo(bsp);
			}

			if (this->back != NULL) {
				this->back->clipTo(bsp);
			}
		}

		std::vector<Polygon> allPolygons() {
			std::vector<Polygon> result = this->polygons;

			if (this->front != NULL) {
				std::vector<Polygon> part_result = this->front->allPolygons();
				for (Polygon p : part_result) {
					result.push_back(p);
				}
			}

			if (this->back != NULL) {
				std::vector<Polygon> part_result = this->back->allPolygons();
				for (Polygon p : part_result) {
					result.push_back(p);
				}
			}

			return result;
		}

	private:
			bool initialized;
			Plane plane;
			Node *front;
			Node *back;
			std::vector<Polygon> polygons;
};

class Object {
	public:
		Node* bsp_tree;
		
		Object(std::vector<Polygon> polygons) {
			bsp_tree = new Node(polygons);
		}

		Object* subtract(Object *other) {
			std::vector<Polygon> V1 = this->bsp_tree->allPolygons();
			std::vector<Polygon> V2 = other->bsp_tree->allPolygons();

			Node* A = new Node(V1);
			Node* B = new Node(V2);

			A->invert();
			A->clipTo(B);
			B->clipTo(A);
			B->invert();
			B->clipTo(A);
			B->invert();
			A->build(B->allPolygons());
			A->invert();

			std::vector<Polygon> resultingPolygons = A->allPolygons();
			return new Object(resultingPolygons);
		}

		std::vector<Polygon> get_polygons() {
			return bsp_tree->allPolygons();
		}
};

