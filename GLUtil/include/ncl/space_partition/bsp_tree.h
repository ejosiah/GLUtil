#pragma once
#include <iterator>
#include <vector>
#include <functional>
#include <stack>
#include "../geom/Plane.h"

namespace ncl {
	namespace space_partition {

		template<typename T>
		class BSPTree {
		public:
			enum Type { NODE_STORING, LEAF_STORING, SOLID_LEAF };

			enum Classification { COPLANAR, IN_FRONT, BEHIND, STRADDLING};
			struct Node;

			struct BSPObjectSet {
				T* objects;
				size_t size;

				BSPObjectSet() :objects{ nullptr }, size{ 0 } {

				}

				BSPObjectSet(T* objects, size_t size) {
					this->objects = new T[size];
					for (size_t i = 0; i < size; i++) {
						this->objects[i] = objects[i];
					}
					this->size = size;
				}
			};
			using PickSplittingPlane = std::function<geom::Plane(BSPObjectSet)>;
			using ClassifyObjectToPlane = std::function<Classification(T, geom::Plane)>;

			class InOrderIterator : public std::iterator<std::forward_iterator_tag, T> {
			public:
				InOrderIterator(Node* node, bool rootNode = true, int idx = 0);
				InOrderIterator(const InOrderIterator& copy);

				~InOrderIterator() {
				}

				InOrderIterator& operator++();
				InOrderIterator operator++(int);
				bool operator==(const InOrderIterator& rhs) const;
				bool operator!=(const InOrderIterator& rhs) const;
				InOrderIterator& operator=(const InOrderIterator& copy);
				T& operator*();


			private:
				Node * node;
				int currIdx;
				std::stack<Node*> stack;
			};

			~BSPTree() {
				// TODO
			}

			InOrderIterator inOrderIterator() {
				return InOrderIterator(_root, true);
			}

			InOrderIterator inOrderEnd() {
				return InOrderIterator(nullptr, false);
			}
			
			static BSPTree<T> create(T* objects, size_t objects_size, PickSplittingPlane pickSplittingPlane, ClassifyObjectToPlane classifyObjectToPlane, size_t max_depth, size_t max_leaf_size, Type type = LEAF_STORING);

			Node& root() {
				return *_root;
			}

		private:
			struct Node {

				struct Child {
					Node* parent;
					Node* operator[](const int& index) {
						assert(parent);
						assert(index >= 0 && index <= 1);
						if (index == 0) return parent->_front;
						return parent->_back;
					}
				};

				geom::Plane _plane;
				Node* _front;
				Node* _back;
				BSPObjectSet _objects;
				Child child;

				Node(geom::Plane p, Node* f, Node* b, BSPObjectSet objs)
					:_plane{ p }, _front{ f }, _back{ b }, _objects{objs}, child{this}{}

				Node(geom::Plane p, Node* f, Node* b, std::vector<T> objs)
					:_plane{ p }, _front{ f }, _back{ b }, _objects{ }, child{ this } {
					if (!objs.empty()) {
						_objects.objects = new T[objs.size()];
						_objects.size = objs.size();
						for (size_t i = 0; i < _objects.size; i++) {
							_objects.objects[i] = objs[i];
						}
					}
				}

				Node(geom::Plane p, Node* f, Node* b) :Node{ p, f, b, {} }{}

				Node(BSPObjectSet objs) : Node{ Plane::Null(), nullptr, nullptr, objs }{}

				Node* front() const {
					return _front;
				}

				Node* back() const {
					return _back;
				}

				geom::Plane plane() const {
					return _plane;
				};

				BSPObjectSet objects() const {
					return _objects;
				}

				bool isLeaf() const {
					return _front == nullptr && _back == nullptr && _objects.size != 0;
				}
			};
			Node* _root;
			Type _type;

			BSPTree(Node* root, Type type = LEAF_STORING) :
				_root(root), _type{ type }
			{}

		};
		
		template<typename T>
		BSPTree<T> BSPTree<T>::create(T* objects, size_t objects_size, typename BSPTree<T>::PickSplittingPlane pickSplittingPlane, typename BSPTree<T>::ClassifyObjectToPlane classifyObjectToPlane, size_t max_depth, size_t min_leaf_size, typename BSPTree<T>::Type type) {
			assert(min_leaf_size > 0);
			assert(max_depth > 0);

			std::function<BSPTree<T>::Node*(T*, size_t, size_t)> buildTree = [&](T* objects, size_t objects_size, size_t depth) {	// TODO auto
				if (objects_size == 0) {
					return (BSPTree<T>::Node*)nullptr;
				}

				auto objectSet = BSPObjectSet{ objects, objects_size };

				if (depth >= max_depth || objects_size <= min_leaf_size) {
					return new BSPTree<T>::Node{ objectSet };
				}

				Plane splitPlane = pickSplittingPlane(objectSet);
				auto frontSet = std::vector<T>{};
				auto backSet =  std::vector<T>{};
				auto coplanar = std::vector<T>{};

				for (size_t i = 0; i < objects_size; i++) {
					T object = objects[i];
					auto classification = classifyObjectToPlane(object, splitPlane);
					switch (classification) {
					case COPLANAR:
						if (type == BSPTree<T>::Type::NODE_STORING) {
							coplanar.push_back(object);
							break;
						}
						// fall through to store in front
					case IN_FRONT:
						frontSet.push_back(object);
						break;
					case BEHIND:
						backSet.push_back(object);
						break;
					case STRADDLING:
						// TODO
						break;
					}
				}
				
				auto frontNode = buildTree(&frontSet[0], frontSet.size(), depth + 1);
				auto backNode = buildTree(&backSet[0], backSet.size(), depth + 1);

				return new BSPTree<T>::Node{ splitPlane, frontNode, backNode, coplanar };
			};

			return BSPTree<T>(buildTree(objects, objects_size, 0), type);
		}

		template<typename T>
		BSPTree<T>::InOrderIterator::InOrderIterator(Node* node, bool rootNode, int idx) :
		node{ node }, currIdx{ idx } {
			if(rootNode){
				while (this->node) {
					stack.push(this->node);
					this->node = this->node->child[0];
				}
				this->node = stack.top();
				stack.pop();
			}
		}

		template<typename T>
		BSPTree<T>::InOrderIterator::InOrderIterator(typename const BSPTree<T>::InOrderIterator& copy):
			BSPTree<T>::InOrderIterator(copy.node, false, copy.currIdx){
			this->stack = copy.stack;
		}


		template<typename T>
		typename BSPTree<T>::InOrderIterator& BSPTree<T>::InOrderIterator::operator++() {
			if ((currIdx+1) < node->objects().size) {
				currIdx++;
			}
			else {
				currIdx = 0;
				node = node->child[1];
				if (node || !stack.empty()) {
					while (node) {
						stack.push(node);
						node = node->child[0];
					}
					node = stack.top();
					stack.pop();
				}
			}
			return *this;
		};

		template<typename T>
		typename BSPTree<T>::InOrderIterator BSPTree<T>::InOrderIterator::operator++(int) {
			BSPTree<T>::InOrderIterator temp(*this, false);
			operator++();
			return temp;
		};

		template<typename T>
		typename BSPTree<T>::InOrderIterator& BSPTree<T>::InOrderIterator::operator=(typename const BSPTree<T>::InOrderIterator& copy) {
			if (this != = &copy) {
				this->currIdx = copy.currIdx;
				this->node = copy.node;
				this->stack = copy.stack;
			}
			return *this;
		}

		template<typename T>
		bool BSPTree<T>::InOrderIterator::operator==(typename const BSPTree<T>::InOrderIterator& rhs) const {
			return this->node == rhs.node;
		}

		template<typename T>
		bool BSPTree<T>::InOrderIterator::operator!=(typename const BSPTree<T>::InOrderIterator& rhs) const {
			return this->node != rhs.node;
		}

		template<typename T>
		T& BSPTree<T>::InOrderIterator::operator*() {
			return node->objects().objects[currIdx];
		};


	}
}