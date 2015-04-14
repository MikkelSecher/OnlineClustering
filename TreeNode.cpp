#include "TreeNode.h"

using namespace std;

long long TreeNode::nodeCounter = 0;


/*********************************
********** CONSTRUCTORS **********
*********************************/
TreeNode::TreeNode()
{
    //ctor
}

TreeNode::TreeNode(double dPoint, TreeNode *parent)
{

    parentNode = parent;

    live = true;

    visited = true;
    #pragma omp critical
    {
        nodeCounter++;
        nId = TreeNode::nodeCounter;
    }

    // TODO pointer to list a points

    if(parent != NULL)
    {
        depth = parent->depth+1;
        content = Partitioning(&parent->content);
    }
    else
    {
        rootNode = true;
        depth = 0;
        content = Partitioning(dPoint);
    }

}


/*********************************
********* INITITALIZATION ********
*********************************/
void TreeNode::setParent(TreeNode *parent)
{
    parentNode = parent;
}

void TreeNode::addChild(TreeNode *child)
{
    children.push_front(child);
    livingChildren++;
}

/*********************************
************* CHECKING ***********
*********************************/
bool TreeNode::doesChildExist(TreeNode *node)
{
    list<TreeNode*>::iterator i;
    for(i = children.begin(); i != children.end(); i++)
    {
        if((*i)->nId == node->nId)
            return false;
    }
    return true;
}

