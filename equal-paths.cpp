#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int curr_height(Node * root)
{
  if (root == nullptr) return 0;

  int left = curr_height(root->left);
	int right = curr_height(root->right);

	if (left == -1 || right == -1) return -1;
	if ((left-right)!=0 && left!=0 && right!=0) return -1;

	if (root->left != nullptr)
		return left+1;
	if (root->right != nullptr)
		return right+1;
	return 1;
}

bool equalPaths(Node * root)
{
    // Add your code below
    return (curr_height(root)!=-1);
}

