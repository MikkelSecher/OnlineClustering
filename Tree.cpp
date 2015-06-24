#include "Tree.h"

using namespace std;

/*********************************/
/********** CONSTRUCTORS *********/
/*********************************/


Tree::Tree(double dStartingPoint, list<double> deltaPoints, int prefix, double ratioin, int printin){
// Creates a root with one point
//    int worldRank     = omp_get_thread_num();
    deltas      = deltaPoints.size();
    resPrefix   = prefix;
    print       = printin;
    ratio       = ratioin;

    for( int i = 0 ; i < deltas ; i++ ){
        delta.push_back( deltaPoints.back() );
        deltaPoints.pop_back();
    }

    nodes[ worldRank ].push_front( TreeNode(dStartingPoint, NULL) );
    nodeQueue.push_back( &nodes[worldRank].front() );
    nodeQueue.front()->parentNode = NULL;
    rootNode = &nodes[worldRank].front();

}

Tree::Tree(list<double> sequence, int prefix, double ratioIn){
//Creates a tree from a sequence, and prints the tree to a GML file
//    int worldRank     = omp_get_thread_num();

    resPrefix   = prefix;
    print       = 2;
    ratio       = ratioIn;

    list<double>::iterator  pointIt = sequence.begin();

    nodes[0].push_front( TreeNode( (*pointIt), NULL) );
    nodeQueue.push_back( &nodes[0].front() );
    nodeQueue.front()->parentNode = NULL;
    rootNode = &nodes[0].front();
    int level = 0;
    ++pointIt;
    for (pointIt; pointIt != sequence.end() ; pointIt++ ){
        level++;
        list<TreeNode*>::iterator nodeIt;

        for(nodeIt = nodeQueue.begin() ; nodeIt != nodeQueue.end() ; nodeIt++ ){
            if( (*nodeIt)->depth == level-1 ) {
                addPoint( 0, (*pointIt) );
                nodeIt = nodeQueue.erase( nodeIt );
                nodeIt--;
            }
        }
    }
    FILE *pFile;
    char filename[30];
    sprintf( filename , "res\\%d_%d_solution_%d .gml" , resPrefix , worldRank , 10000 );

    pFile = fopen (filename,"w"); //cleans the previous file
    fprintf(pFile, "graph \n [ \n");

    fclose(pFile);

    printFullTreeToFile(rootNode);

    pFile = fopen (filename,"a");
    fprintf(pFile, "]");
    fclose(pFile);

    list<TreeNode>::iterator nodeIt = nodes[0].begin();
}


/*********************************/
/************** IO ***************/
/*********************************/

void Tree::printFullTreeToFile(TreeNode *node){

    list<TreeNode*>::iterator childIt = node->children.begin();

    node->content.insertNodeLabel(false, node->nId, 10000, resPrefix);

    if(!node->rootNode){
        node->content.insertEdgeLabel(node->parentNode->nId, node->nId, 10000, resPrefix);

    }
    if(!node->children.empty()){
        for (childIt ; childIt != node->children.end() ; childIt++ ){
            printFullTreeToFile( (*childIt) );
        }
    }
    return;
}

//Creates a new node
TreeNode* Tree::newNode( int worldRank, double point, TreeNode* parent ){
    nodes[worldRank].push_front( TreeNode(point, parent) );
    nodes[worldRank].front().trueLink = nodes[worldRank].begin();

    return &nodes[worldRank].front();
}

void Tree::destroyNode(TreeNode *node){
//    int worldRank = omp_get_thread_num();
    nodes[worldRank].erase(node->trueLink);
}

/*********************************/
/***** HANDLERS FOR NEW POINTS ****/
/*********************************/

void Tree::addLevelBF(int level){
// For an entire level in the tree, this calculates all the points to be added to each
// node, based on the delta values in tree.h.
// If a point already exists in a node, it is skipped.

    if(nodeQueue.empty()){
        cout << "No queue to work from! " << endl;
        exit(0);
    } else {
        list<TreeNode*>::iterator nodeIt;
        list<double>::iterator nextPoint;
        list<double> pointsToAdd;

        for(nodeIt = nodeQueue.begin() ; nodeIt != nodeQueue.end() ; nodeIt++ )
        {
            if( (*nodeIt)->depth == level-1 ) {

                pointsToAdd = getNextPoints( *nodeIt );

                for( nextPoint = pointsToAdd.begin() ; nextPoint != pointsToAdd.end() ; nextPoint++ ){
                    addPoint( 0, (*nextPoint) );
                }

                pointsToAdd.clear();

                nodeIt = nodeQueue.erase( nodeIt );
                nodeIt--;
            }
        }
    }
}

bool Tree::checkPartitioning(){
// Checks ratio for a partitioning
// Returns FALSE if the ratio is above the threshold
// Also tries to break the partitioning by forcing new points
//    int worldRank = omp_get_thread_num();

    TreeNode *workingNode = &nodes[worldRank].front();
    Partitioning *workingPartitioning = &workingNode->content;

    if( geq( workingPartitioning->calcRatio(), ratio ) )
    {
        return false;
    }
    if( nodes[worldRank].front().depth == 0 ){
        return true;
    }

    workingPartitioning->setAmbSpace();
    list<double> ambPoints = workingPartitioning->getAmbPoints();

    if( !force(workingNode , workingNode , workingPartitioning->getForcePoints( ), ambPoints) ){

        return false;
    }
    return true;
}

bool Tree::force(TreeNode *node, TreeNode *originalNode, list<double> forcePoints, list<double> ambPoints){ ///TODO: Cleanup
//    int worldRank = omp_get_thread_num();

    list<double> emptyList;
    TreeNode* forceNode;

    //If there are no forcepoints
    if( forcePoints.empty()){
        while( !ambPoints.empty() ){//As long as there is still ambiguous points
            if( !node->doesPointExist( ambPoints.front() ) ){//Check that the point doesn't already exist (safety)
                ambPoints.pop_front();
                continue;
            }
            ///Get a new node to work on
            forceNode = newNode( worldRank, ambPoints.front(), node );
            forceNode->openCluster( ambPoints.front(), true );

            //If opening the new cluster breaks the ratio, it means that
            //there is only one choice -> expand the current cluster
            if(geq(forceNode->calcRatio(), ratio)){
                //Erase the node that broke the ratio
                destroyNode(forceNode);
                //And make a new one
                forceNode = newNode( worldRank, ambPoints.front(), node );

                int pointInRange = forceNode->pointInRange( ambPoints.front() );

                if(pointInRange == 2 || pointInRange == -2) {

                    if(pointInRange == 2){ //Grow the cluster
                        forceNode->growClusterRight( ambPoints.front(), true );
                    } else {
                        forceNode->growClusterLeft( ambPoints.front(), true );
                    }

                    forcePoints = forceNode->getForcePoints(); //Get new forcePoints
                    emptyList = ambPoints; //Get ready to call the force with an empty ambPoints list
                    emptyList.remove( ambPoints.front() );

                    if( !force( forceNode, originalNode, forcePoints, emptyList ) ) {//Call it
                        //If it is now forcable, return false (Success)
                        node->content.forced = true;
                        node->live = false;

                        if( pointInRange == 2 ){ //Update the original node with the expanded cluster
                            originalNode->growClusterRight( ambPoints.front(), true );
                        } else {
                            originalNode->growClusterLeft( ambPoints.front(), true );
                        }

                        destroyNode( forceNode );  //Erase the forceNode from memory
                        node->children.clear(); //Clear the nodes children

                        return false;
                    }
                }
            }

            destroyNode( forceNode ); //If this doesn't work, erase the node worked on
            ambPoints.pop_front(); //And try with the next ambiguous point
        }
        return true;//When there are no more points return true (FAIL)
    }

    forcePoints = node->getForcePoints(); //Get the current forcepoints for the node

    list<double>::iterator forcePointIt; //For each of the forcePoints
    for( forcePointIt = forcePoints.begin() ; forcePointIt != forcePoints.end() ; forcePointIt++ ) {

        if( !node->doesPointExist( (*forcePointIt) ) ) { //Check that the point doesn't already exist (safety)

             continue;
           }
        forceNode = newNode( worldRank, *forcePointIt, node ); //Create a new node to force with
        forceNode->openCluster( *forcePointIt ); //And open a forced cluster


        if(geq(forceNode->calcRatio(), ratio) || //If the ratio is broken by this
        !force(forceNode, originalNode, forcePoints, ambPoints ) || //Else call again with the forcepoint list
        !force( forceNode, originalNode, emptyList, ambPoints ) ){ //Else try with ambiguous points
            //If successful
            node->content.forced = true;
            node->live = false;

            originalNode->openCluster( *forcePointIt , true );

            destroyNode( forceNode );
            node->children.clear();

            return false;
        } else { //Else clean up and return true (FAIL)
            destroyNode( forceNode );
            return true;
        }
        originalNode->children.clear();
    }

    //cout << "Could not force at all "<< endl;
    return true;
}

list<double> Tree::getNextPoints(){
    return getNextPoints( nodeQueue.front() );
}

list<double> Tree::getNextPoints( TreeNode *node ){
    list<double> nextPoints;
    list<double>::iterator it;

    for( it = node->content.sortedPoints.begin(); it != node->content.sortedPoints.end(); it++)
    {
        for( int k = 0; k < deltas; k++ )
        {
            if( node->doesPointExist( (*it) + delta[k]) )
            {
                nextPoints.push_front( (*it)+delta[k] );
            }
        }
    }
    nextPoints.sort();
    nextPoints.unique();
    nextPoints.reverse(); ///negative numbers makes it normalize the list, so if there is a proof in the positives we want it as fast as possible..

    return nextPoints;
}


/*********************************/
/********* TREE CLEANUP **********/
/*********************************/

bool node_compare( const TreeNode *first,const TreeNode *second ) { ///TODO: Test
    if (first->content.fullHash.compare(second->content.fullHash) >= 0){
        return false;
    }
    if(first->content.fullHash.compare(second->content.fullHash) < 0){
        return true;
    }
    return false;
}

bool node_compare_sequence( const TreeNode *first,const TreeNode *second ) { ///TODO: Test
    if (first->content.fullHash.compare(second->content.pointHash) >= 0){
        return false;
    }

    if(first->content.fullHash.compare(second->content.pointHash) < 0){
        return true;
    }
    return false;
}


bool node_pointscompare(const TreeNode *first,const TreeNode *second){ ///TODO: Test

    if (first->content.pointHash.compare(second->content.pointHash) == 0){
        if(first->nId < second->nId){
            return true;
        }
        return false;
    }

    if(first->content.pointHash.compare(second->content.pointHash) < 0){
        return true;
    } else {
        return false;
    }
}


void Tree::treeCleanup() {
    // Normalizes the points in each node by shifting everything to start out at 0
    // After all nodes have been normalized, they are compared and duplicates are removed
//    cout << "before cleanup: " << nodeQueue.size() << endl;

    nodeQueue.sort(node_compare);

    list<TreeNode*>::iterator firstNode, secondNode;

    for(firstNode = nodeQueue.begin(); firstNode != nodeQueue.end(); firstNode++){
        for(secondNode = next(firstNode); secondNode != nodeQueue.end(); secondNode++){
            if(firstNode != secondNode){
                if((*firstNode)->content.fullHash.compare((*secondNode)->content.fullHash) == 0){
                    secondNode = nodeQueue.erase(secondNode);
                }else{
                    break;
                }
            }
        }
    }
//    cout << "after cleanup: " << nodeQueue.size() << endl;
}

void Tree::normalize(TreeNode *node){
// Shifts points, sortedPoints and the clusters, to start from 0
    if ( leq( node->content.points.back() , 0) ) {
        double offset = 0;
        offset = offset - node->content.adClusters[0][0];

        list<double>::iterator i;

        for(i = node->content.sortedPoints.begin(); i != node->content.sortedPoints.end(); i++){
            *i = *i+offset;
        }
        for(i = node->content.points.begin(); i != node->content.points.end(); i++){
            *i = *i+offset;
        }
        for(int i = 0; i < node->content.nNumberOfClusters; i++){
            node->content.adClusters[i][0] = ((node->content.adClusters[i][0])+offset);
            node->content.adClusters[i][1] = ((node->content.adClusters[i][1])+offset);
        }
    }
    node->content.updateHashes();
}



/*********************************/
/**** BREADTH-FIRST FUNCTIONS ****/
/*********************************/

void Tree::addPoint(int p, double point) {
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.
    TreeNode *parent = nodeQueue.front();

    switch (parent->content.pointInRange(point))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            oneChoice(parent, point);
        } break;
        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            twoChoicesRight(parent, point);

        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            twoChoicesLeft(parent, point);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            threeChoices(parent, point);
        } break;

        case 0 :            //no choice: Must open a new cluster for the point
        {
            noChoice(parent, point);
        } break;
    }
}

void Tree::twoChoicesLeft(TreeNode *parent, double point){
    // Tries to add a point by growing a cluster to the left, and trying to break it by force
    // and then tries to open a new cluster and tries to break it by force

//    int worldRank = omp_get_thread_num();

    growClusterLeftBF(parent, point, worldRank);
    openClusterBF(parent, point, worldRank);
}

void Tree::twoChoicesRight(TreeNode *parent, double point){
    // Tries to add a point by growing a cluster to the left, and trying to break it by force
    // and then tries to open a new cluster and tries to break it by force
    // This is exactly the same as twoChoicesLeft, only with growth to the right insted.
//    int worldRank = omp_get_thread_num();

    growClusterRightBF(parent, point, worldRank);
    openClusterBF(parent, point, worldRank);
}

void Tree::oneChoice(TreeNode *parent, double point){
    // Add point the an existing cluster, and see if it breaks.
    // For further explaination see twoChoicesLeft()

//    int worldRank = omp_get_thread_num();

    addToClusterBF(parent, point, worldRank);
}

void Tree::noChoice(TreeNode *parent, double point){
    // A new cluster MUST be opened for the point.
    // Tries to break it by force and returns 1 if it holds up 0 if it breaks
//    int worldRank = omp_get_thread_num();

    openClusterBF(parent, point, worldRank);
}

void Tree::threeChoices(TreeNode *parent, double point){
    // Grows right, left and opens a new cluster.
    // Returns the number of new nodes where succesfully added
//    int worldRank = omp_get_thread_num();

    growClusterRightBF(parent, point, worldRank);
    growClusterLeftBF(parent, point, worldRank);
    openClusterBF(parent, point, worldRank);

}

int Tree::openClusterBF(TreeNode *parent, double point, int worldRank){


    newNode( worldRank, point, parent );
    nodes[worldRank].front().openCluster( point);

    if(checkPartitioning()){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[worldRank].pop_front();
        newNode( worldRank, point, parent );
        nodes[worldRank].front().openCluster( point);
        //Add it as a child to the parent node
        parent->addChild( &nodes[worldRank].front() );
        //Set live to false - needed for later...
        nodeQueue.push_back( &nodes[worldRank].front() );
        nodes[worldRank].front().queueLink = nodeQueue.end();
    }else{
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[worldRank].front());
        //Set live to false - needed for later...
        nodes[worldRank].front().live = false;
    }

    normalize(&nodes[worldRank].front());
    nCount++;
    return 0;
}

int Tree::growClusterRightBF(TreeNode *parent, double point, int worldRank){
    newNode(worldRank, point, parent);
    nodes[worldRank].front().growClusterRight( point);

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[worldRank].pop_front();
        newNode(worldRank, point, parent);
        nodes[worldRank].front().growClusterRight(point);

        //Add it as a child to the parent node
        parent->addChild( &nodes[worldRank].front() );

        //Add it the the node queue
        nodeQueue.push_back( &nodes[worldRank].front() );
        nodes[worldRank].front().queueLink = nodeQueue.end();
    } else {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[worldRank].front());
        //Set live to false - needed for later...
        nodes[worldRank].front().live = false;
    }

    normalize(&nodes[worldRank].front());
    return 0;
}

int Tree::growClusterLeftBF(TreeNode *parent, double point, int worldRank){
    newNode( worldRank, point, parent );
    nodes[worldRank].front().growClusterLeft( point );

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[worldRank].pop_front();
        newNode(worldRank, point, parent);
        nodes[worldRank].front().growClusterLeft( point );

        //Add it as a child to the parent node
        parent->addChild( &nodes[worldRank].front() );

        //Add it the the node queue
        nodeQueue.push_back( &nodes[worldRank].front() );
        nodes[worldRank].front().queueLink = nodeQueue.end();
    } else {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild( &nodes[worldRank].front() );
        //Set live to false - needed for later...
        nodes[worldRank].front().live = false;
    }

    normalize(&nodes[worldRank].front());
    return 0;

}

int Tree::addToClusterBF(TreeNode *parent, double point, int worldRank){

    newNode(worldRank, point, parent);
    nodes[worldRank].front().addPointToCluster( point );

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[worldRank].pop_front();
        newNode(worldRank, point, parent);
        nodes[worldRank].front().addPointToCluster(point);
        //Add it as a child to the parent node
        parent->addChild(&nodes[worldRank].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[worldRank].front());
        nodes[worldRank].front().queueLink = nodeQueue.end();
    } else {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[worldRank].front());
        //Set live to false - needed for later...
        nodes[worldRank].front().live = false;
    }

    normalize(&nodes[worldRank].front());
    return 0;
}

/*********************************/
/****** DEPTH-FIRST FUNCTIONS ****/
/*********************************/

bool Tree::startDF(int levelsOfBF, int levelsOfDF) {
    if( nodeQueue.size() > 1 ){
        nodeQueue.sort( node_compare_sequence ); //Sort queue wrt. sequence of points
    }
    cout << "Size of queue: " << endl;

    sequenceTree(); //sort queue into miniqueues sorted by sequence of points
    int proofsToTry = sequencedTreeQueue.size();
    splitSequenceTree();


//    cout << "Possibilities for proofs: " << proofsToTry << endl;
//    listInitializeTextFile(ratio, worldRank);

//    int startTime = omp_get_wtime();
//
//    int offsetForNode = numberOfMiniQueues/worldSize;
//    int myStartingPoint = offsetForNode*worldRank;
//    int myEndPoint = myStartingPoint+offsetForNode-1;
//
//    myStartingPoint = 0;

    for(int miniQueue = 0; miniQueue < numberOfMiniQueues; miniQueue++){

//        cout << "Checked " << miniQueue << " from node " << worldRank << endl;
        if(!miniQueueDF(parallelMiniQueues[miniQueue], levelsOfBF, levelsOfDF) ) {

                listProofSequenceToTextFile( proofSequences[worldRank].back(), worldRank);
//                cout << "Solution found from " << worldRank << " miniQueue number: "<< miniQueue << endl;
        }
    }
//    cout << "Checked " << myStartingPoint << " to " << myEndPoint << " out of " << proofsToTry << " at process " << worldRank << endl;
//    cout << "Time spent in parallel: " << omp_get_wtime() - startTime << endl;
//    dfTime = omp_get_wtime() - startTime;
        normalizeSolutions();
//        printSolutionSequences();

        for(int i = 0; i < NUM_THREADS; i++){
//            listProofsToFiles(proofSequences[i], ratio, worldRank);
        }
    return false;
}

bool Tree::miniQueueDF (list<TreeNode*> miniQueue, int levelsOfBF, int levelsOfDF ) {

//    int worldRank = omp_get_thread_num();
    list<TreeNode*>::iterator nodeIt = miniQueue.begin();
//    miniQueue.back()->listClusters();
//    miniQueue.back()->listPoints();
    makeNewSequenceReady();

    for( nodeIt ; nodeIt != miniQueue.end() ; nodeIt++ ) {

        depthFirstQueue[worldRank].push_front( (*nodeIt) );

//        if(resPrefix == 101){
//        (*nodeIt)->listClusters();
//        cout << (*nodeIt)->nId << endl;
//        (*nodeIt)->listPoints();
//        }
        if( addPointRecursive( levelsOfBF , levelsOfDF ) ){

            depthFirstQueue[worldRank].clear();
            miniQueue.clear();
            removePartialSequences();
//            cout << "Did not find any proof... at " << worldRank << endl;
            return true; ///Fail
        }
        (*nodeIt)->live = false;
        succesfulNodes.push_back(*nodeIt);
        depthFirstQueue[worldRank].clear();
    }
    return false; ///Success - found a full proof
}

void Tree::destroySubtree(TreeNode* node) {

    if(node->children.empty()){
        return;
    }

    list<TreeNode*>::iterator childIt;

    for( childIt = node->children.begin() ; childIt != node->children.end() ; childIt++ ){
        destroySubtree( (*childIt) );
        destroyNode( (*childIt) );
    }
    node->children.clear();
}

int Tree::addPointDF_experimental(double point, TreeNode *node){
// The following functions do the same as when exploring full tree, exept they use another queue,
// that is used only for the subtree.
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.
//    int worldRank = omp_get_thread_num();
    int nodesAdded = 0;
    //int newChildren = 0;
    switch (node->content.pointInRange(point))
    {
        case 1 :            //one choice: Must be put in existing cluster
        {
            nodesAdded = oneChoiceDF(node, point);
        } break;

        case 2 :            //two choices: New cluster and grow a cluster to the RIGHT
        {
            nodesAdded = twoChoicesRightDF(node, point);
        } break;

        case -2 :           //two choices: New cluster and grow a cluster to the LEFT
        {
            nodesAdded = twoChoicesLeftDF(node, point);
        } break;

        case 3 :            //three choices: In range of a cluster on each side, and get its own
        {
            nodesAdded = threeChoicesDF(node, point);
        } break;


        case 0 :            //no choice: Must open a new cluster for the point
        {
            nodesAdded = noChoiceDF(node, point);
        } break;
    }

    return nodesAdded;
}

int Tree::twoChoicesLeftDF(TreeNode *parent, double point){
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
//    int worldRank = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterLeftDF(parent, point, worldRank);
    nodesAdded += openClusterDF(parent, point, worldRank);

    return nodesAdded;
}

int Tree::twoChoicesRightDF(TreeNode *parent, double point){
    // Tries to add a point by growing a cluster to the left, and trying to break it by force
    // and then tries to open a new cluster and tries to break it by force
    // This is exactly the same as twoChoicesLeft, only with growth to the right insted.
//    int worldRank = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterRightDF(parent, point, worldRank);
    nodesAdded += openClusterDF(parent, point, worldRank);
    return nodesAdded;
}

int Tree::oneChoiceDF(TreeNode *parent, double point) {
    // Add point the an existing cluster, and see if it breaks.
    // For further explaination see twoChoicesLeft()
//    int worldRank = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += addToClusterDF(parent, point, worldRank);

    return nodesAdded;
}

int Tree::noChoiceDF(TreeNode *parent, double point){
// A new cluster MUST be opened for the point.
// Tries to break it by force and returns 1 if it holds up 0 if it breaks
//    int worldRank = omp_get_thread_num();

    int nodesAdded = 0;

    nodesAdded += openClusterDF(parent, point, worldRank);

    return nodesAdded;
}

int Tree::threeChoicesDF(TreeNode *parent, double point){
    // Grows right, left and opens a new cluster.
    // Returns the number of new nodes where succesfully added
//    int worldRank = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterRightDF(parent, point, worldRank);
    nodesAdded += growClusterLeftDF(parent, point, worldRank);
    nodesAdded += openClusterDF(parent, point, worldRank);

    return nodesAdded;
}

int Tree::openClusterDF(TreeNode *parent, double point, int worldRank){

    int nodesAdded = 0;
    //cout << "Must open new cluster..." << endl;
    newNode(worldRank, point, parent);
    nodes[worldRank].front().openCluster( point );

    if(checkPartitioning())
    {
        nodes[worldRank].pop_front();
        newNode(worldRank, point, parent);

        nodes[worldRank].front().openCluster(point);
        parent->addChild(&nodes[worldRank].front());
        depthFirstQueue[worldRank].push_back(&nodes[worldRank].front());
        nodes[worldRank].front().queueLink = depthFirstQueue[worldRank].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[worldRank].front());
    }

    return nodesAdded;

}

int Tree::growClusterRightDF(TreeNode *parent, double point, int worldRank){
    int nodesAdded = 0;

    newNode(worldRank, point, parent);         //Create a new node, with data from the parent and the new point.
    nodes[worldRank].front().growClusterRight(point);

    if(checkPartitioning())                             //Check and try to break. If it holds up then:
    {
        nodes[worldRank].pop_front();                               //remove the node contaminated with extra points from the force
        newNode(worldRank, point, parent);      //create a new "clean" one.
        nodes[worldRank].front().growClusterRight(point);
        parent->addChild(&nodes[worldRank].front());
        depthFirstQueue[worldRank].push_back(&nodes[worldRank].front());       //put it in the queue for points needing attention
        nodes[worldRank].front().queueLink = depthFirstQueue[worldRank].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[worldRank].front());                //add broken point as a child, but do NOT keep it for future point addition
    }

    return nodesAdded;


}

int Tree::growClusterLeftDF(TreeNode *parent, double point, int worldRank){

    int nodesAdded = 0;

    newNode(worldRank, point, parent);         //Create a new node, with data from the parent and the new point.
    nodes[worldRank].front().growClusterLeft(point);

    if(checkPartitioning())                             //Check and try to break. If it holds up then:
    {
        nodes[worldRank].pop_front();                               //remove the node contaminated with extra points from the force
        newNode(worldRank, point, parent);      //create a new "clean" one.
        nodes[worldRank].front().growClusterLeft(point);
        parent->addChild(&nodes[worldRank].front());
        depthFirstQueue[worldRank].push_back(&nodes[worldRank].front());       //put it in the queue for points needing attention
        nodes[worldRank].front().queueLink = depthFirstQueue[worldRank].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[worldRank].front());                //add broken point as a child, but do NOT keep it for future point addition
    }

    return nodesAdded;
}

int Tree::addToClusterDF(TreeNode *parent, double point, int worldRank){

    int nodesAdded = 0;

    newNode(worldRank, point, parent);
    nodes[worldRank].front().addPointToCluster(point );

    if( checkPartitioning() ) {
        nodes[worldRank].pop_front();
        newNode(worldRank, point, parent);
        nodes[worldRank].front().addPointToCluster(point );
        parent->addChild(&nodes[worldRank].front());
        depthFirstQueue[worldRank].push_back(&nodes[worldRank].front());
        nodes[worldRank].front().queueLink = depthFirstQueue[worldRank].end();
        nodesAdded++;
    } else {
        parent->addChild(&nodes[worldRank].front());
    }
    return nodesAdded;
}


/*********************************/
/*** PARALLEL HELPER FUNCTIONS ***/
/*********************************/

void Tree::splitSequenceTree(){

    int sizeOfQueue = sequencedTreeQueue.size();

    parallelMiniQueues = new list<TreeNode*>[sizeOfQueue];

    for(int i = 0; i < sizeOfQueue; i++){
        parallelMiniQueues[i] = sequencedTreeQueue.front();
        sequencedTreeQueue.pop_front();
    }

//    for(int i = 0; i < sizeOfQueue; i++){
//        cout << "Size " << parallelMiniQueues[i].size() << endl;
//    }

    numberOfMiniQueues = sizeOfQueue;
}

list<string> Tree::splitFullMessage(string fullMessage){
    list<string> resultingStrings;
//    std::string delimiter = "x ";
    std::string delimiter = "x";
    size_t pos = 0;
    std::string token;
    while ((pos = fullMessage.find(delimiter)) != std::string::npos) {
        token = fullMessage.substr(0, pos);
//        cout << "Pushing: " << token << endl;
        resultingStrings.push_back(token);
        fullMessage.erase(0, pos + delimiter.length());
    }
//    cout << "Pushing final: " << fullMessage << endl;
    resultingStrings.push_back(fullMessage);
    resultingStrings.pop_front(); //because the first token is empty ;)
    return resultingStrings;
}

list<string> Tree::splitHashString(string inputString){

    list<string> resultingStrings;
//    std::string delimitery = "  y ";
//    std::string delimiterx = "x ";
    std::string delimitery = "y";
    std::string delimiterx = "x";
    size_t pos = 0;
    std::string token;
    while ((pos = inputString.find(delimiterx)) != std::string::npos) {
        token = inputString.substr(0, pos);
        inputString.erase(0, pos + delimiterx.length());
    }


    while ((pos = inputString.find(delimitery)) != std::string::npos) {
        token = inputString.substr(0, pos);

        resultingStrings.push_back(token);
        inputString.erase(0, pos + delimitery.length());
    }


    resultingStrings.push_back(inputString);


    return resultingStrings;

}

list<double> Tree::parseClusters(string pointsString){
    list<double> result;
    string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = pointsString.find(delimiter)) != std::string::npos) {
        token = pointsString.substr(0, pos);
        result.push_back(atof(token.c_str()));
        pointsString.erase(0, pos + delimiter.length());
    }
    result.push_back(atof(pointsString.c_str()));
    return result;

}

list<double> Tree::parsePoints(string pointsString){
    list<double> result;
    string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = pointsString.find(delimiter)) != std::string::npos) {
        token = pointsString.substr(0, pos);
        result.push_back(atof(token.c_str()));
        pointsString.erase(0, pos + delimiter.length());
    }
//    result.push_back(atof(pointsString.c_str()));
    return result;

}

void Tree::newNodeFromLists(list<double> pointList, list<double> clusterList){
    //create new node
    nodes[worldRank].push_front( TreeNode(pointList, clusterList) );
    nodes[worldRank].front().trueLink = nodes[worldRank].begin();

    nodeQueue.push_back(&nodes[worldRank].front());

    //add node to nodeQueue
}

void Tree::buildNodesFromString(string fullMessage){

    //split up message for individual nodeStrings
    if(nodeQueue.size() != 0){
        if(nodeQueue.front()->isRoot()){
            nodeQueue.pop_front();
        }
    }

    list<string> nodeStrings = splitFullMessage(fullMessage);

    list<string>::iterator stringIterator = nodeStrings.begin();

    //create node for each string
    for(stringIterator; stringIterator != nodeStrings.end(); stringIterator++){
        list<string> splittedStrings = splitHashString((*stringIterator));
        string pointString = splittedStrings.front();
        string clusterString = splittedStrings.back();
//        cout << "PointString: " << pointString << endl;
//        cout << "clusterString: " << clusterString << endl;


        list<double> pointList = parsePoints(pointString);
        list<double> clusterList = parseClusters(clusterString);

//        printDoubleList("Cluster points", clusterList);
//        printDoubleList("Point", pointList);

        newNodeFromLists(pointList, clusterList);
    }
}

void Tree::addMiniQueueToMessages(list<TreeNode*> nodes){

    list<TreeNode*>::iterator nodeIterator = nodes.begin();

    for(nodeIterator; nodeIterator != nodes.end(); nodeIterator++){
        addNodeToMessages((*nodeIterator));

    }



}

void Tree::addNodeToMessages(TreeNode *node){
    node->updateHashes();
    messageQueue.back().append(node->getFullHash());

}

void Tree::createMessages(int numberOfMessages){
    int splitSize = (sequencedTreeQueue.size()/numberOfMessages)+1;
    int counter = 0;

    list<list<TreeNode*>>::iterator miniQueueIt;

    for(miniQueueIt = sequencedTreeQueue.begin(); miniQueueIt != sequencedTreeQueue.end(); miniQueueIt++){
        if(counter%splitSize == 0){

            messageQueue.push_back(""); //adds a new message to the queue

        }

        addMiniQueueToMessages(*miniQueueIt);

        ++counter;
    }




//    list<string>::iterator messageIterator;
//
//    for(messageIterator = messageQueue.begin(); messageIterator != messageQueue.end(); messageIterator++){
//        cout << (*messageIterator) << endl;
//        cout <<endl;
//    }


    cout << "Size of the final message queue is: " << messageQueue.size() << endl;

}


/*********************************/
/****** EXPERIMENTAL DF **********/
/*********************************/
bool Tree::addPointRecursive(int level, int maxLevel){ //Returns false if a subproof is found

//    int worldRank = omp_get_thread_num();

    //If reached the maxlevel, just return true (FAIL)
    if(level == maxLevel) {
        return true;
    }

    //Get the next points for the level
    list<double> nextPoints;
    list<TreeNode*>::iterator nodeIt;
    for(nodeIt = depthFirstQueue[worldRank].begin(); nodeIt != depthFirstQueue[worldRank].end(); nodeIt++ ) {

        if((*nodeIt)->depth == level){

            nextPoints = getNextPoints(*nodeIt);
            break;
        }
    }

    //add the points
    int nodesAdded = 0;
    list<double>::iterator pointIt;
    for(pointIt = nextPoints.begin(); pointIt != nextPoints.end(); pointIt++) {

        nodesAdded = addLevel(level, maxLevel, *pointIt);

        if(nodesAdded == 0){
            addSolutionSequence(&nodes[worldRank].front());
            return false;
        }
        else if(!addPointRecursive(level+1, maxLevel)){
            return false;
        }
        clearLevel(level+1, 100); //might need updating... 100 is unneccesary..?
    }
    return true;
}

int Tree::addLevel( int level , int maxLevel , double point ) {

    int nodesAdded = 0;
//    int worldRank = omp_get_thread_num();
    list<TreeNode*>::iterator nodeIt;

    if(level == maxLevel){
        return 1;
    }

    for(nodeIt = depthFirstQueue[worldRank].begin(); nodeIt != depthFirstQueue[worldRank].end(); nodeIt++) {
        if((*nodeIt)->depth == level) {
            nodesAdded += addPointDF_experimental(point, (*nodeIt));
        }
    }

    return nodesAdded;
}

void Tree::clearLevel(int level, double successPoint){
//    int worldRank = omp_get_thread_num();
    list<TreeNode*>::iterator nodeIt = depthFirstQueue[worldRank].begin();

    while(nodeIt != depthFirstQueue[worldRank].end()){

        if((*nodeIt)->depth == level){

            destroySubtree((*nodeIt));
            nodeIt = depthFirstQueue[worldRank].erase(nodeIt);
        } else {

            ++nodeIt;
        }
    }
}

void Tree::sequenceTree(){
//Splits the queue into miniQueues, node in the same miniQueue has the same sequence of points
    list<TreeNode*>::iterator nodeIt = nodeQueue.begin();
    string currentHash = nodeQueue.front()->getPointHash();
    sequencedTreeQueue.push_back(list<TreeNode*>());
    while( nodeIt != nodeQueue.end() ) {
            if(currentHash.compare( (*nodeIt)->getPointHash() ) == 0) {
                sequencedTreeQueue.back().push_back(*nodeIt);
            } else {
                sequencedTreeQueue.push_back(list<TreeNode*>());
                sequencedTreeQueue.back().push_back(*nodeIt);
                currentHash = (*nodeIt)->content.pointHash;
            }
            nodeIt++;
    }

    list<list<TreeNode*>>::iterator it = sequencedTreeQueue.begin();
    sequencedTreeQueue.sort(list_compare);
    nodeQueue.clear();

    for(it = sequencedTreeQueue.begin(); it != sequencedTreeQueue.end(); it++) {
        for(nodeIt = (*it).begin(); nodeIt != (*it).end(); nodeIt++) {
            nodeQueue.push_back(*nodeIt);
        }
    }
}




void Tree::makeNewSequenceReady(){
//    int worldRank = omp_get_thread_num();

    list< list<double> > emptyList;
    proofSequences[worldRank].push_back(emptyList);
}

void Tree::removePartialSequences(){
//    int worldRank = omp_get_thread_num();

    proofSequences[worldRank].pop_back();
}

void Tree::addSolutionSequence(TreeNode *node){
//    int worldRank = omp_get_thread_num();
    proofSequences[worldRank].back().push_back( (node->content.points) ) ;
}

void Tree::normalizeSolutions(){
    list< list< list<double> > >::iterator proofIt;

    for(int i = 0; i < NUM_THREADS; i++){
        for(proofIt = proofSequences[i].begin() ; proofIt != proofSequences[i].end(); proofIt++){
            normalizeProof(&(*proofIt));
        }
    }

}

void Tree::printSolutionSequences(){

    list< list< list<double> > >::iterator proofIt;

    list< list<double> >::iterator listIt;
//    double offset;
    for(int i = 0; i < NUM_THREADS; i++)    {
        for(proofIt = proofSequences[i].begin(); proofIt != proofSequences[i].end(); proofIt++){
            cout << "-----------------" << endl;
            //offset = normalizeProof(&(*proofIt));
            for(listIt = (*proofIt).begin() ; listIt != (*proofIt).end() ; listIt++ ){
                printDoubleList( "Proof : " , *listIt, 0);

            }
        }
    }
}

double Tree::normalizeProof(list < list<double> > *sequence){

    list< list<double> >::iterator listIt = sequence->begin();
    list<double>::iterator pointIt = (*listIt).begin();

//    list<double> normalizedSequence;

    double minPoint = sequence->front().front();
    double offset = 0;
    for(listIt = sequence->begin(); listIt != sequence->end(); listIt++){
        for(pointIt = (*listIt).begin()  ; pointIt != (*listIt).end();  pointIt++){
            if(leq( (*pointIt), minPoint) ){
                minPoint = (*pointIt);
            }
        }
    }

    offset = offset+minPoint;
//    cout << "Offset for proof is : " << offset << endl;
    for(listIt = sequence->begin(); listIt != sequence->end(); listIt++){
        for(pointIt = (*listIt).begin()  ; pointIt != (*listIt).end();  pointIt++){
//            cout << "Subtracting offset " << offset << " from " << (*pointIt) << endl;
            (*pointIt) -= offset ;
//            cout << "After subtraction " << (*pointIt) << endl;
        }
    }
    return offset;
}


bool list_compare(const list<TreeNode*> first,const list<TreeNode*> second) {
    if (first.size() >= second.size()){
        return false;
    }
    if (first.size() < second.size()){
        return true;
    }
    return false;
}

bool printDoubleList(string textToPrint, list<double> listToPrint, double offset){

    list<double>::iterator doubleIt = listToPrint.begin();
    cout << textToPrint << endl;

    for (doubleIt; doubleIt != listToPrint.end(); doubleIt++ ){

        cout << (*doubleIt)-offset << " , ";
    }
    cout << endl;
    return false;
}

void listProofsToFiles(list< list< list<double> > > proofSequences, double ratioIn, int worldRank){
    list< list< list<double> > >::iterator proofIt = proofSequences.begin();
    int proofCount = 0;

    for(proofIt; proofIt != proofSequences.end(); proofIt++){
        if((*proofIt).size() == 1){

            Tree( (*proofIt).front() , proofCount , ratioIn);
            ++proofCount;
        }
    }
}

void listProofSequenceToTextFile( list< list<double> > proofLists, int worldRank){
    FILE *pFile;
    list< list<double> >::iterator listIt;
    list<double>::iterator pointIt;
//    cout << "listing a proof to the file from "<< worldRank << endl;
    char filename[40];
    sprintf( filename , "KnownProofSequences_%i.txt", worldRank);

    pFile = fopen (filename, "a"); //Append to the file
    fprintf(pFile, "\n Proof Sequence: \n");

    for(listIt = proofLists.begin(); listIt != proofLists.end(); listIt++){
        fprintf(pFile, " { ");
        for(pointIt = (*listIt).begin(); pointIt != (*listIt).end(); pointIt++){
            fprintf(pFile, " %2.2f ,", (*pointIt) );
        }
        fprintf(pFile, " }\n ");
    }
    fclose(pFile);
}

void listInitializeTextFile( double ratio, int worldRank){
    FILE *pFile;
    cout << "Initializing sequence file for "<< worldRank << endl;
    char filename[40];
    sprintf( filename , "KnownProofSequences_%i.txt", worldRank);
    pFile = fopen (filename, "w"); //Append to the file

    fprintf(pFile, "\n The following sequences are for ratio: %2.2f ---------------------------\n", ratio);
    fclose(pFile);
}

bool printDoubleList(string textToPrint, list<double> listToPrint) {
    printDoubleList(textToPrint , listToPrint, 0);
    return false;
}
