#pragma once

#include <stack>

namespace ncl {
	namespace ds {
		namespace tree {
			enum TraverseType { IN_ORDER, PRE_ORDER, POST_ORDER };

			template <typename Node, typename Visit>
			void inOrder(Node* node, Visit& visit) {
				std::stack<Node*> stack;
				auto curr = node;

				while (curr != nullptr || !stack.empty()) {
					while (curr != nullptr) {
						stack.push(curr);
						curr = curr->child[0];
					}

					curr = stack.top();
					stack.pop();

					visit(curr);

					curr = curr->child[1];
				}
			}

			template <typename Node, typename Visit>
			void preOrder(Node* node, Visit& visit) {
				if (node != nullptr) {
					visit(node);
					preOrder(node->child[0], visit);
					preOrder(node->child[1], visit);
				}
			}


			template <typename Node, typename Visit>
			void postOrder(Node* node, Visit& visit) {
				if (nullptr == node) return;

				std::function<Node*(std::stack<Node*>)> peek = [](std::stack<Node*> stack) {
					if (stack.empty()) return (Node*)nullptr;
					return stack.top();
				};

				std::function<Node*(std::stack<Node*>)> pop = [](std::stack<Node*> stack) {
					auto n = stack.top();
					stack.pop();
					return n;
				};

				std::stack<Node*> stack;
				Node* curr = node;
				do {

					while (curr) {
						if (nullptr != curr->child[1]) stack.push(curr->child[1]);
						stack.push(curr);
						curr = curr->child[0];
					}

					curr = stack.top();
					stack.pop();

					if (nullptr != curr->child[1] && peek(stack) == curr->child[1]) {
						// traverse right child if not yet traversed
						stack.pop();
						stack.push(curr);
						curr = curr->child[1];
					}
					else {
						visit(curr);
						curr = nullptr;
					}

				} while (!stack.empty());
			}

			template<typename Node, typename Visit>
			inline void traverse(Node node, Visit visit, TraverseType type) {
				switch (type) {
				case IN_ORDER:
					inOrder(node, visit);
					break;
				case PRE_ORDER:
					preOrder(node, visit);
					break;
				case POST_ORDER:
					postOrder(node, visit);
					break;
				}
			}

			template <typename Node>
			inline size_t size(Node* node) {

				struct Counter {
					size_t size;
					void operator()(Node* node) {
						size += node != nullptr ? 1 : 0;
					}
				} counter{};

				inOrder(node, counter);

				return counter.size;
			};

			template <typename Node>
			inline size_t height(Node* node) {
				if (nullptr == node) return 0;
				int lh = height(node->child[0]);
				int rh = height(node->child[1]);

				return glm::max(lh, rh) + 1;
			}

			template<typename Node>
			void dispose(Node* node) {
				std::function<void(Node*)> free = [](Node* n) { delete n;  };
				postOrder(node, free);
			}


			template <typename Node>
			inline Node* min(Node* node) {
				if (node && node->child[0] == nullptr && node->child[1] == nullptr) return node;
				min(node->child[0]);
			}

			template <typename Node>
			inline Node* max(Node* node) {
				if (node && node->child[0] == nullptr && node->child[1] == nullptr) return node;
				min(node->child[1]);
			}

			template<typename T>
			struct Node {
				Node* child[2] = { nullptr, nullptr };
				T v;
			};

			/*
			void test() {
			Node<char> i; i.v = 'I';

			Node<char> d; d.v = 'D';
			Node<char> a; a.v = 'A';
			Node<char> f; f.v = 'F';
			Node<char> h; h.v = 'H';

			d.child[0] = &a; d.child[1] = &f;
			f.child[1] = &h;

			Node<char> l; l.v = 'L';
			Node<char> k; k.v = 'K';
			Node<char> m; m.v = 'M';
			Node<char> p; p.v = 'P';

			l.child[0] = &k; l.child[1] = &m;
			m.child[1] = &p;

			i.child[0] = &d;
			i.child[1] = &l;

			std::function<void(Node<char>*)> print = [](Node<char>* n) { std::cout << n->v << ", ";  };

			std::cout << "in order: ";
			inOrder(&i, print);
			std::cout << std::endl;

			std::cout << "pre order order: ";
			preOrder(&i, print);
			std::cout << std::endl;

			std::cout << "post order: ";
			traverse(&i, print, POST_ORDER);
			std::cout << std::endl;

			std::cout << "minimum: " << min(&i);
			std::cout << "maximum: " << max(&i);
			std::cout << "no of nodes: " << size(&i);
			std::cout << "height of tree: " << height(&i);
			}*/
		}
	}
}