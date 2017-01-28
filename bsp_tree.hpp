#pragma once

#include "lab_csg.hpp"

// Forward declaration for the swap function.
class BSPNode;

void swapBSPs(BSPNode *&A, BSPNode *&B) {
	// Swap two binary space partitioning tree nodes.
	auto aux = A;
	A = B;
	B = aux;
}

// This is a binary space partitioning tree. Every object in out environment
// will have one coresponding BSP tree. Every node will have an assigned plane
// which will split the space into two halves (front and back half). It will
// recursively keep all polygons that construct the object.
class BSPNode {
	private:
			bool initialized;
			int splittingIndex;
			Plane plane;
			BSPNode *left;
			BSPNode *right;
			std::vector<Polygon> coplanarPolygons;

	public:

		BSPNode(std::vector<Polygon> polygons) {
			// Construct a node in the binary space partitioning tree with the
			// given polygons. It will pick a random polygon and consider it as
			// the dividing plane. By using this 
			
			// If we have no polygons, we can stop the algorithm.
			if (polygons.size() == 0) {
				this->left = NULL;
				this->right = NULL;
				this->splittingIndex = -1;
				this->initialized = false;
				return;
			}
			this->initialized = true;

			// We pick a random plane to split the polygons. This is better than
			// choosing the first point or another fixed point because it would
			// have a better distribution in the tree.
			this->splittingIndex = rand() % polygons.size();
			this->plane = getPlaneFromPolygon(polygons[this->splittingIndex]);

			std::vector<Polygon> coplanar;
			std::vector<Polygon> front;
			std::vector<Polygon> back;

			for (auto polygon : polygons) {
				splitByPlane(this->plane, polygon, coplanar, front, back);
			}
			this->coplanarPolygons = coplanar;

			if (front.size() == 0) {
				this->left = NULL;
			} else {
				this->left = new BSPNode(front);
			}

			if (back.size() == 0) {
				this->right = NULL;
			} else {
				this->right = new BSPNode(back);
			}
		}

		void appendPolygons(std::vector<Polygon> polygons) {
			// Add some new polygons to the already existing tree. These will be
			// added considering the already existing split (at least for the
			// nodes that are already computed and have a plane assigned) and
			// for the rest we will calculate another splitting plane that will
			// split the rest.

			if (polygons.size() == 0) return;
			if (this->splittingIndex == -1) {
				// If we haven't splitted this plane before, we need to split it
				// now. We will do this by picking another random polygon from
				// the list and split after it.
				this->splittingIndex = rand() % polygons.size();
				this->plane = getPlaneFromPolygon(polygons[this->splittingIndex]);
			}
			this->initialized = true;

			std::vector<Polygon> coplanar;
			std::vector<Polygon> front;
			std::vector<Polygon> back;

			for (auto polygon : polygons) {
				splitByPlane(this->plane, polygon, coplanar, front, back);
			}
			this->coplanarPolygons.insert(this->coplanarPolygons.end(),
					std::make_move_iterator(coplanar.begin()),
					std::make_move_iterator(coplanar.end()));

			// If the left node isn't initialized we initialize it by passing
			// the nodes that it needs to take care of. Otherwise we continue
			// the appending process.
			if (this->left == NULL) {
				this->left = new BSPNode(front);
			} else {
				this->left->appendPolygons(front);
			}

			// Do the same for the right node.
			if (this->right == NULL) {
				this->right = new BSPNode(back);
			} else {
				this->right->appendPolygons(back);
			}
		}

		void constructComplement() {
			// For our operations, we need to be able to get the complement of
			// a binary space partitioning tree. This is, everything that is
			// not contained in the BSP.

			for (auto &polygon : this->coplanarPolygons) {
				// We need to reverse every point from this polygon.
				std::reverse (polygon.points.begin(), polygon.points.end());
				for (auto &point : polygon.points) {
					point.normal_x *= -1;
					point.normal_y *= -1;
					point.normal_z *= -1;
				}
			}

			// Reverse the plane as well.
			this->plane.A *= -1;
			this->plane.B *= -1;
			this->plane.C *= -1;
			this->plane.D *= -1;

			if (this->left != NULL) this->left->constructComplement();
			if (this->right != NULL) this->right->constructComplement();

			swapBSPs(this->left, this->right);
		}


		void subtract(BSPNode* bsp) {
			// Subtract a bsp tree from another one.

			this->coplanarPolygons = bsp->subtractAux(this->coplanarPolygons);
			if (this->left != NULL) {
				this->left->subtract(bsp);
			}
			if (this->right != NULL) {
				this->right->subtract(bsp);
			}
		}

		std::vector<Polygon> getPolygons() {
			// Get all polygons for this tree recursively.

			std::vector<Polygon> result = this->coplanarPolygons;
			if (this->left != NULL) {
				auto part_result = this->left->getPolygons();
				result.insert(result.end(),
						std::make_move_iterator(part_result.begin()),
						std::make_move_iterator(part_result.end()));
			}

			if (this->right != NULL) {
				auto part_result = this->right->getPolygons();
				result.insert(result.end(),
						std::make_move_iterator(part_result.begin()),
						std::make_move_iterator(part_result.end()));
			}

			return result;
		}


	private:
		std::vector<Polygon> subtractAux(std::vector<Polygon> polygons) {
			// Perform the actual subtraction. This tree will lose everything
			// from polygons.

			std::vector<Polygon> result;

			std::vector<Polygon> front, back;
			for (auto polygon : polygons) {
				splitByPlane(this->plane, polygon, front, front, back);
			}

			if (this->left != NULL) {
				front = this->left->subtractAux(front);
			}
			result.insert(result.end(), std::make_move_iterator(front.begin()),
					std::make_move_iterator(front.end()));

			if (this->right != NULL) {
				back = this->right->subtractAux(back);
				result.insert(result.end(), std::make_move_iterator(
							back.begin()), std::make_move_iterator(back.end()));
			}
			
			return result;
		}
};

// Object class for keeping a primitive. It contains a bsp tree that will
// split the polygon by planes.
class Object {
	public:
		BSPNode* bsp_tree;
		
		Object (std::vector<Polygon> polygons) {
			bsp_tree = new BSPNode(polygons);
		}


		Object* subtract(Object *other) {
			std::vector<Polygon> V1 = this->bsp_tree->getPolygons();
			std::vector<Polygon> V2 = other->bsp_tree->getPolygons();
			BSPNode* A = new BSPNode(V1);
			BSPNode* B = new BSPNode(V2);
			A->subtract(B);
			std::vector<Polygon> resultingPolygons = A->getPolygons();
			return new Object(resultingPolygons);
		}


		Object* combine(Object *other) {
			std::vector<Polygon> V1 = this->bsp_tree->getPolygons();
			std::vector<Polygon> V2 = other->bsp_tree->getPolygons();
			BSPNode* A = new BSPNode(V1);
			BSPNode* B = new BSPNode(V2);
			A->subtract(B);
			B->subtract(A);
			A->appendPolygons(B->getPolygons());
			std::vector<Polygon> resultingPolygons = A->getPolygons();
			return new Object(resultingPolygons);
		}

		Object* intersect(Object *other) {
			std::vector<Polygon> V1 = this->bsp_tree->getPolygons();
			std::vector<Polygon> V2 = other->bsp_tree->getPolygons();
			BSPNode* A = new BSPNode(V1);
			BSPNode* B = new BSPNode(V2);
			A->constructComplement();
			B->subtract(A);
			std::vector<Polygon> resultingPolygons = B->getPolygons();
			return new Object(resultingPolygons);
		}

		std::vector<Polygon> get_polygons() {
			return bsp_tree->getPolygons();
		}
};

