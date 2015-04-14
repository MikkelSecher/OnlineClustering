#include "Tree.h"

using namespace std;

//# define ratio 1.6
/*********************************
********** CONSTRUCTORS **********
*********************************/

Tree::Tree()
{
    //ctor
}


Tree::Tree(double dStartingPoint, list<double> deltaPoints, int prefix, double ratioin, int printin) //REGULAR SEARCH-TREE
{

    //delta[deltaPoints.size()];
//    cout << "Number of deltas is "<< deltaPoints.size() << endl;
    deltas = deltaPoints.size();
    resPrefix = prefix;
    print = printin;
    ratio = ratioin;
    for(int i = 0; i < deltas; i++)
    {
        delta.push_back(deltaPoints.back());
        deltaPoints.pop_back();
    }

    nodes[omp_get_thread_num()].push_front(TreeNode(dStartingPoint, NULL));
    nodeQueue.push_back(&nodes[omp_get_thread_num()].front());
    nodeQueue.front()->parentNode = NULL;
}

Tree::Tree(list<double> sequence, list<double> deltaPoints, int prefix, double ratioin, int printin ) //CREATES A TREE FOR CHECKING A SEQUENCE
{
    cout << "Number of deltas is "<< deltaPoints.size() << endl;
    deltas = deltaPoints.size();
    resPrefix = prefix;
    print = printin;
    ratio = ratioin;
    for(int i = 0; i < deltas; i++)
    {
        delta.push_back(deltaPoints.back());
        deltaPoints.pop_back();
    }

    seqNodes.push_front(TreeNode(sequence.front(), NULL));
    seqQueue.push_front(&seqNodes.front());
    sequence.pop_front();

    checkSequence(sequence);
}

TreeNode* Tree::newNode(int tid, double point, TreeNode* parent)
{
    nodes[tid].push_front(TreeNode(point, parent));
    nodes[tid].front().trueLink = nodes[tid].begin();

//    if(77 == nodes[tid].front().nId)
//    {
//        cout << "******************************************************" << endl;
//        nodes[tid].front().content.listClusters();
//        nodes[tid].front().content.listPoints();
//    }

    return &nodes[tid].front();
}

/*********************************
***** HANDLERS FOR NEW POINTS ****
*********************************/

void Tree::nextPoint(int level) //ADDS ALL POSSIBLE POINTS, BASED ON DELTAS
{
// For an entire level in the tree, this calculates all the points to be added to each
// node, based on the delta values in tree.h.
// If a point already exists in a node, it is skipped.

    if(print != 2)
    {

        if(nodeQueue.size() == 0)
            exit(0);
        if(nodeQueue.size() > 0)
        {
            list<TreeNode*>::iterator i;
            list<double>::iterator j;

            list<double> pointsToAdd;

            cout << "Level: " << level << endl;


            for(i = nodeQueue.begin(); i != nodeQueue.end(); i++)
            {
                if((*i)->depth == level-1)
                {
                    for(j = nodeQueue.front()->content.sortedPoints.begin(); j != nodeQueue.front()->content.sortedPoints.end(); j++)
                    {
                        for(int k = 0; k < deltas; k++)
                        {
                            //debugging
                            //double point = nodeQueue.front()->content.doesPointExist((*j) + delta[k]);
                            if(nodeQueue.front()->content.doesPointExist((*j) + delta[k]))
                            {
                                pointsToAdd.push_back((*j)+delta[k]);
                                //addPoint(0, (*j)+delta[k]);
                            }
                        }
                    }
                    pointsToAdd.sort();
                    pointsToAdd.unique();

                    for(j = pointsToAdd.begin(); j != pointsToAdd.end(); j++)
                    {
                        addPoint(0, (*j));
                    }
                    pointsToAdd.clear();

                    i = nodeQueue.erase(i);

                    i--;
                }
            }

        }
    }else{
        double pointList[11] = {4,5,6,2,1,0, 2.5,7, 8, 8.5, 9};
        cout << "Adding point " << pointList[level-1] << endl;
        list<TreeNode*>::iterator i;

        for(i = nodeQueue.begin(); i != nodeQueue.end(); i++)
        {
            if((*i)->depth == level-1)
            {
                (*i)->content.listPoints();
                (*i)->content.listClusters();

                addPoint(0, pointList[level-1]);
                i = nodeQueue.erase(i);
                i--;
            }
        }
    }
}


bool Tree::checkPartitioning() //CHECKS RATIO, AND IF A PARTITIONING CAN BE FORCED
{
// Checks ratio for a partitioning
// Returns FALSE if the ratio is above the threshold
// Also tries to break the partitioning by forcing new points
    int tid = omp_get_thread_num();
    if(geq(nodes[tid].front().content.calcRatio(), ratio))
    {
        return false;
    }
    if(nodes[tid].front().depth == 0)
        return true;

    nodes[tid].front().content.setAmbSpace();
    list<double> ambPoints = nodes[tid].front().content.getAmbPoints();
    if(!force_experimental(&nodes[tid].front(), &nodes[tid].front(), nodes[tid].front().content.getForcePoints(), ambPoints))
        return false;

    //return nodes[tid].front().content.force(ratio);
    return true;
}

bool Tree::force_experimental(TreeNode *node, TreeNode *originalNode, list<double> forcePoints, list<double> ambPoints)
{
    int tid = omp_get_thread_num();
    list<double>::iterator dit;
    list<double> emptyList;
    //node->content.forced = true;
    TreeNode* forceNode;

    ///If there are no forcepoints
    if(forcePoints.size() == 0)
    {
        ///As long as there is still ambiguous points
        while(ambPoints.size() != 0)
        {
            ///Check that the point doesn't already exist (safety)
            if(!node->content.doesPointExist(ambPoints.front()))
            {
                //cout << "POOOOOOOOOOOOOOOOPPING! " << endl;
                ambPoints.pop_front();
                continue;
            }
            ///Get a new node to work on
            forceNode = newNode(tid, ambPoints.front(), node);
            forceNode->content.openCluster(ambPoints.front());

            ///If opening the new cluster breaks the ratio, it means that
            ///there is only one choice -> expand the current cluster
            if(geq(forceNode->content.calcRatio(), ratio))
            {
                ///Erase the node that broke the ratio
                nodes[tid].erase(forceNode->trueLink);
                ///And make a new one
                forceNode = newNode(tid, ambPoints.front(), node);
                ///If the ambiguous point is in range to the LEFT of a cluster/point
                if(forceNode->content.pointInRange(ambPoints.front()) == -2)
                {
                    ///Grow the cluster
                    forceNode->content.growClusterLeft(ambPoints.front());
                    ///Get new forcePoints
                    forcePoints = forceNode->content.getForcePoints();
                    ///Get ready to call the force with an empty ambPoints list
                    emptyList = ambPoints;
                    emptyList.remove(ambPoints.front());
                    ///Call it
                    if(!force_experimental(forceNode, originalNode, forcePoints, emptyList))
                    {
                        ///If it is now forcable, return false (Success)
                        node->content.forced = true;
                        node->live = false;

                        ///Update the original node with the expanded cluster
                        originalNode->content.growClusterLeft(ambPoints.front());
                        originalNode->content.forcedPoints.push_front(ambPoints.front());
                        ///Erase the forceNode from memory
                        nodes[tid].erase(forceNode->trueLink);
                        ///Clear the nodes children
                        node->children.clear();

                        return false;
                    }
                }
                ///If the ambiguous point is in range to the RIGHT of a cluster/point
                else if(forceNode->content.pointInRange(ambPoints.front()) == 2)
                {
                    forceNode->content.growClusterRight(ambPoints.front());
                    forcePoints = forceNode->content.getForcePoints();
                    emptyList = ambPoints;
                    emptyList.remove(ambPoints.front());
                    if(!force_experimental(forceNode, originalNode, forcePoints, emptyList))
                    {
                        node->content.forced = true;
                        node->live = false;

                        originalNode->content.growClusterRight(ambPoints.front());
                        originalNode->content.forcedPoints.push_front(ambPoints.front());
                        nodes[tid].erase(forceNode->trueLink);
                        node->children.clear();

                        return false;

                    }
                }
            }
            ///If this doesn't work, erase the node worked on
            nodes[tid].erase(forceNode->trueLink);
            ///And try with the next ambiguous point
            ambPoints.pop_front();
        }
        ///When there are no more points return true (FAIL)
        return true;

    }

    ///Get the current forcepoints for the node
    forcePoints = node->content.getForcePoints();

    ///For each of the forcePoints
    for(dit = forcePoints.begin(); dit != forcePoints.end(); dit++)
    {
        ///Check that the point doesn't already exist (safety)
        if(!node->content.doesPointExist((*dit)))
           {
             continue;
           }
        ///Create a new node to force with
        forceNode = newNode(tid, *dit, node);
        ///And open a forced cluster
        forceNode->content.openCluster(*dit);

        //cout<< "Node Id: " << forceNode->nId << endl;

        ///If the ratio is broken by this
        if(geq(forceNode->content.calcRatio(), ratio))
        {
            ///If successful
            node->content.forced = true;
            node->live = false;

            originalNode->content.openCluster(*dit);
            originalNode->content.forcedPoints.push_front(*dit);
            nodes[tid].erase(forceNode->trueLink);
            node->children.clear();

            return false;
        }
        ///Else call again with the forcepoint list
        else if(!force_experimental(forceNode, originalNode, forcePoints, ambPoints))
        {
            ///If successful
            originalNode->content.openCluster(*dit);

            originalNode->content.forcedPoints.push_front(*dit);
            node->content.forced = true;
            nodes[tid].erase(forceNode->trueLink);
            node->children.clear();

            return false;
        }
        ///Else try with ambiguous points
        else if(!force_experimental(forceNode, originalNode, emptyList, ambPoints))
        {
            ///If successful
            originalNode->content.openCluster(*dit);

            originalNode->content.forcedPoints.push_front(*dit);
            node->content.forced = true;
            nodes[tid].erase(forceNode->trueLink);
            node->children.clear();
            return false;
        }
        ///Else clean up and return true (FAIL)
        else{
            nodes[tid].erase(forceNode->trueLink);
            return true;
        }
        originalNode->children.clear();
    }

    //cout << "Could not force at all "<< endl;
    return true;
}



/*********************************
****** GENERAL FUNCTIONS *********
*********************************/

bool Tree::compareNodes(TreeNode* node1, TreeNode* node2) //COMPARES TWO NODES, TRUE IF EQUAL
{
    if(node1->content.nNumberOfClusters != node2->content.nNumberOfClusters)
        return false;

    if(node1->content.nNumberOfPoints != node2->content.nNumberOfPoints)
        return false;

    list<double>::iterator i;
    list<double>::iterator j = node2->content.points.begin();
    for(i = node1->content.points.begin(); i != node1->content.points.end(); i++)
    {
        if(!deq(*i, *j))
            return false;
        j++;
    }

    for(int i = 0; i < node1->content.nNumberOfClusters; i++)
    {
        if(!deq(node1->content.adClusters[i][0],node2->content.adClusters[i][0]))
            return false;

        if(!deq(node1->content.adClusters[i][1],node2->content.adClusters[i][1]))
            return false;
    }
    return true;
}

void Tree::analyzeTree(int DFlevel) //FINDS PROOFS AFTER DF PART IS DONE
{
//Start the analysis of the complete tree.
//Running through the list of succesfull nodes with subproofs
//Will count the number of subproofs resulting in proofs
    cout << endl;
    cout << "Analyzing search tree" << endl;
    cout << "---------------------------------------------" << endl;
    cout << endl;

    //int amount = 0;
    successes = 0;
    int numberOfThreads = 0;

    double startTime = omp_get_wtime();
    double endTime;

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        int succes = 0;
        list<TreeNode*>::iterator i;
        numberOfThreads = omp_get_num_threads();
            #pragma omp master
        {
            splitSuccesQueue(numberOfThreads);
        }
        #pragma omp barrier
        cout << "Split the nodes" << endl;
        #pragma omp for
        for(int t = 0; t < numberOfThreads; t++)
        {
            for(i = parSuccesfulNodes[t].begin(); i != parSuccesfulNodes[t].end(); i++)
            {
                if((*i)->nId = 1 and backtrackSolution((*i), (*i)->content.points, DFlevel, succes))
                {
                    cout << "from root PROOF FOUND in thread "<< t << endl;
                    succes++;
                }


                //amount++;
                if((*i)->nId != 1 and backtrackSolution((*i)->parentNode, (*i)->content.points, DFlevel, succes))
                {
                    cout << " from non-root PROOF FOUND in thread "<< t << endl;
                    succes++;
                }
            }
        }
        #pragma omp atomic
        successes += succes;
    }
    endTime = omp_get_wtime();
    proofTime = endTime-startTime;
    cout << endl;
    cout << "SUMMARY:" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Found " << successes << " proofs for the ratio "<< ratio << endl;
    cout << endl;
    cout << "Time spent in parallel proof finding: " << proofTime << endl;

}



/*********************************
********* TREE CLEANUP ***********
*********************************/

bool node_compare(const TreeNode *first,const TreeNode *second)
{
    if (first->content.fullHash.compare(second->content.fullHash) >= 0)
    {
        return false;
    }
    if(first->content.fullHash.compare(second->content.fullHash) < 0)
        return true;
}

bool node_compare_sequence(const TreeNode *first,const TreeNode *second)
{
    if (first->content.fullHash.compare(second->content.pointHash) >= 0)
    {
        return false;
    }
    if(first->content.fullHash.compare(second->content.pointHash) < 0)
        return true;


}

void Tree::treeCleanup() //REMOVE DUPES OF NODES AFTER NORMALIZATION
{
// Normalizes the points in each node by shifting everything to start out at 0
// After all nodes have been normalized, they are compared and duplicates are removed
    cout << "before cleanup: " << nodeQueue.size() << endl;

    nodeQueue.sort(node_compare);

    list<TreeNode*>::iterator i, j;

    for(i = nodeQueue.begin(); i != nodeQueue.end(); i++)
    {
        for(j = next(i); j != nodeQueue.end(); j++)
        {
            if(i != j)
            {
                if((*i)->content.fullHash.compare((*j)->content.fullHash) == 0)
                {
                    j = nodeQueue.erase(j);
                }
                else
                {
                    break;
                }
            }
        }
    }
    cout << "after cleanup: " << nodeQueue.size() << endl;
}

void Tree::normalize(TreeNode *node) // SHIFT ALL PARTITIONINGS TO START AT 0
{
// Shifts points, sortedPoints and the clusters, to start from 0
    if (leq(node->content.points.back(), 0))
    {
        double offset = 0;
        offset = offset - node->content.adClusters[0][0];


        list<double>::iterator i;

        for(i = node->content.sortedPoints.begin(); i != node->content.sortedPoints.end(); i++)
        {
            *i = *i+offset;
        }
        for(i = node->content.points.begin(); i != node->content.points.end(); i++)
        {
            *i = *i+offset;
        }
        for(int i = 0; i < node->content.nNumberOfClusters; i++)
        {
            node->content.adClusters[i][0] = ((node->content.adClusters[i][0])+offset);
            node->content.adClusters[i][1] = ((node->content.adClusters[i][1])+offset);
        }
    }
        node->content.updateHashes();
}



/*********************************
**** BREADTH-FIRST FUNCTIONS *****
*********************************/

void Tree::addPoint(int p, double dPoint) //ADDS A GIVEN POINT BF
{
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.


    switch (nodeQueue.front()->content.pointInRange(dPoint))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            oneChoice(nodeQueue.front(), dPoint);
        } break;
        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            twoChoicesRight(nodeQueue.front(), dPoint);

        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            twoChoicesLeft(nodeQueue.front(), dPoint);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            threeChoices(nodeQueue.front(), dPoint);
        } break;


        case 0 :            //no choice: Must open a new cluster for the point
        {
            noChoice(nodeQueue.front(), dPoint);
        } break;
    }
}

void Tree::twoChoicesLeft(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force

    int tid = omp_get_thread_num();
    //Create a new node with the point
    //Grow the appropriate cluster to the left
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterLeft(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterLeft(dPoint);

        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());

        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());

        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());

    //Increment global number of nodes
    nCount++;
    //Create a new node with the point
    //Open a new cluster for the point
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);
    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;

    }
    //Increment global number of nodes
    normalize(&nodes[tid].front());
    nCount++;
}

void Tree::twoChoicesRight(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
// This is exactly the same as twoChoicesLeft, only with growth to the right insted.
// For further info check above...
    //Create a new node with the point
    //Grow the appropriate cluster to the right
    int tid = omp_get_thread_num();
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterRight(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterRight(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();

    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());
    //Increment global number of nodes
    nCount++;
    //Create a new node with the point
    //Open a new cluster for the point
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }
//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());
    //Increment global number of nodes

}

void Tree::oneChoice(TreeNode *parent, double dPoint)
{
// Add point the an existing cluster, and see if it breaks.
// For further explaination see twoChoicesLeft()

    int tid = omp_get_thread_num();
    //Create a new node with the point
    //The point is inside an existing cluster
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.addPointToCluster(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.addPointToCluster(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }
//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());

}

void Tree::noChoice(TreeNode *parent, double dPoint)
{
// A new cluster MUST be opened for the point.
// Tries to break it by force and returns 1 if it holds up 0 if it breaks
    int tid = omp_get_thread_num();
    //Create a new node with the point
    //Open a new cluster for the point
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }
//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());
    nCount++;
}

void Tree::threeChoices(TreeNode *parent, double dPoint)
{
// Grows right, left and opens a new cluster.
// Returns the number of new nodes where succesfully added
    int tid = omp_get_thread_num();
    //Create a new node with the point
    //Grow the appropriate cluster to the right
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterRight(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterRight(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();


    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }
//    if(leq(dPoint, 0))
        normalize(&nodes[tid].front());
    //Increment global number of nodes


    //Create a new node with the point
    //Grow the appropriate cluster to the left
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterLeft(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterLeft(dPoint);

        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());

        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());

        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }
        normalize(&nodes[tid].front());


    //Create a new node with the point
    //Open a new cluster for the point
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    }
    else
    {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

    normalize(&nodes[tid].front());
}


/*********************************
****** DEPTH-FIRST FUNCTIONS *****
*********************************/

void Tree::startDF_experimental(int DFlevel, int dfDepth)
{
    int tid = omp_get_thread_num();

    if(nodeQueue.size() > 1)
    {
        cout << "Sorting" << endl;
        nodeQueue.sort(node_compare_sequence); //Sort queue wrt. sequence of points
    }

    sequenceTree();

    cout << "sorted" << endl;

    list<TreeNode*>::iterator nit = sequencedTreeQueue.front().begin();
    list<TreeNode>::iterator temp = nodes[tid].begin();

    string currentHash = nodeQueue.front()->content.pointHash;

    bool potential = true; //If a proof for a sequence is still possible, this is true.
    int nodesDeleted = 0;
    int proofCount = 0;
    //depthFirstQueue[tid].push_back(nodeQueue.front());

    list<list<TreeNode*>>::iterator lit = sequencedTreeQueue.begin();

    while(lit != sequencedTreeQueue.end())
    {
        cout << "Should only print once (startDF while loop) "<< endl;
        temp = nodes[0].begin();
        //go DF on each of the nodes in the miniQueue.
        cout << "------------------------------" << endl;
        while(nit != (*lit).end())
        {
            cout << "Node Id : "<< (*nit)->nId << endl;
            depthFirstQueue[tid].push_front((*nit));
            if(potential and !addPointRecursive(DFlevel, dfDepth))
            {
                (*nit)->live = false;
                succesfulNodes.push_back(*nit);
                nit++;

                if(nit == (*lit).end())
                    cout << "YAY Subproof found!" << endl;

                depthFirstQueue[0].clear();
            }
            else{
                //if one fails, erase all that were in the miniQueue.
                if (potential)
                {
                    nit = (*lit).begin();
                    nodes[0].erase(nodes[0].begin(), temp);
                }
                depthFirstQueue[0].clear();
                potential = false;
            }
            if (!potential)
            {
                //clearLevel(DFlevel);
                nit++;
                //erase full subtree from the node.
            }

            if(nit == (*lit).end() and potential)
            {
                //if all succeeds analyze tree and print solution.
                cout << "Found Proof! :) " << endl;
                analyzeTree(DFlevel);

                exit(1);
            }
        }

        lit++;
        if(lit != sequencedTreeQueue.end())
        {
            nit = (*lit).begin();
        }
        potential = true;


    }
//    analyzeTree(DFlevel);
    cout << "Didn't find a proof" << endl;
    if(succesfulNodes.size() > 0){
        cout << "However I did find " << succesfulNodes.size() << " sub proofs" << endl;
        succesfulNodes.back()->content.listPoints();
        succesfulNodes.back()->content.listClusters();
    }


    return; ///For testing
    while(nit != nodeQueue.end())
    {
//        cout << "Ever" << endl;
        if(nodeQueue.size()%100 == 0)
        {
            cout << nodeQueue.size() << endl;
            cout << "Node ID: " << nodeQueue.front()->nId << endl;
        }
        if(potential)
        {
            if(currentHash.compare((*nit)->content.pointHash) == 0)
            {
                depthFirstQueue[tid].push_front((*nit));
                if(addPointRecursive(DFlevel, dfDepth))
                {
                    nodes[tid].erase(nodes[tid].begin(), temp);
                    temp = nodes[tid].begin();
                    potential = false;
                }
                else{
                    (*nit)->live = false;
                }
                depthFirstQueue[tid].clear();
                nit++;
                if(nit == nodeQueue.end())
                {
                    cout << "End of the line" << endl;
                    cout << "Found " << proofCount << " proofs" << endl;
                    analyzeTree(DFlevel);
                    break;
                }
            }
            if(potential and currentHash.compare((*nit)->content.pointHash) != 0)
            {
                cout <<(*nit)->nId << endl;
                proofCount++;
                temp = nodes[tid].begin();
                    succesfulNodes.push_back(*nit);
                cout << "Found a god damn proof!" << endl;
                analyzeTree(DFlevel);
                exit(1);
                potential = false;
            }

            while(!potential and currentHash.compare((*nit)->content.pointHash) == 0)
            {
                        nit++;
            }
            if(!potential and currentHash.compare((*nit)->content.pointHash) != 0)
            {
                clearLevel(DFlevel, 1000);
                currentHash = (*nit)->content.pointHash;
                potential = true;
            }
        }
        nodeQueue.pop_front();
    }
}

void Tree::startDF(int DFlevel, int dfDepth) //DF PART OF THE PROGRAM - RUNS IN OPENMP PARALLEL
{
// Starts the Depth First portion of the program.
// Currently this only goes three levels down, from the stopping layer of the full BFS.
// This must be corrected. The solutions found using this are not complete. But will give an idea of
// which states/configurations can lead to solutions.
// It might be fixed by putting together the right states, that are related, and seeing if they can all be successful.
    double startTime = omp_get_wtime();
    double endTime;
    numberOfThreads = 0;
    if(nodeQueue.size() == 0)
        exit(0);
    if(nodeQueue.size() > 0)

    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {

        numberOfThreads = omp_get_num_threads();
        //split nodeQueue
        #pragma omp master
        {
            splitNodeQueue(numberOfThreads);
        }

        #pragma omp barrier

        cout << "Number of Threads before call: "<< omp_get_num_threads() << endl;


        #pragma omp for
        for(int t = 0; t < numberOfThreads; t++)
        {

            double possibilities[100];
            double sequence[10];
            int perms[10] = {0};
            int pos = dfDepth-1;
            int numPossibilities = 0;
            list<double>::iterator i;
            list<TreeNode*>::iterator n;
            int counter = 0;
            int queueSize = parNodeQueue[t].size();
            cout << "Queuesize for this "<< parNodeQueue[t].size() << endl;

            while(parNodeQueue[t].size() > 0)
            {
                counter++;
                if(counter%1000 == 0)
                {
                    cout << endl;
                    printf("In thread %d, %d / %d has been expanded", t, queueSize, counter);
                }

                depthFirstQueue[t].push_back(parNodeQueue[t].back());
                numPossibilities = 0;

                for(i = parNodeQueue[t].back()->content.sortedPoints.begin(); i != parNodeQueue[t].back()->content.sortedPoints.end(); i++)
                {
                    for(int j = 0; j < deltas; j++)
                    {
                        if(parNodeQueue[t].back()->content.doesPointExist((*i) + delta[j]))
                        {
                            for(int k = 0; k <= numPossibilities; k++)
                            {
                                if(deq(*i+delta[j], possibilities[k]))
                                {
                                    goto pointAlreadyAdded;
                                }
                            }
                            possibilities[numPossibilities] = ((*i) + delta[j]);
                            numPossibilities++;
                            pointAlreadyAdded:
                            continue;
                        }
                    }
                }

                // Out of the list of possible points. All combinations of 3 are added, building small subtrees in from the node.
                // If a subtree succesfully breaks the lower bound, true is returned.


                for(int initial = 0; initial < dfDepth; initial++)
                {
                    perms[initial] = initial;
                }

                bool done = false;
                pos = dfDepth-1;

                while(!done)
                {
                    bool dupes = false;


                    if(pos == dfDepth-1)
                    {


                        perms[pos]++;

                        for(int d = 0; d < dfDepth; d++)
                        {
                            for(int d2 = 0; d2 < dfDepth; d2++)
                            {
                                if(d != d2)
                                {
                                    if(perms[d] == perms[d2] or perms[pos] >= numPossibilities)
                                        dupes = true;
                                }
                            }
                        }

                        if(!dupes)
                        {

                            for(int s = 0; s < dfDepth; s++)
                            {
                                sequence[s] = possibilities[perms[s]];
                            }

                            if(addSequence(sequence, t, dfDepth))
                            {
                                parNodeQueue[t].back()->live = false;
                                succesfulNodes.push_back(parNodeQueue[t].back());

                                FILE *pFile;
                                char filename[30];

                                succesfulNodes.back()->content.listPointsToFile(t);

                                sprintf(filename, "%d_subProofSequences.txt ", t);
                                pFile = fopen (filename,"a");
                                fprintf(pFile, " | ");
                                for(int s = 0; s < dfDepth; s++)
                                {
                                    fprintf(pFile, "%2.2f ", sequence[s]);
                                }
                                fprintf(pFile, " | ");
                                fclose(pFile);

                                succesfulNodes.back()->content.listClustersToFile(t);

                                pFile = fopen (filename,"a");
                                fprintf(pFile, " \n");
                                fclose(pFile);

                                parNodeQueue[t].pop_back();
                                done = true;
                            }
                            else
                            {
                                destroySubtree(parNodeQueue[t].back());
                            }
                        }
                    }

                    if(perms[pos] >= numPossibilities and pos >= 0)
                    {
                        perms[pos] = 0;
                        perms[pos-1]++;
                        pos--;
                    }else
                    if(pos != dfDepth-1 and perms[pos+1] == 0)
                    {
                        pos++;
                    }
                    if(pos == 0 and perms[pos] >= numPossibilities)
                        {
                            parNodeQueue[t].pop_back();
                            done = true;
                            pos = dfDepth-1;
                        }
                }
                //count up the index at the current position
                //if the index at the position gets to numPossibilities, set to 0 and pos--
                //reset pos to numPossibilities and check again

            }
        }
    }
    cout << endl;
    endTime = omp_get_wtime();
    dfTime = endTime-startTime;
    analyzeTree(DFlevel);
    cout << endl;
    cout << "Time spent in parallel depth first was: "<< dfTime << endl;
}

bool Tree::addSequence(double sequence[10], int thread, int dfDepth) //ADDS A GIVEN SEQUENCE DF
{
    int tid = omp_get_thread_num();
    depthFirstQueue[tid].clear();
    depthFirstQueue[tid].push_front(parNodeQueue[tid].back());
    int currentLevel = depthFirstQueue[tid].front()->depth;
    int initialQueueSize = 1;
    int nodesAtLevel = 0;
    for(int i = 0; i < dfDepth-1; i++)
    {
        nodesAtLevel = 0;
        initialQueueSize = depthFirstQueue[tid].size();
        while(depthFirstQueue[tid].front()->depth == currentLevel)
        {
            nodesAtLevel++;
            addPointDF(sequence[i]);
            depthFirstQueue[tid].pop_front();
        }
        if(initialQueueSize-nodesAtLevel == depthFirstQueue[tid].size())
        {
            return true;
        }
        currentLevel++;
    }
    return false;
}

void Tree::destroySubtree(TreeNode* node) //DESTROYS AN UNSUCCESSFUL SUBTREE
{

    if(node->children.size() == 0)
        return;

    list<TreeNode*>::iterator i;
    int tid = omp_get_thread_num();
        for(i = node->children.begin(); i != node->children.end(); i++)
        {
            destroySubtree((*i));
//            cout << nodes[tid].size() << endl;
            nodes[tid].erase((*i)->trueLink);
//            cout << "Erase" << endl;
//            cout << nodes[tid].size() << endl;
            list<TreeNode>::iterator j;
            //for(int t = 0; t < NUM_THREADS; t++)
//            {
//
//                for(j = nodes[tid].begin(); j != nodes[tid].end(); j++)
//                {
//                    if((j)->nId == (*i)->nId)
//                    {
//                        nodes[tid].erase(j);
//                        break;
//                    }
//                }
//            }
        }

    node->children.clear();
}

int Tree::addPointDF(double dPoint)//ADDS A GIVEN POINT TO NODE IN DF PART
{
// The following functions do the same as when exploring full tree, exept they use another queue,
// that is used only for the subtree.
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    //int newChildren = 0;
    switch (depthFirstQueue[tid].front()->content.pointInRange(dPoint))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            nodesAdded = oneChoiceDF(depthFirstQueue[tid].front(), dPoint);
        } break;

        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            nodesAdded = twoChoicesRightDF(depthFirstQueue[tid].front(), dPoint);
        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            nodesAdded = twoChoicesLeftDF(depthFirstQueue[tid].front(), dPoint);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            nodesAdded = threeChoicesDF(depthFirstQueue[tid].front(), dPoint);
        } break;


        case 0 :            //no choice: Must open a new cluster for the point
        {
            nodesAdded = noChoiceDF(depthFirstQueue[tid].front(), dPoint);
        } break;
    }

    return nodesAdded;

}

int Tree::addPointDF_experimental(double dPoint, TreeNode *node)//ADDS A GIVEN POINT TO NODE IN DF PART
{
// The following functions do the same as when exploring full tree, exept they use another queue,
// that is used only for the subtree.
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    //int newChildren = 0;
    switch (node->content.pointInRange(dPoint))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            nodesAdded = oneChoiceDF(node, dPoint);
        } break;

        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            nodesAdded = twoChoicesRightDF(node, dPoint);
        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            nodesAdded = twoChoicesLeftDF(node, dPoint);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            nodesAdded = threeChoicesDF(node, dPoint);
        } break;


        case 0 :            //no choice: Must open a new cluster for the point
        {
            nodesAdded = noChoiceDF(node, dPoint);
        } break;
    }

//    if(node->rootNode){
//        cout << "Returning " << nodesAdded << " nodes, for point " << dPoint << endl;
//    }

    return nodesAdded;

}

bool Tree::backtrackSolution(TreeNode* node, list<double> points, int DFlevel, int succesNumber) //BACKTRACKS TO ROOT OF THE TREE WITH A GIVEN SEQUENCE
{
    //Backtracks through to the root of the tree, and explores to see if
    //all leafs end in ratios higher than threshold
    node->content.listClusters();
    cout << "Nodes ID: "<< node->nId << endl;
    if(node->parentNode == NULL)
    {
        cout << "ITS NULL" << endl;
    }



    FILE * pFile;
    cout << "REACHED BACKTRACK SOLUTION" << endl;
    string sNodeID = node->content.stringIt(node->nId);
    if(node->nId != 1 and node->parentNode != NULL )
    {
        cout << "nID: " << node->nId << endl;
        if(!backtrackSolution(node->parentNode, points, DFlevel, succesNumber))
            return false;
        else
            return true;
    }


    char filename[30];
    sprintf(filename, "res\\%d_%d_solution_%d .gml", resPrefix, omp_get_thread_num(), succesNumber);



    if(node->depth == 0 or node->rootNode)
    {
        cout << "node " << node->nId << " number of children " <<  node->children.size() << endl;
        node->children.front()->content.listClusters();
        node->children.back()->content.listClusters();
        if(print >= 1)
        {

            pFile = fopen (filename,"w"); //cleans the previous file
            fprintf(pFile, "graph \n [ \n");
            fprintf(pFile, "node \n [ \n id n%s \n ] \n ", sNodeID.c_str());
            fclose(pFile);
        }

        if(!forwardCheck(node, points, DFlevel, succesNumber))
        {
            return false;
        }
    }
    if(print >= 1)
    {
        pFile = fopen (filename,"a");
        fprintf(pFile, "]");
        fclose(pFile);
    }
    return true;
}

bool Tree::forwardCheck(TreeNode* node, list<double> points, int DFlevel, int succesNumber) //FROM A GIVEN SEQUENCE, TRIES TO MAKE A FULL PROOF
{
//    cout << "REACHED FORWARD CHECK" << endl;
    if(points.size() == 0)
    {
        return false;
    }

    double point = points.front();
    points.pop_front();

    double difference =  point-points.front();
    int livingChildren = 0;
    list<TreeNode*>::iterator i;

    for(i = node->children.begin(); i != node->children.end(); i++)
    {
        if(deq((*i)->content.difference, difference) and (*i)->depth < DFlevel)
        {

            if(print >= 1)
            {
                (*i)->content.insertNodeLabel(false, (*i)->nId, succesNumber, resPrefix);
            }

            livingChildren++;
            if(!forwardCheck((*i), points, DFlevel, succesNumber))
            {
                return false;
            }
            if(print >= 1)
            {
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);

            }
        }

        if((*i)->depth == DFlevel and deq((*i)->content.difference, difference))
        {
            if((*i)->live == true)
            {
                return false;
            }
            (*i)->live = true;
            if(print >= 1)
            {
                (*i)->content.insertNodeLabel(true, (*i)->nId, succesNumber, resPrefix);
            }

            list<double> newPoints;
            for(int j = 0; j < DFlevel; j++)
                newPoints.push_back(node->workingSequence[j]);

            if(!forwardCheck((*i), newPoints, DFlevel, succesNumber))
            {
                return false;
            }
            if(print >= 1)
            {
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);
            }
        }

        if((*i)->depth > DFlevel)
        {
            if(print >= 1 and (geq((*i)->content.calcRatio(), ratio) or (*i)->children.size() > 0))
            {
                (*i)->content.insertNodeLabel(false, (*i)->nId, succesNumber, resPrefix);
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);
            }
            forwardCheck((*i), points, DFlevel, succesNumber);
        }
    }
    return true;
}

int Tree::twoChoicesLeftDF(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
    int tid = omp_get_thread_num();
    int nodesAdded = 0;

    newNode(tid, dPoint, parent);         //Create a new node, with data from the parent and the new point.
    nodes[tid].front().content.growClusterLeft(dPoint);

    if(checkPartitioning())                             //Check and try to break. If it holds up then:
    {
        nodes[tid].pop_front();                               //remove the node contaminated with extra points from the force
        newNode(tid, dPoint, parent);      //create a new "clean" one.
        nodes[tid].front().content.growClusterLeft(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());       //put it in the queue for points needing attention
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());                //add broken point as a child, but do NOT keep it for future point addition
    }


    newNode(tid, dPoint, parent);          //SEE THE ABOVE COMMENTS, these are the same
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }
//    cout << "Returning " << nodesAdded << endl;
    return nodesAdded;
}

int Tree::twoChoicesRightDF(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
// This is exactly the same as twoChoicesLeft, only with growth to the right insted.
// For further info check above...
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterRight(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterRight(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }

    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }
    return nodesAdded;
}

int Tree::oneChoiceDF(TreeNode *parent, double dPoint) //POINT INSIDE EXISTING CLUSTER
{
// Add point the an existing cluster, and see if it breaks.
// For further explaination see twoChoicesLeft()
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    //cout << "Only one choice... Add point to existing cluster..." << endl;
    //Leave cluster as is, but increase point count
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.addPointToCluster(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
       newNode(tid, dPoint, parent);
        nodes[tid].front().content.addPointToCluster(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());

    }
//    cout << "Returning " << nodesAdded << endl;
    return nodesAdded;
}

int Tree::noChoiceDF(TreeNode *parent, double dPoint)
{
// A new cluster MUST be opened for the point.
// Tries to break it by force and returns 1 if it holds up 0 if it breaks
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    //cout << "Must open new cluster..." << endl;
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }
//    cout << "Returning " << nodesAdded << endl;
    return nodesAdded;
}

int Tree::threeChoicesDF(TreeNode *parent, double dPoint)
{
// Grows right, left and opens a new cluster.
// Returns the number of new nodes where succesfully added
    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    //cout << "Three choices..." << endl;
    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterRight(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterRight(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }

    newNode(tid, dPoint, parent);
    nodes[tid].front().content.growClusterLeft(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.growClusterLeft(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }

    newNode(tid, dPoint, parent);
    nodes[tid].front().content.openCluster(dPoint);

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, dPoint, parent);
        nodes[tid].front().content.openCluster(dPoint);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());
    }
//    cout << "Returning " << nodesAdded << endl;
    return nodesAdded;
}

void Tree::printChildren(TreeNode* node) //PRINTS ALL CHILDREN OF NODE TO TERMINAL (POINTS AND CLUSTERS)
{
    list<TreeNode*>::iterator i;


    cout << endl;
    if(node->children.size() == 0)
    {
        cout << " NODE HAS NO CHILDREN" << endl;
    }
    for(i = node->children.begin(); i != node->children.end(); i++)
    {
        cout << "ID: " << (*i)->nId << endl;
        (*i)->content.listPoints();
        (*i)->content.listClusters();
    }
    cout << endl;
}

void Tree::subtreePrinter(TreeNode* node, int succesNumber) //PRINTS A NODES SUBTREE TO A FILE
{
            cout << "Writing node" << endl;
            node->content.insertNodeLabel(false, node->nId, succesNumber, resPrefix);

            cout << "Number of children " << node->children.size() << endl;
            list<TreeNode*>::iterator i;
            for(i = node->children.begin(); i != node->children.end(); i++)
            {
                subtreePrinter((*i), succesNumber);
            }

            cout <<  node->nId << " has parent " << node->parentNode->nId << endl;

            cout << "Writing edge" << endl;
            node->content.insertEdgeLabel(node->parentNode->nId, node->nId, succesNumber, resPrefix);
}

void Tree::initPrinter(int succesNumber)
{
    FILE *pFile;
    char filename[30];

    sprintf(filename, "res\\%d_%d_solution_%d .gml", resPrefix, omp_get_thread_num(), succesNumber);

    pFile = fopen (filename,"w");

     fprintf(pFile,"graph \n");
     fprintf(pFile," \[ \n");

    fclose(pFile);
}

void Tree::closePrinter(int succesNumber)
{
    FILE *pFile;
    char filename[30];

    sprintf(filename, "res\\%d_%d_solution_%d .gml", resPrefix, omp_get_thread_num(), succesNumber);

    pFile = fopen (filename,"a");

     fprintf(pFile,"\]");

    fclose(pFile);
}


/*********************************
*** PARALLEL HELPER FUNCTIONS ****
*********************************/

int Tree::splitNodeQueue(int numberOfThreads) //SPLITS THE NODEQUEUE INTO CHUNKS - 1 FOR EACH THREAD
{
    list<TreeNode*>::iterator i, first, last;
    first = nodeQueue.begin();
    last = nodeQueue.end();
    int sizeOfQueue = nodeQueue.size();
    int counter = 0;
    int listsMade = 1;


    //calculate splitpoints
    int splits = sizeOfQueue/numberOfThreads;

    cout << "Queue of " << nodeQueue.size() << " is split into " << numberOfThreads << " of " << splits << endl;


    for(i = nodeQueue.begin(); i != nodeQueue.end(); i++)
    {

        if(listsMade == numberOfThreads)
        {
            nodeQueue.splice(parNodeQueue[listsMade-1].begin(), parNodeQueue[listsMade-1], first, last);
            goto finish;
        }

        if(counter == splits*listsMade)
        {

            nodeQueue.splice(parNodeQueue[listsMade-1].begin(), parNodeQueue[listsMade-1], first, i);
            first = i;
            listsMade++;
        }
        counter++;

    }

    finish:

    for(int i = 0; i < numberOfThreads; i++)
    {
        cout << "Size of the parallel queue " << i << " " << parNodeQueue[i].size() << endl;
    }

    return 0;

}

int Tree::splitSuccesQueue(int numberOfThreads) //SPLITS SUCCESQUEUE INTO CHUNKS - 1 FOR EACH THREAD
{
    list<TreeNode*>::iterator i, first, last;
    first = succesfulNodes.begin();
    last = succesfulNodes.end();
    int sizeOfQueue = succesfulNodes.size();
    int counter = 0;
    int listsMade = 1;


    //calculate splitpoints
    int splits = sizeOfQueue/numberOfThreads;

    cout << "SuccesQueue of " << succesfulNodes.size() << " is split into " << numberOfThreads << " of " << splits << endl;


    for(i = succesfulNodes.begin(); i != succesfulNodes.end(); i++)
    {

        if(listsMade == numberOfThreads or splits == 0)
        {
            succesfulNodes.splice(parSuccesfulNodes[listsMade-1].begin(), parSuccesfulNodes[listsMade-1], first, last);
            goto finish;
        }

        if(counter == splits*listsMade)
        {

            succesfulNodes.splice(parSuccesfulNodes[listsMade-1].begin(), parSuccesfulNodes[listsMade-1], first, i);
            first = i;
            listsMade++;
        }
        counter++;

    }

    finish:

    for(int i = 0; i < numberOfThreads; i++)
    {
        cout << "Size of the parallel succesfulNodesQueue " << i << " " << parSuccesfulNodes[i].size() << endl;
    }

    return 0;

}



/*********************************
****** SEQUENCE-CHECKER **********
*********************************/
void Tree::checkSequence(list<double> sequence) //DOES THE CHECKING OF A PRESET SEQUENCE
{
    FILE *pFile;

    char filename[30];
    sprintf(filename, "res\\%d_%d_solution_%d .gml", 12345, omp_get_thread_num(), 1);

    pFile = fopen (filename,"w"); //cleans the previous file
            fprintf(pFile, "graph \n [ \n");
            fclose(pFile);

    seqQueue.front()->content.insertNodeLabel(false, seqQueue.front()->nId, 1, 12345);
    list<double>::iterator i;
    int currentLevel = 0;
    for(i = sequence.begin(); i != sequence.end(); i++)
    {
        currentLevel = seqQueue.front()->depth;
        while(seqQueue.front()->depth == currentLevel)
        {
            addSeqPoint((*i));
            seqQueue.pop_front();
            if(seqQueue.size() == 0)
            {
                cout << "DID IT!" << endl;
                pFile = fopen (filename,"a");
                fprintf(pFile, "]");
                fclose(pFile);
                exit(1);
            }

        }
    }

    pFile = fopen (filename,"a");
        fprintf(pFile, "]");
        fclose(pFile);

    cout << "Sorry, didn't do it.... :(" << endl;
    cout << "" << endl;
    cout << "" << endl;

    list<TreeNode*>::iterator it;
    it = seqQueue.begin();
    //nodeQueue.splice(parNodeQueue[listsMade-1].begin(), parNodeQueue[listsMade-1], first, last);
    seqQueue.splice(nodeQueue.begin(), seqQueue);
    cout << "NodeQueue Size " << nodeQueue.size() << endl;
    cout << "SeqQueue Size " << seqQueue.size() << endl;

    cout << "-----------------------------------" << endl;
    cout << endl;
    //exit(1);
}

bool Tree::checkSeqPartitioning() //CHECK RATIO AND TRY TO FORCE (FOR SEQUENCER)
{
    if(seqNodes.front().content.calcRatio() > ratio)
    {
        return false;
    }
    return seqNodes.front().content.force(ratio);
}

void Tree::addSeqPoint(double dPoint) //ADDS A POINT IN THE SEQUENCE PART
{
    switch (seqQueue.front()->content.pointInRange(dPoint))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            oneChoiceSeq(seqQueue.front(), dPoint);
        } break;
        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            twoChoicesRightSeq(seqQueue.front(), dPoint);
        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            twoChoicesLeftSeq(seqQueue.front(), dPoint);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            threeChoicesSeq(seqQueue.front(), dPoint);
        } break;

        case 0 :            //no choice: Must open a new cluster for the point
        {
            noChoiceSeq(seqQueue.front(), dPoint);
        } break;
    }

}

void Tree::twoChoicesLeftSeq(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
    cout << "In Two Choices Left" << endl;
    seqNodes.push_front(TreeNode(dPoint, parent));          //Create a new node, with data from the parent and the new point.
    seqNodes.front().content.growClusterLeft(dPoint);

    if(checkSeqPartitioning())                                 //Check and try to break. If it holds up then:
    {
        cout << "Point held up" << endl;
        seqNodes.pop_front();                               //remove the node contaminated with extra points from the force
        seqNodes.push_front(TreeNode(dPoint, parent));      //create a new "clean" one.
        seqNodes.front().content.growClusterLeft(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());              //put it in the queue for points needing attention
    }
    else
    {
        cout << "Broke partitioning" << endl;
        parent->addChild(&seqNodes.front());                //add broken point as a child, but do NOT keep it for future point addition
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);

    seqNodes.push_front(TreeNode(dPoint, parent));          //SEE THE ABOVE COMMENTS, these are the same
    seqNodes.front().content.openCluster(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.openCluster(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);
}

void Tree::twoChoicesRightSeq(TreeNode *parent, double dPoint)
{
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
// This is exactly the same as twoChoicesLeft, only with growth to the right insted.
// For further info check above...
    cout << "In Two Choices Right" << endl;
    seqNodes.push_front(TreeNode(dPoint, parent));
    seqNodes.front().content.growClusterRight(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.growClusterRight(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);

    seqNodes.push_front(TreeNode(dPoint, parent));
    seqNodes.front().content.openCluster(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.openCluster(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);
}

void Tree::oneChoiceSeq(TreeNode *parent, double dPoint)
{
// Add point the an existing cluster, and see if it breaks.
// Returns 1 if the point was succesfully added
// For further explaination see twoChoicesLeft()
    cout << "In One Choice" << endl;
    seqNodes.push_front(TreeNode(dPoint, parent));
    seqNodes.front().content.addPointToCluster(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.addPointToCluster(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);
}

void Tree::noChoiceSeq(TreeNode *parent, double dPoint)
{
// A new cluster MUST be opened for the point.
// Tries to break it by force and returns 1 if it holds up 0 if it breaks
    cout << "In No Choice" << endl;
    seqNodes.push_front(TreeNode(dPoint, parent));          //SEE THE ABOVE COMMENTS, these are the same
    seqNodes.front().content.openCluster(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.openCluster(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);
}

void Tree::threeChoicesSeq(TreeNode *parent, double dPoint)
{
// Grows right, left and opens a new cluster.
// Returns the number of new nodes where succesfully added
    cout << "In Three Choices" << endl;
    seqNodes.push_front(TreeNode(dPoint, parent));
    seqNodes.front().content.growClusterRight(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.growClusterRight(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);

    seqNodes.push_front(TreeNode(dPoint, parent));          //Create a new node, with data from the parent and the new point.
    seqNodes.front().content.growClusterLeft(dPoint);

    if(checkSeqPartitioning())                             //Check and try to break. If it holds up then:
    {
        seqNodes.pop_front();                               //remove the node contaminated with extra points from the force
        seqNodes.push_front(TreeNode(dPoint, parent));      //create a new "clean" one.
        seqNodes.front().content.growClusterLeft(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());       //put it in the queue for points needing attention
    }
    else
    {
        parent->addChild(&seqNodes.front());                //add broken point as a child, but do NOT keep it for future point addition
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);

    seqNodes.push_front(TreeNode(dPoint, parent));          //SEE THE ABOVE COMMENTS, these are the same
    seqNodes.front().content.openCluster(dPoint);

    if(checkSeqPartitioning())
    {
        seqNodes.pop_front();
        seqNodes.push_front(TreeNode(dPoint, parent));
        seqNodes.front().content.openCluster(dPoint);
        parent->addChild(&seqNodes.front());
        seqQueue.push_back(&seqNodes.front());
    }
    else
    {
        parent->addChild(&seqNodes.front());
    }
    seqNodes.front().content.insertNodeLabel(false, seqNodes.front().nId, 1, 12345);
    seqNodes.front().content.insertEdgeLabel(seqNodes.front().parentNode->nId, seqNodes.front().nId, 1, 12345);
}



/*********************************
****** EXPERIMENTAL DF ***********
*********************************/
bool Tree::addPointRecursive(int level, int maxLevel)
//Returns false if a subproof is found

{
//    if(nodes[0].size()%1000 == 0)
//    cout << "Number of nodes " << nodes[0].size() << endl;

    int tid = omp_get_thread_num();
    int nodesAdded = 0;
    list<double> nextPoints;
    list<TreeNode*>::iterator nit;
    list<double>::iterator pit;

    ///If reached the maxlevel, just return true (FAIL)
    if(level == maxLevel)
    {
        //cout << "All the way down..." << endl;
        return true;
    }


    for(nit = depthFirstQueue[tid].begin(); nit != depthFirstQueue[tid].end(); nit++ )
    {
        if((*nit)->depth == level)
        {
            nextPoints = calculateNextPoints(*nit);
            break;
        }
    }

    if(level == 11){
        cout << "Number of poinst to go through " << nextPoints.size() << endl;
    }
    for(pit = nextPoints.begin(); pit != nextPoints.end(); pit++)
    {
        nodesAdded = 0;
        nodesAdded = addLevel(level, maxLevel, *pit);
        if(level == 11){
            cout << "Next point: "<< (*pit) << endl;
        }
        if(nodesAdded == 0)
        {
            cout << "NO NEW POINTS ADDED ON LEVEL "<< level << endl;
            return false;
        }
        else
            if(!addPointRecursive(level+1, maxLevel)){
                cout << "How often does this happen?" << endl;
                // ->clear next level for unsuccessful nodes, that are not to be continued.
//                clearLevel(level+1, (*pit));
                return false;
            }
        clearLevel(level+1, 100);
    }
    return true;
}

int Tree::addLevel(int level,int maxLevel, double point)
{
    int nodesAdded = 0;
    int tid = omp_get_thread_num();
    list<TreeNode*>::iterator nit;

    for(nit = depthFirstQueue[tid].begin(); nit != depthFirstQueue[tid].end(); nit++)
    {
        if((*nit)->depth == level)
        {
            nodesAdded += addPointDF_experimental(point, (*nit));
        }
    }


    return nodesAdded;
}

void Tree::clearLevel(int level, double successPoint)
{

    int tid = omp_get_thread_num();
//    if(successPoint >= 100){
        list<TreeNode*>::iterator nit = depthFirstQueue[tid].begin();
        list<TreeNode*>::iterator pit;
        int start = depthFirstQueue[tid].size();

        while(nit != depthFirstQueue[tid].end())
        {
            if((*nit)->depth == level)
            {
//                if(deq(successPoint, 100)){
//                    for(pit = (*nit)->parentNode->children.begin(); pit != (*nit)->parentNode->children.end(); pit++){
//    //                        cout << "Will this fail immideately? " << endl;
//                            (*nit)->parentNode->children.erase(pit);
//                    }
//                }   else{
                destroySubtree((*nit));
    //            nodes[0].erase((*nit)->trueLink);
                nit = depthFirstQueue[tid].erase(nit);
            }
//        }
        else
        {
            ++nit;
        }
        }
//    }
//    else {
//        list<TreeNode*>::iterator nit = depthFirstQueue[tid].begin();
//        int start = depthFirstQueue[tid].size();
//
//        while(nit != depthFirstQueue[tid].end())
//        {
//            if((*nit)->depth == level and (!deq((*nit)->content.points.back(), successPoint)))
//            {
//
//
//    //            (*nit)->parentNode->children.clear();
//                nodes[0].erase((*nit)->trueLink);
//                nit = depthFirstQueue[tid].erase(nit);
//            }
//            else
//            {
//                ++nit;
//            }
//        }
//    }

}

list<double> Tree::calculateNextPoints(TreeNode *node)
{

    list<double> nextPoints;
    list<double>::iterator it;
//    node->content.listPoints();
//    cout << "Calculating possible points... " << endl;
//    cout << "They are: ";
    for(it = node->content.sortedPoints.begin(); it != node->content.sortedPoints.end(); it++)
    {
        for(int k = 0; k < deltas; k++)
        {
            //debugging
            //double point = nodeQueue.front()->content.doesPointExist((*j) + delta[k]);
//            cout << "Does " << (*it) + delta[k] << " exist?" << endl;
            if(node->content.doesPointExist((*it) + delta[k]))
            {
//                cout << "no it doesn't" << endl;
                nextPoints.push_front((*it)+delta[k]);
            }
        }
    }
    nextPoints.sort();
    nextPoints.unique();
    nextPoints.reverse(); ///negative numbers makes it normalize the list, so if there is a proof in the positives we want it as fast as possible..

//    cout << "Points sorted out for node: "<< node->nId << endl;
//    for (it = nextPoints.begin(); it != nextPoints.end(); it++)
//    {
//        cout << *it << " ";
//
//    }
//    cout << "" << endl;
    return nextPoints;
}

void Tree::removeChildren(int tid, TreeNode* node)
{

    if(node->children.size() == 0)
    {
        return;
    }
    list<TreeNode*>::iterator child, nq;

//    for (child = node->children.begin(); child != node->children.end() ; child++ )
//    {
//        depthFirstQueue[tid].erase((*child)->queueLink);
//        nodes[tid].erase((*child)->trueLink);
//    }

    for(child = node->children.begin(); child != node->children.end(); child++)
    {
        for(nq = depthFirstQueue[tid].begin(); nq != depthFirstQueue[tid].end(); nq++)
        {
            if((*nq)->nId == (*child)->nId)
            {
                cout << "Child ID: " << (*child)->nId <<"NodeQueue Node ID: "<< (*nq)->nId << endl;
                cout << "deleting child from queue" << endl;
                depthFirstQueue[tid].erase(nq);
            }
        }
    }
    node->children.clear();
}

void Tree::sequenceTree()
{
    cout << "size of nodeQueue: " << nodeQueue.size() << endl;
    list<TreeNode*>::iterator nit = nodeQueue.begin();
    string currentHash = nodeQueue.front()->content.pointHash;
    sequencedTreeQueue.push_back(list<TreeNode*>());
    while(nit != nodeQueue.end())
    {
            if(currentHash.compare((*nit)->content.pointHash) == 0)
            {
                sequencedTreeQueue.back().push_back(*nit);
            }
            else{

                sequencedTreeQueue.push_back(list<TreeNode*>());
                sequencedTreeQueue.back().push_back(*nit);
                currentHash = (*nit)->content.pointHash;
            }
            nit++;
    }
    cout << "size of nodeQueue: " << nodeQueue.size() << endl;
    //print the sizes of the mini-lists
    list<list<TreeNode*>>::iterator it = sequencedTreeQueue.begin();

//    while(it != sequencedTreeQueue.end())
//    {
//        cout << " " << (*it).size();
//        it++;
//    }
    sequencedTreeQueue.sort(list_compare);
//    cout << "" << endl;
//    cout << "" << endl;
//    it = sequencedTreeQueue.begin();
//    while(it != sequencedTreeQueue.end())
//    {
//        cout << " " << (*it).size();
//        it++;
//   }
//    cout << "Out" << endl;
    nodeQueue.clear();
    cout << "size of nodeQueue (should be 0) : " << nodeQueue.size() << endl;
    for(it = sequencedTreeQueue.begin(); it != sequencedTreeQueue.end(); it++)
    {
        for(nit = (*it).begin(); nit != (*it).end(); nit++)
        {
            nodeQueue.push_back(*nit);
        }
    }
    cout << "size of nodeQueue: " << nodeQueue.size() << endl;
    cout << "size of other list: "<< sequencedTreeQueue.front().size() << endl;
    cout << "Size of the sequence list: " << sequencedTreeQueue.size() << endl;

}
bool list_compare(const list<TreeNode*> first,const list<TreeNode*> second)
{

    if (first.size() >= second.size())
    {
        return false;
    }
    if (first.size() < second.size())
        return true;
}





