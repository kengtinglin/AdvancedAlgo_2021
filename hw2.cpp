#include <iostream>
#include <fstream>
#include <string> 
#include <vector> 
#include <algorithm>
#include <math.h>
#include <iomanip>
using namespace std;


struct Node {
  float value;
  vector<int> id_list;  // Because there are many same values, I used a vector to store them.
  Node *parent;
  Node *left;
  Node *right;
  int color;  // 0: black; 1: red
};

// The data structure of instruction R/r's output
struct r_output {
  int id;
  float value;
};

// The data structure of instruction V/v'a output
struct v_output {
  int high;
  int low;
};

// The data structure used to express the first one node in find K function
struct k_start {
  Node *first;
  bool is_value;
  int listsize;
};


typedef Node *NodePtr;

// The RedBlack Tree
class RedBlackTree {
  private:
    NodePtr root;
    NodePtr TNULL;

    // Sort a list
    vector<int> sortlist(vector<int> list) {
      sort(list.begin(), list.end());
      return list;
    }

    // Find the rightmost node
    NodePtr rightmost(NodePtr node) {
      while (node->right != TNULL) {
        node = node->right;
      }
      return node;
    }

    // Find the leftmost node
    NodePtr leftmost(NodePtr node){
      while (node->left != TNULL){
        node = node->left;
      }
      return node;
    }

    // Search in a list
    int searchlist(vector<int> list, int key) {
      int index = -1;
      for (int i = 0; i < list.size(); i++) {
        if (list[i] == key) {
          index = i;
        }
      }
      return index;
    }

    /* This function is used to find the first node when executing the find_K function
    If the node we want to find is not in the R-B tree, we will find the approxinate node 
    in the R-B tree. */ 
    k_start search_K_first(NodePtr node, float key) {   // key is the value we want to find
      bool found = false;
      k_start k_begin;
      k_begin.first = node;
      k_begin.is_value = false;
      k_begin.listsize = 0;
      
      while (found == false){  // When the key is not in the RB tree
        if (key < node->value) {
          if (node->left == TNULL) {  // The key we found is the left child of someone
            k_begin.is_value = false;
            k_begin.listsize = 1; // size = node size
            found = true;
            if (fabs(node->value - key) <= fabs(predecessor(node)->value - key) || predecessor(node) == TNULL) {
              k_begin.first = node;
              if (fabs(node->value - key) == fabs(predecessor(node)->value - key)) {
                k_begin.listsize = 2; // (predecessor + node)'s id
              }
              return k_begin;
            } else {
              k_begin.first = predecessor(node);
              return k_begin;
            }
          } else {
            node = node->left;
          }
        } else if (key > node->value) {
          if (node->right == TNULL) {  // The key we found is the right child of someone
            k_begin.is_value = false;
            k_begin.listsize = 1;
            found = true;
            if (fabs(node->value - key) <= fabs(successor(node)->value - key) || successor(node) == TNULL ) {
              k_begin.first = node;
              if (fabs(node->value - key) == fabs(successor(node)->value - key)) {
                k_begin.listsize = 3; // (successor + node)'s id
              }
              return k_begin;
            } else {
              k_begin.first = successor(node);
              return k_begin;
            }
          } else {
            node = node->right;
          }
        } else {  // The key is found in the R-B tree
          k_begin.first = node;
          k_begin.is_value = true;
          k_begin.listsize = 1;
          found = true;
          return k_begin;
        }    
      }
      return k_begin;
    }

    // For balancing the tree after deletion
    void deleteFix(NodePtr x) {
      NodePtr s;
      while (x != root && x->color == 0) {
        if (x == x->parent->left) {
          s = x->parent->right;
          if (s->color == 1) {  // case 1
            s->color = 0;  // 0: black
            x->parent->color = 1;  // 1: red
            leftRotate(x->parent);
            s = x->parent->right;
          }

          if (s->left->color == 0 && s->right->color == 0) {  // case 2
            s->color = 1;
            x = x->parent;
          } else {
            if (s->right->color == 0) {  // case 3
              s->left->color = 0;
              s->color = 1;
              rightRotate(s);
              s = x->parent->right;
            }

            s->color = x->parent->color;  // case 4
            x->parent->color = 0;
            s->right->color = 0;
            leftRotate(x->parent);
            x = root;
          }
        } else {
          s = x->parent->left;
          if (s->color == 1) {  // case 1
            s->color = 0;
            x->parent->color = 1;
            rightRotate(x->parent);
            s = x->parent->left;
          }

          if (s->right->color == 0 && s->right->color == 0) {  // case 2
            s->color = 1;
            x = x->parent;
          } else {
            if (s->left->color == 0) {  // case 3
              s->right->color = 0;
              s->color = 1;
              leftRotate(s);
              s = x->parent->left;
            }

            s->color = x->parent->color;  // case 4
            x->parent->color = 0;
            s->left->color = 0;
            rightRotate(x->parent);
            x = root;
          }
       }
      }
      x->color = 0;
    }

    void rbTransplant(NodePtr u, NodePtr v) {
      if (u->parent == TNULL) {
        root = v;
      } else if (u == u->parent->left) {
        u->parent->left = v;
      } else {
        u->parent->right = v;
      }
      v->parent = u->parent;
    }

    // Delete the node in the R-B tree
    void deleteNodeHelper(NodePtr node, int id, float key) {
      bool repeat = false;
      NodePtr z = TNULL;
      NodePtr x, y;
      while (node != TNULL) {
        if (node->value == key) {
          if (searchlist(node->id_list, id) != -1 && (node->id_list).size() > 1) {
            repeat = true;
            int index = searchlist(node->id_list, id);
            node->id_list.erase(node->id_list.begin() + index);
          } else {
            z = node;
          }
        }
        if (node->value < key) {
          node = node->right;
        } else {
          node = node->left;
        }
      }

      if (z == TNULL && repeat == false) {
        cout << "Key not found in the tree" << endl;
        return;
      }
      if (repeat == false) {
        y = z;
        int y_original_color = y->color;
        if (z->left == TNULL) {
          x = z->right;
          rbTransplant(z, z->right);
        } else if (z->right == TNULL) {
          x = z->left;
          rbTransplant(z, z->left);
        } else {
          y = minimum(z->right);
          y_original_color = y->color;
          x = y->right;
          if (y->parent == z) {
            x->parent = y;
          } else {
            rbTransplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
          }

          rbTransplant(z, y);
          y->left = z->left;
          y->left->parent = y;
          y->color = z->color;
        }
        delete z;
        if (y_original_color == 0) {
          deleteFix(x);
        }
      }
    }

    // For balancing the tree after insertion
    void insertFix(NodePtr k) {
      NodePtr u;
      while (k->parent->color == 1) {
        if (k->parent == k->parent->parent->right) {
          u = k->parent->parent->left;
          if (u->color == 1) {  // case 1
            u->color = 0;
            k->parent->color = 0;
            k->parent->parent->color = 1;
            k = k->parent->parent;
          } else {  // case 2
            if (k == k->parent->left) {
              k = k->parent;
              rightRotate(k);
            }
            k->parent->color = 0;  // case 3
            k->parent->parent->color = 1;
            leftRotate(k->parent->parent);
          }
        } else {
          u = k->parent->parent->right;
            if (u->color == 1) { // case 1
            u->color = 0;  
            k->parent->color = 0;
            k->parent->parent->color = 1;
            k = k->parent->parent;
            } else {  // case 2
            if (k == k->parent->right) {
              k = k->parent;
              leftRotate(k);
            }
            k->parent->color = 0;  // case 3
            k->parent->parent->color = 1;
            rightRotate(k->parent->parent);
          }
        }
        if (k == root) {
          break;
        }
      }
      root->color = 0;
    }


  public:
    RedBlackTree() {
      TNULL = new Node;
      TNULL->color = 0;
      TNULL->left = TNULL;
      TNULL->right = TNULL;
      root = TNULL;
    }

    // Find minimum
    NodePtr minimum(NodePtr node) {
      while (node->left != TNULL) {
        node = node->left;
      }
      return node;
    }

    // Find maximum
    NodePtr maximum(NodePtr node) {
      while (node->right != TNULL) {
        node = node->right;
      }
      return node;
    }

    // Find successor
    NodePtr successor(NodePtr x) {
      if (x->right != TNULL) {
        return minimum(x->right);
      }

      NodePtr y = x->parent;
      while (y != TNULL && x == y->right) {
        x = y;
        y = y->parent;
      }
      return y;
    }

    // Fidn predecessor
    NodePtr predecessor(NodePtr x) {
      if (x->left != TNULL) {
        return maximum(x->left);
      }

      NodePtr y = x->parent;
      while (y != TNULL && x == y->left) {
        x = y;
        y = y->parent;
      }
      return y;
    }

    // Do left rotate
    void leftRotate(NodePtr x) {
      NodePtr y = x->right;
      x->right = y->left;
      if (y->left != TNULL) {
        y->left->parent = x;
      }
      y->parent = x->parent;
      if (x->parent == TNULL) {
        this->root = y;
      } else if (x == x->parent->left) {
        x->parent->left = y;
      } else {
        x->parent->right = y;
      }
      y->left = x;
      x->parent = y;
    }

    // Do right rotate
    void rightRotate(NodePtr x) {
      NodePtr y = x->left;
      x->left = y->right;
      if (y->right != TNULL) {
        y->right->parent = x;
      }
      y->parent = x->parent;
      if (x->parent == TNULL) {
        this->root = y;
      } else if (x == x->parent->right) {
        x->parent->right = y;
      } else {
        x->parent->left = y;
      }
      y->right = x;
      x->parent = y;
    }

  // Inserting a key
    void insert_key(int id, float key) {
      NodePtr node = new Node;
      node->parent = TNULL;
      node->value = key;
      node->left = TNULL;
      node->right = TNULL;
      node->color = 1;
      node->id_list.push_back(id);

      NodePtr y = TNULL;
      NodePtr x = this->root;
      bool repeat = false;
      while (x != TNULL && repeat == false) {
        y = x;
        if (node->value < x->value) {
          x = x->left;
        } else if (node->value > x->value){
          x = x->right;
        } else {
          (x->id_list).push_back(id);
          repeat = true;
        }
      }

      if (repeat == true){
        if (x->parent == TNULL){
          root = x;
        }
      }
  
      if (repeat == false) {
        node->parent = y;
        if (y == TNULL) {
          root = node;
        } else if (node->value < y->value) {
        y->left = node;
        } else {
        y->right = node;
        }  

        if (node->parent == TNULL) {
          node->color = 0;
          return;
        }

        if (node->parent->parent == TNULL) {
          return;
        }
        insertFix(node);
      }
    }

    void deleteNode(int id, float value) {
      deleteNodeHelper(this->root, id, value);
    }

  // R and r
  /* I searched the rightmost node which has the largest value first,
  and do inverse-in-order(Right child-Visiting-Left child) walk to find the rank.*/
    r_output find_by_rank(int rank, int mode){
      r_output output;
      NodePtr node = rightmost(this->root);
      rank = rank - node->id_list.size();
      while (rank > 0){
        node = predecessor(node);
        rank = rank - node->id_list.size();
      }  
      node->id_list = sortlist(node->id_list);
      if (mode == 1){
        output.id = node->id_list.at(0);
        output.value = node->value;
      } else {
        output.id = node->id_list.at(rank + node->id_list.size() - 1);
        output.value = node->value;
      }
      return output;
    }
  
  // V and v
  /* I searched the rightmost node which has the largest value first,
  and do inverse-in-order(Right child-Visiting-Left child) walk to search the value.*/
    v_output find_by_value(float value, int mode){
      v_output output;
      int rank = 1;
      bool found = false;
      NodePtr node = rightmost(this->root);
      while (found == false) {
        if (node->value == value) {
          found = true;
          node->id_list = sortlist(node->id_list);
          if (mode == 1){
            output.high = rank - node->id_list.size() + 1;
            output.low = output.high; 
          } else {
            output.high = rank - node->id_list.size() + 1;
            output.low = rank;
          }
          return output;
        }
        node = predecessor(node);
        rank = rank + node->id_list.size();
      }
      return output;
    }

    // K
    /* 1. Find the first node. The best case is that the K-value is found in the tree.
       2. Because of RBtree's property, the successor or predecessor may be the closest value,
          we prior to search them fitst.
       3. Repeat 2. when reach the request. */
    vector<int> find_K(int n, float value) {
      vector<int> K_id;
      NodePtr node_begin;
      k_start node_from_search;
      NodePtr node_left;
      NodePtr node_right;
       
      node_from_search = search_K_first(this->root, value);
      node_begin = node_from_search.first;
      
      
      if (node_from_search.listsize == 2) {  // (predecessor + node)'s id
        node_begin->id_list.insert(node_begin->id_list.end(), predecessor(node_begin)->id_list.begin(), predecessor(node_begin)->id_list.end());
        node_left = predecessor(predecessor(node_begin));
        node_right = successor(node_begin);
      } else if (node_from_search.listsize == 3) {  // (successor + node)'s id
        node_begin->id_list.insert(node_begin->id_list.end(), successor(node_begin)->id_list.begin(), successor(node_begin)->id_list.end());
        node_left = predecessor(node_begin);
        node_right = successor(successor(node_begin));
      } else {
        node_left = predecessor(node_begin);
        node_right = successor(node_begin);
      }
      node_begin->id_list = sortlist(node_begin->id_list);
      if (n <= node_begin->id_list.size()) {
        K_id.insert(K_id.end(), node_begin->id_list.begin(), node_begin->id_list.begin() + n);
        return K_id;
      } else {
        K_id.insert(K_id.end(), node_begin->id_list.begin(), node_begin->id_list.end());
        n = n - node_begin->id_list.size();
      }
      
      while (n > 0) {
        if (fabs(node_left->value - value) < fabs(node_right->value - value) || node_right == TNULL) {
          n = n - node_left->id_list.size();
          node_left->id_list = sortlist(node_left->id_list);
          if (n > 0) {
            K_id.insert(K_id.end(), node_left->id_list.begin(), node_left->id_list.end());
            node_left = predecessor(node_left);
          } else {
            n = n + node_left->id_list.size();
            K_id.insert(K_id.end(), node_left->id_list.begin(), node_left->id_list.begin() + n);
            return K_id;
          }
        } else if (fabs(node_left->value - value) > fabs(node_right->value - value) || node_left == TNULL) {
          n = n - node_right->id_list.size();
          node_right->id_list = sortlist(node_right->id_list);
          if (n > 0){
            K_id.insert(K_id.end(), node_right->id_list.begin(), node_right->id_list.end());
            node_right = successor(node_right);
          } else {
            n = n + node_right->id_list.size();
            K_id.insert(K_id.end(), node_right->id_list.begin(), node_right->id_list.begin() + n);
            return K_id;
          }
        } else if (fabs(node_left->value - value) == fabs(node_right->value - value)) {
          n = n - node_left->id_list.size() - node_right->id_list.size();
          node_left->id_list.insert(node_left->id_list.end(), node_right->id_list.begin(), node_right->id_list.end());
          node_left->id_list = sortlist(node_left->id_list);
          if (n > 0){
            K_id.insert(K_id.end(), node_left->id_list.begin(), node_left->id_list.end());
            node_left = predecessor(node_left);
            node_right = successor(node_right);
          } else {
            n = n + node_left->id_list.size();
            K_id.insert(K_id.end(), node_left->id_list.begin(), node_left->id_list.begin() + n);
            return K_id;
          }
        }
      }
      return K_id;
    }
  };



int main(int argc, char *argv[]) {
  RedBlackTree bst;
  fstream input(argv[1]);
  ofstream output(argv[2]); 
  vector<NodePtr> node_list;
  v_output output_v;
  r_output output_r;
  vector<int> output_k;
  char token;
  int id,rank,number;
  float value; // the Combat Power (CP)
	
  //input and read file
  while (input >> token) {  
    switch (token) {
      case 'I':  // insert
			  input >> id;
			  input >> value;
			  bst.insert_key(id, value);
			  break;
      case 'D':  // delete
        input >> id;
        input >> value;
			  bst.deleteNode(id, value);
        break;
      case 'r':
        input >> rank;
        output_r = bst.find_by_rank(rank, 1); // mode = 1
        output << "r " << output_r.id << " " << fixed << setprecision(1) << output_r.value << "\n";
        break;
      case 'R':
        input >> rank;
        output_r = bst.find_by_rank(rank, 2);  // mode = 2
        output << "R " << output_r.id << " " << output_r.value << "\n";
        break;
      case 'v':
        input >> value;
        output_v = bst.find_by_value(value, 1);
        output << "v " << output_v.high << "\n";
        break;
      case 'V':
        input >> value;
        output_v = bst.find_by_value(value, 2);
        output << "V " << output_v.high << " " << output_v.low << "\n";
        break;
      case 'K':
        input >> value;
        input >> number;
        output_k = bst.find_K(number, value);
        output << "K ";
        for (int i = 0; i < output_k.size() - 1; i++) {
          output << output_k.at(i) << " ";
        }
        output << output_k.at(output_k.size() - 1) << "\n";
        break;
      }
  }
  input.close();
  output.close();
  return 0;
}