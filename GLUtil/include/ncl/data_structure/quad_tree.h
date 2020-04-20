#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace ds {

		template<typename Data>
		struct Node {
			glm::vec2 pos;
			Data* data;

            Node() = default;
            Node(glm::vec2 pos, Data* data) :pos{ pos }, data{ data }{

            }
		};


		template<typename Data>
		class quad_tree {
			glm::vec2 topLeft;
			glm::vec2 botRight;

			using Quad = quad_tree<Data>;
            using Point = glm::vec2;

            Quad* topLeftTree = nullptr;
            Quad* topRightTree = nullptr;
            Quad* botLeftTree = nullptr;
            Quad* botRightTree = nullptr;

			Node<Data>* n;

		public:
            quad_tree(Point topLeft = { 0, 0 }, Point botRight = { 0, 0 }) :topLeft{ topLeft }, botRight{ botRight } {
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
            if (abs(topLeft.x - botRight.x) <= 1 &&
                abs(topLeft.y - botRight.y) <= 1)
            {
                if (n == nullptr)
                    n = node;
                return;
            }

            if ((topLeft.x + botRight.x) / 2 >= node->pos.x)
            {
                // Indicates topLeftTree 
                if ((topLeft.y + botRight.y) / 2 >= node->pos.y)
                {
                    if (topLeftTree == nullptr)
                        topLeftTree = new Quad(
                            Point(topLeft.x, topLeft.y),
                            Point((topLeft.x + botRight.x) / 2,
                                (topLeft.y + botRight.y) / 2));
                    topLeftTree->insert(node);
                }

                // Indicates botLeftTree 
                else
                {
                    if (botLeftTree == nullptr)
                        botLeftTree = new Quad(
                            Point(topLeft.x,
                                (topLeft.y + botRight.y) / 2),
                            Point((topLeft.x + botRight.x) / 2,
                                botRight.y));
                    botLeftTree->insert(node);
                }
            }
            else
            {
                // Indicates topRightTree 
                if ((topLeft.y + botRight.y) / 2 >= node->pos.y)
                {
                    if (topRightTree == nullptr)
                        topRightTree = new Quad(
                            Point((topLeft.x + botRight.x) / 2,
                                topLeft.y),
                            Point(botRight.x,
                                (topLeft.y + botRight.y) / 2));
                    topRightTree->insert(node);
                }

                // Indicates botRightTree 
                else
                {
                    if (botRightTree == nullptr)
                        botRightTree = new Quad(
                            Point((topLeft.x + botRight.x) / 2,
                                (topLeft.y + botRight.y) / 2),
                            Point(botRight.x, botRight.y));
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

            if ((topLeft.x + botRight.x) / 2 >= p.x)
            {
                // Indicates topLeftTree 
                if ((topLeft.y + botRight.y) / 2 >= p.y)
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
                if ((topLeft.y + botRight.y) / 2 >= p.y)
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
            return (p.x >= topLeft.x &&
                p.x <= botRight.x &&
                p.y >= topLeft.y &&
                p.y <= botRight.y);
		}

	}
}