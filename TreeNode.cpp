#include "TreeNode.h"
#include <stdio.h>
#include <string.h>
using namespace std;

long long TreeNode::nodeCounter = 0;


/*********************************/
/********** CONSTRUCTORS **********/
/*********************************/

TreeNode::TreeNode(double dPoint, TreeNode *parent){

    parentNode = parent;
    live = true;
    visited = true;

        nodeCounter++;
        nId = TreeNode::nodeCounter;

    if(parent != NULL){
        depth = parent->depth+1;
        content = Partitioning(&parent->content);
    }else{
        rootNode = true;
        depth = 0;
        content = Partitioning(dPoint);
    }
}

TreeNode::TreeNode(list<double> pointList, list<double> clustersList){
    live = true;
    visited = true;
    nodeCounter++;
    nId = TreeNode::nodeCounter;
    depth = 0;

    content = Partitioning(pointList, clustersList);
    //create content partitioning
    //add to nodeQueue

}



/*********************************/
/********* INITITALIZATION ********/
/*********************************/

void TreeNode::addChild(TreeNode *child){
    children.push_front(child);
    livingChildren++;
}


/*********************************/
/******** CONTENT FUNCTIONS *******/
/*********************************/

void TreeNode::listClusters(){
    content.listClusters();
}

void TreeNode::listPoints(){
    content.listPoints();
}

void TreeNode::listSortedPoints(){
    content.listSortedPoints();
}

bool TreeNode::doesPointExist(double point){
    return content.doesPointExist(point);
}

void TreeNode::openCluster(double point, bool force){
    content.openCluster(point, force);
}

void TreeNode::openCluster(double point){
    content.openCluster(point, false);
}

void TreeNode::addPointToCluster(double point, bool force){
    content.addPointToCluster(point, force);
}

void TreeNode::addPointToCluster(double point){
    content.addPointToCluster(point, false);
}

void TreeNode::growClusterRight(double point, bool force){
    content.growClusterRight(point, force);
}

void TreeNode::growClusterRight(double point){
    content.growClusterRight(point, false);
}

void TreeNode::growClusterLeft(double point, bool force){
    content.growClusterLeft(point, force);
}

void TreeNode::growClusterLeft(double point){
    content.growClusterLeft(point, false);
}

string TreeNode::getPointHash(){
    return content.pointHash;
}

string TreeNode::getFullHash(){
    return content.fullHash;
}

void TreeNode::updateHashes(){
    content.updateHashes();

}

int TreeNode::getNumberOfPoints(){
    return content.nNumberOfPoints;
}


int TreeNode::getNumberOfClusters(){
    return content.nNumberOfClusters;
}


int TreeNode::pointInRange(double point){
    return content.pointInRange(point);
}

list<double> TreeNode::getAmbPoints(){
    return content.getAmbPoints();
}

list<double> TreeNode::getForcePoints(){
    return content.getForcePoints();
}

double TreeNode::calcRatio(){
    return content.calcRatio();
}

bool TreeNode::isRoot(){
    return rootNode;
}
