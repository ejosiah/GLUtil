#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace ds {

		using Point = glm::vec2;

		template<typename Data>
		struct Node {
			Point pos;
			Data* data;

            Node() = default;
            Node(glm::vec2 pos, Data* data) :pos{ pos }, data{ data }{

            }
		};


		template<typename Data>
		class quad_tree {
			Point min;
			Point max;

			using Quad = quad_tree<Data>;
           
			Quad* topLeftTree = nullptr;
			Quad* topRightTree = nullptr;
			Quad* botLeftTree = nullptr;
			Quad* botRightTree = nullptr;

			Node<Data>* n;
			float unit;

		public:
            quad_tree(Point min = { 0, 0 }, Point max = { 0, 0 }, float unit = 1.f) 
				:min{ min }, max{ max }, unit{ unit } {
                topLeftTree = nullptr;
                topRightTree = nullptr;
                botLeftTree = nullptr;
                botRightTree = nullptr;
                n = nullptr;

			}

			void insert(Node<Data>* node);
			Node<Data>* search(Point point);
			bool inBoundary(Point point);
		};

		template<typename Data>
		void quad_tree<Data>::insert(Node<Data>* node) {
            if (node == nullptr)
                return;

            // Current quad cannot contain it 
            if (!inBoundary(node->pos))
                return;

            // We are at a quad of unit area 
            // We cannot subdivide this quad further 
            if (abs(min.x - max.x) <= unit &&
                abs(min.y - max.y) <= unit)
            {
                if (n == nullptr)
                    n = node;
                return;
            }

            if ((min.x + max.x) / 2 >= node->pos.x)
            {
                // Indicates topLeftTree 
                if ((min.y + max.y) / 2 >= node->pos.y)
                {
                    if (topLeftTree == nullptr)
                        topLeftTree = new Quad(
                            Point(min.x, min.y),
                            Point((min.x + max.x) / 2,
                                (min.y + max.y) / 2), unit);
                    topLeftTree->insert(node);
                }

                // Indicates botLeftTree 
                else
                {
                    if (botLeftTree == nullptr)
                        botLeftTree = new Quad(
                            Point(min.x,
                                (min.y + max.y) / 2),
                            Point((min.x + max.x) / 2,
                                max.y), unit);
                    botLeftTree->insert(node);
                }
            }
            else
            {
                // Indicates topRightTree 
                if ((min.y + max.y) / 2 >= node->pos.y)
                {
                    if (topRightTree == nullptr)
                        topRightTree = new Quad(
                            Point((min.x + max.x) / 2,
                                min.y),
                            Point(max.x,
                                (min.y + max.y) / 2), unit);
                    topRightTree->insert(node);
                }

                // Indicates botRightTree 
                else
                {
                    if (botRightTree == nullptr)
                        botRightTree = new Quad(
                            Point((min.x + max.x) / 2,
                                (min.y + max.y) / 2),
                            Point(max.x, max.y), unit);
                    botRightTree->insert(node);
                }
            }
		}


		template<typename Data>
        Node<Data>* quad_tree<Data>::search(Point p) {
            // Current quad cannot contain it 
            if (!inBoundary(p))
                return nullptr;

            // We are at a quad of unit length 
            // We cannot subdivide this quad further 
            if (n != nullptr)
                return n;

            if ((min.x + max.x) / 2 >= p.x)
            {
                // Indicates topLeftTree 
                if ((min.y + max.y) / 2 >= p.y)
                {
                    if (topLeftTree == nullptr)
                        return nullptr;
                    return topLeftTree->search(p);
                }

                // Indicates botLeftTree 
                else
                {
                    if (botLeftTree == nullptr)
                        return nullptr;
                    return botLeftTree->search(p);
                }
            }
            else
            {
                // Indicates topRightTree 
                if ((min.y + max.y) / 2 >= p.y)
                {
                    if (topRightTree == nullptr)
                        return nullptr;
                    return topRightTree->search(p);
                }

                // Indicates botRightTree 
                else
                {
                    if (botRightTree == nullptr)
                        return nullptr;
                    return botRightTree->search(p);
                }
            }
		}

		template<typename Data>
		bool quad_tree<Data>::inBoundary(Point p) {
            return (p.x >= min.x &&
                p.x <= max.x &&
                p.y >= min.y &&
                p.y <= max.y);
		}

	}
}