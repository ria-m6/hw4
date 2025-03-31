#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathCounter(Node * root){
  if(root == NULL){
    return 0;
  }
  if(root->left == NULL && root->right == NULL){ //if there are no children
    return 1;
  } else if(root->left != NULL && root->right != NULL){
    return 1+std::max(pathCounter(root -> left), pathCounter(root -> right));
    // return 1+;
  }else if(root->left != NULL){
    return 1+pathCounter(root -> left);
  } else{
    return 1+pathCounter(root -> right);
  }
}



bool equalPaths(Node * root)
{
    // Add your code below
    //base case - nothing in the tree
    if(root == NULL){
      return true;
    }
    if(root->left == NULL && root->right == NULL){ //if there are no children
      return true;
    }
    int rightTree = 0;
    int leftTree = 0;
    //helper function
    leftTree = pathCounter(root->left);
    rightTree = pathCounter(root->right);
    // std::cout << leftTree;

    if(leftTree == rightTree){
      return true;
    } else if((leftTree == 0 && rightTree == 1) || (leftTree == 1 && rightTree == 0)){
      //if there is only 1 child
      return true;
    }
    return false;
    
    
    
}

