#pragma once
#include "DSCEngine/types/stack.hpp"
#include "DSCEngine/types/templates.hpp"
#include "DSCEngine/types/comparers.hpp"
#include "DSCEngine/debug/assert.hpp"

namespace DSC::TypePrimitives
{
    // base for AVL logic: https://github.com/Ekan5h/AVLtree/blob/master/AVL.cpp
    template <typename T, typename Comp = Less<T>>
    class AVLTree
    {            
    public:
        T* insert(const T& x, bool overwrite_duplicate = true) 
		{ 
			Node* result = nullptr;
			root = insertUtil(root, x, result, overwrite_duplicate); 
			gba_assert(result!=nullptr);
			//Debug::tty_log("result = %X", result);
			//Debug::tty_log("result = %X", &result->key);
			return &result->key;
		}
        void remove(const T& x) { root = removeUtil(root, x); }
        bool find(const T& x) const { return searchUtil(root, x) != nullptr; }		

        int height() const { return height(root); }
        int size() const { return nodes_count; }
    private:        
		
		class Node
        {
        private:		
			int data0=0;
			int data1=0;		
        public:
            T key;
            Node(T k) {                
                key = k;				
				left(nullptr);
				right(nullptr);
				height(1);
            }			

            int height() const { return (data0>>20) & HEIGHT_MASK; }
            Node* left() const 
			{  
				int a = data0 & CHILD_MASK;
				if(a==0x00040000)
					return nullptr;				
				a |= 0x02000000;				
				return (Node*)a;	
			}
            Node* right() const { return (Node*)data1; }

            void height(int v) 
			{ 				
				data0 &= CHILD_MASK;
				data0 |= (v&HEIGHT_MASK)<<20;
			}
			
			void left(Node* n) 
			{ 		
				data0 &= ~CHILD_MASK;
				data0 |= (n==nullptr ? 0x00040000 : (int)n)&CHILD_MASK;				
			}
			
            void right(Node* n) { data1 = (int)n; }
			
			inline static constexpr int HEIGHT_MASK = (1<<12)-1;
			inline static constexpr int CHILD_MASK = 0x000FFFFF;
			
			static Node* clone(Node* node)
			{                    
				if (node == nullptr) return nullptr;
				Node* n = new Node(node->key);                    
				n->left(clone(node->left()));
				n->right(clone(node->right()));
				n->height(node->height());
				return n;
			}
        }; 
    private:

        Node* root = nullptr;
        int nodes_count = 0;    

        static int height(Node* head) {
            if (head == nullptr) return 0;
            return head->height();
        }

        Node* rightRotation(Node* head) {
            Node* newhead = head->left();
            head->left(newhead->right());
            newhead->right(head);
            head->height(1 + max(height(head->left()), height(head->right())));
            newhead->height(1 + max(height(newhead->left()), height(newhead->right())));
            return newhead;
        }

        Node* leftRotation(Node* head) {
            Node* newhead = head->right();
            head->right(newhead->left());
            newhead->left(head);
            head->height(1 + max(height(head->left()), height(head->right())));
            newhead->height(1 + max(height(newhead->left()), height(newhead->right())));
            return newhead;
        }

        Node* insertUtil(Node* head, const T& x, Node* &target_node, bool overwrite_duplicate = true) {
            if (head == nullptr) {
                nodes_count += 1;
                Node* temp = new Node(x);
				
				target_node = temp;				
                return temp;
            }
            if ((Comp{})(x, head->key) == 0)
            {
				if(overwrite_duplicate)
					head->key = x; // update node, in case there other useful properties besides the comparator key
				target_node = head;
                return head;
            }

            if ((Comp{})(x, head->key) < 0) head->left(insertUtil(head->left(), x, target_node, overwrite_duplicate));
            else if ((Comp{})(x, head->key) > 0) head->right(insertUtil(head->right(), x, target_node, overwrite_duplicate));

            head->height(1 + max(height(head->left()), height(head->right())));
            int bal = height(head->left()) - height(head->right());
            if (bal > 1) {
                if ((Comp{})(x, head->left()->key) < 0) {
                    return rightRotation(head);
                }
                else {
                    head->left(leftRotation(head->left()));
                    return rightRotation(head);
                }
            }
            else if (bal < -1) {
                if ((Comp{})(x, head->right()->key) > 0) {
                    return leftRotation(head);
                }
                else {
                    head->right(rightRotation(head->right()));
                    return leftRotation(head);
                }
            }
            return head;
        }

        Node* removeUtil(Node* head, const T& x) {
            if (head == nullptr) return nullptr;
            if ((Comp{})(x, head->key)<0) {
                head->left(removeUtil(head->left(), x));
            }
            else if ((Comp{})(x, head->key) > 0) {
                head->right(removeUtil(head->right(), x));
            }
            else {
                Node* r = head->right();
                if (head->right() == nullptr) {
                    Node* l = head->left();
                    delete head;
                    head = l;
                    nodes_count--;
                }
                else if (head->left() == nullptr) {
                    delete head;
                    head = r;
                    nodes_count--;
                }
                else {
                    while (r->left() != nullptr) r = r->left();
                    head->key = r->key;
                    head->right(removeUtil(head->right(), r->key));
                }
            }
            if (head == nullptr) return head;
            head->height(1 + max(height(head->left()), height(head->right())));
            int bal = height(head->left()) - height(head->right());
            if (bal > 1) {
                // it should be relpaced with "height(head->left->left) >= height(head->left->right)"?
                if (height(head->left()->left()) >= height(head->left()->right())) {
                    return rightRotation(head);
                }
                else {
                    head->left(leftRotation(head->left()));
                    return rightRotation(head);
                }
            }
            else if (bal < -1) {
                if (height(head->right()->right()) >= height(head->right()->left())) {
                    return leftRotation(head);
                }
                else {
                    head->right(rightRotation(head->right()));
                    return leftRotation(head);
                }
            }
            return head;
        }

        Node* searchUtil(Node* head, const T& x) const 
		{
            if (head == nullptr) return nullptr;
            T k = head->key;
            if ((Comp{})(k, x) == 0) return head;
            if ((Comp{})(k, x) > 0) 
				return searchUtil(head->left(), x);
            else  
				return searchUtil(head->right(), x);
        }        

    public:
        class iterator
        {
        private:
            AVLTree<T, Comp>* tree;
            DSC::Stack<Node*> stack;
        public:
            iterator(AVLTree<T, Comp>* t) : tree(t) {}

            iterator operator ++()
            {
                if (stack.is_empty()) return *this;

                Node* e = stack.top();
                if (e->right() != nullptr)
                {
                    e = e->right();
                    for (; e != nullptr; e = e->left())
                        stack.push(e);
                }
                else
                {
                    stack.pop();
                    while (!stack.is_empty() && stack.top()->right() == e)
                        e = stack.pop();
                }
                return *this;
            }

            const T& operator *()
            {
                gba_assert(!stack.is_empty(), "Invalid iterator");
                return stack.top()->key;
            }

            bool operator != (const iterator& other)
            {
                if (tree->root != other.tree->root) return true;
                if (stack.is_empty() && other.stack.is_empty()) return false;
                if (stack.is_empty() ^ other.stack.is_empty()) return true;
                return stack.top() != other.stack.top();
            }

            static iterator first(AVLTree<T, Comp>* t)
            {
                iterator it(t);
                for (Node* n = t->root; n != nullptr; n = n->left())
                {
                    it.stack.push(n);
                }
                return it;
            }
        };

        iterator begin() { return iterator::first(this); }
        iterator end() { return iterator(this); }        

        ~AVLTree()
        {
			if (root == nullptr) return;
            DSC::Stack<Node*> nodes;
            nodes.push(root);
            while (!nodes.is_empty())
            {
                Node* n = nodes.pop();
                if (n->left() != nullptr) nodes.push(n->left());
                if (n->right() != nullptr) nodes.push(n->right());
                delete n;
            }
        }
		
		AVLTree(const AVLTree<T, Comp>& other)
		{
			this->nodes_count = other.nodes_count;
			this->root = Node::clone(other.root);
		}

		AVLTree<T, Comp>& operator = (const AVLTree<T, Comp>& other)
		{
			this->nodes_count = other.nodes_count;
			this->root = Node::clone(other.root);
			return *this;
		}

		AVLTree(AVLTree<T, Comp>&& other)
		{
			this->nodes_count = other.nodes_count;
			this->root = other.root;
			other.root = nullptr;
			other.nodes_count = 0;
		}

		AVLTree<T, Comp>& operator = (AVLTree<T, Comp>&& other)
		{
			this->nodes_count = other.nodes_count;
			this->root = other.root;
			other.root = nullptr;
			other.nodes_count = 0;
			return *this;
		}

		AVLTree() {}
    };
}