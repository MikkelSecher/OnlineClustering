#include "Tree.h"

using namespace std;

/*********************************
********** CONSTRUCTORS **********
*********************************/

//REGULAR SEARCH-TREE
Tree::Tree(double dStartingPoint, list<double> deltaPoints, int prefix, double ratioin, int printin){

    int tid     = omp_get_thread_num();
    deltas      = deltaPoints.size();
    resPrefix   = prefix;
    print       = printin;
    ratio       = ratioin;

    for( int i = 0 ; i < deltas ; i++ ){
        delta.push_back( deltaPoints.back() );
        deltaPoints.pop_back();
    }

    nodes[ tid ].push_front( TreeNode(dStartingPoint, NULL) );
    nodeQueue.push_back( &nodes[tid].front() );
    nodeQueue.front()->parentNode = NULL;
    rootNode = &nodes[tid].front();
}

Tree::Tree(list<double> sequence, int prefix, double ratioIn){
        int tid     = omp_get_thread_num();

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
    for (pointIt; pointIt != sequence.end() ; pointIt++ )
    {
        level++;
//        cout << "point is: " << (*pointIt) << endl;
        list<TreeNode*>::iterator nodeIt;

        for(nodeIt = nodeQueue.begin() ; nodeIt != nodeQueue.end() ; nodeIt++ )
        {
//            cout << "Node level " << (*nodeIt)->depth << endl;
            if( (*nodeIt)->depth == level-1 ) {
                addPoint( 0, (*pointIt) );
                nodeIt = nodeQueue.erase( nodeIt );
                nodeIt--;
            }
        }
    }
    FILE *pFile;
    char filename[30];
    sprintf( filename , "res\\%d_%d_solution_%d .gml" , resPrefix , tid , 10000 );

    pFile = fopen (filename,"w"); //cleans the previous file
    fprintf(pFile, "graph \n [ \n");
//    fprintf(pFile, "node \n [ \n id n%s \n ] \n ", sNodeID.c_str());
    fclose(pFile);

    printFullTreeToFile(rootNode);

    pFile = fopen (filename,"a");
    fprintf(pFile, "]");
    fclose(pFile);


    list<TreeNode>::iterator nodeIt = nodes[0].begin();

//    for (nodeIt; nodeIt != nodes[0].end() ; nodeIt++)
//    {
//        (*nodeIt).listClusters();
//        (*nodeIt).listPoints();
//    }
}

//void Tree::dictatePointBF(double point, int level){
//
//        list<TreeNode*>::iterator nodeIt;
//
//        for(nodeIt = nodeQueue.begin() ; nodeIt != nodeQueue.end() ; nodeIt++ )
//        {
////            cout << "Node level " << (*nodeIt)->depth << endl;
//            if( (*nodeIt)->depth == level-1 ) {
//                addPoint( 0, (*pointIt) );
//                nodeIt = nodeQueue.erase( nodeIt );
//                nodeIt--;
//            }
//        }
//
//
//}

void Tree::printFullTreeToFile(TreeNode *node){

    list<TreeNode*>::iterator childIt = node->children.begin();

    node->content.insertNodeLabel(false, node->nId, 10000, resPrefix);

    if(!node->rootNode){
        node->content.insertEdgeLabel(node->parentNode->nId, node->nId, 10000, resPrefix);

    }
    if(node->children.size() > 0){
        for (childIt ; childIt != node->children.end() ; childIt++ ){
            printFullTreeToFile( (*childIt) );
        }
    }
    return;
}

//Creates a new node
TreeNode* Tree::newNode( int tid, double point, TreeNode* parent ){
    nodes[tid].push_front( TreeNode(point, parent) );
    nodes[tid].front().trueLink = nodes[tid].begin();
    return &nodes[tid].front();
}

void Tree::destroyNode(TreeNode *node){
    int tid = omp_get_thread_num();
    nodes[tid].erase(node->trueLink);
}

/*********************************
***** HANDLERS FOR NEW POINTS ****
*********************************/
//ADDS ALL POSSIBLE POINTS, BASED ON DELTAS
void Tree::addLevelBF(int level)
{
// For an entire level in the tree, this calculates all the points to be added to each
// node, based on the delta values in tree.h.
// If a point already exists in a node, it is skipped.

    if(nodeQueue.size() == 0){
        cout << "No queue to work from! " << endl;
        exit(0);
    }

    if( nodeQueue.size() > 0 ) {
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

//CHECKS RATIO, AND IF A PARTITIONING CAN BE FORCED
bool Tree::checkPartitioning(){
// Checks ratio for a partitioning
// Returns FALSE if the ratio is above the threshold
// Also tries to break the partitioning by forcing new points
    int tid = omp_get_thread_num();

    TreeNode *workingNode = &nodes[tid].front();
    Partitioning *workingPartitioning = &workingNode->content;

    if( geq( workingPartitioning->calcRatio(), ratio ) )
    {
        return false;
    }
    if( nodes[tid].front().depth == 0 ){
        return true;
    }

    workingPartitioning->setAmbSpace();
    list<double> ambPoints = workingPartitioning->getAmbPoints();

    if( !force_experimental(workingNode , workingNode , workingPartitioning->getForcePoints( ), ambPoints) ){

        return false;
    }
    //return nodes[tid].front().content.force(ratio);
    return true;
}

bool Tree::force_experimental(TreeNode *node, TreeNode *originalNode, list<double> forcePoints, list<double> ambPoints)
{ ///TODO: Cleanup
    int tid = omp_get_thread_num();

    list<double> emptyList;
    //node->content.forced = true;
    TreeNode* forceNode;

    ///If there are no forcepoints
    if( forcePoints.size() == 0 ){
        while( ambPoints.size() != 0 ){///As long as there is still ambiguous points
            if( !node->doesPointExist( ambPoints.front() ) ){///Check that the point doesn't already exist (safety)
                ambPoints.pop_front();
                continue;
            }
            ///Get a new node to work on
            forceNode = newNode( tid, ambPoints.front(), node );
            forceNode->openCluster( ambPoints.front(), true );

            ///If opening the new cluster breaks the ratio, it means that
            ///there is only one choice -> expand the current cluster
            if(geq(forceNode->calcRatio(), ratio)){
                ///Erase the node that broke the ratio
                destroyNode(forceNode);
                ///And make a new one
                forceNode = newNode( tid, ambPoints.front(), node );

                int pointInRange = forceNode->pointInRange( ambPoints.front() );

                if(pointInRange == 2 || pointInRange == -2) {

                    if(pointInRange == 2){ ///Grow the cluster
                        forceNode->growClusterRight( ambPoints.front(), true );
                    } else {
                        forceNode->growClusterLeft( ambPoints.front(), true );
                    }

                    forcePoints = forceNode->getForcePoints(); ///Get new forcePoints
                    emptyList = ambPoints; ///Get ready to call the force with an empty ambPoints list
                    emptyList.remove( ambPoints.front() );

                    if( !force_experimental( forceNode, originalNode, forcePoints, emptyList ) ) {///Call it
                        //If it is now forcable, return false (Success)
                        node->content.forced = true;
                        node->live = false;

                        if( pointInRange == 2 ){ ///Update the original node with the expanded cluster
                            originalNode->growClusterRight( ambPoints.front(), true );
                        } else {
                            originalNode->growClusterLeft( ambPoints.front(), true );
                        }

                        destroyNode( forceNode );  ///Erase the forceNode from memory
                        node->children.clear(); ///Clear the nodes children

                        return false;
                    }
                }
            }

            destroyNode( forceNode ); ///If this doesn't work, erase the node worked on
            ambPoints.pop_front(); ///And try with the next ambiguous point
        }
        return true;///When there are no more points return true (FAIL)
    }

    forcePoints = node->getForcePoints(); ///Get the current forcepoints for the node

    list<double>::iterator forcePointIt; ///For each of the forcePoints
    for( forcePointIt = forcePoints.begin() ; forcePointIt != forcePoints.end() ; forcePointIt++ ) {

        if( !node->doesPointExist( (*forcePointIt) ) ) { ///Check that the point doesn't already exist (safety)

             continue;
           }
        forceNode = newNode( tid, *forcePointIt, node ); ///Create a new node to force with
        forceNode->openCluster( *forcePointIt ); ///And open a forced cluster


        if(geq(forceNode->calcRatio(), ratio) || ///If the ratio is broken by this
        !force_experimental(forceNode, originalNode, forcePoints, ambPoints ) || ///Else call again with the forcepoint list
        !force_experimental( forceNode, originalNode, emptyList, ambPoints ) ){ ///Else try with ambiguous points
            ///If successful
            node->content.forced = true;
            node->live = false;

            originalNode->openCluster( *forcePointIt , true );

            destroyNode( forceNode );
            node->children.clear();

            return false;
        } else { ///Else clean up and return true (FAIL)
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

/*********************************
****** GENERAL FUNCTIONS *********
*********************************/

void Tree::analyzeTree( int levelsOfBF ) //FINDS PROOFS AFTER DF PART IS DONE
{
//Start the analysis of the complete tree.
//Running through the list of succesfull nodes with subproofs
//Will count the number of subproofs resulting in proofs
//    cout << endl;
//    cout << "Analyzing search tree" << endl;
//    cout << "---------------------------------------------" << endl;
//    cout << endl;

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
//        cout << "Split the nodes" << endl;
        #pragma omp for
        for(int t = 0; t < numberOfThreads; t++)
        {
            for(i = parSuccesfulNodes[t].begin(); i != parSuccesfulNodes[t].end(); i++)
            {
                if((*i)->nId = 1 and backtrackSolution((*i), (*i)->content.points, levelsOfBF, succes))
                {
                    cout << "from root PROOF FOUND in thread "<< t << endl;
                    succes++;
                }


                //amount++;
                if((*i)->nId != 1 and backtrackSolution((*i)->parentNode, (*i)->content.points, levelsOfBF, succes))
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

bool node_compare( const TreeNode *first,const TreeNode *second ) ///TODO: Test
{
    if (first->content.fullHash.compare(second->content.fullHash) >= 0)
    {
        return false;
    }
    if(first->content.fullHash.compare(second->content.fullHash) < 0)
        return true;
}

bool node_compare_sequence( const TreeNode *first,const TreeNode *second ) ///TODO: Test
{
    if (first->content.fullHash.compare(second->content.pointHash) >= 0){
        return false;
    }

    if(first->content.fullHash.compare(second->content.pointHash) < 0){
        return true;
    }
}

///TODO: Test
bool node_pointscompare(const TreeNode *first,const TreeNode *second){

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

//REMOVE DUPES OF NODES AFTER NORMALIZATION ///TODO: Clean up
void Tree::treeCleanup() {
    // Normalizes the points in each node by shifting everything to start out at 0
    // After all nodes have been normalized, they are compared and duplicates are removed
    cout << "before cleanup: " << nodeQueue.size() << endl;

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
    cout << "after cleanup: " << nodeQueue.size() << endl;
}

void Tree::normalize(TreeNode *node) // SHIFT ALL PARTITIONINGS TO START AT 0
{
// Shifts points, sortedPoints and the clusters, to start from 0
    if ( leq( node->content.points.back() , 0) )
    {
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



/*********************************
**** BREADTH-FIRST FUNCTIONS *****
*********************************/

void Tree::addPoint(int p, double point) //ADDS A GIVEN POINT BF
{
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

    int tid = omp_get_thread_num();

    growClusterLeftBF(parent, point, tid);
    openClusterBF(parent, point, tid);
}

void Tree::twoChoicesRight(TreeNode *parent, double point){
    // Tries to add a point by growing a cluster to the left, and trying to break it by force
    // and then tries to open a new cluster and tries to break it by force
    // This is exactly the same as twoChoicesLeft, only with growth to the right insted.
    int tid = omp_get_thread_num();

    growClusterRightBF(parent, point, tid);
    openClusterBF(parent, point, tid);
}

void Tree::oneChoice(TreeNode *parent, double point){
    // Add point the an existing cluster, and see if it breaks.
    // For further explaination see twoChoicesLeft()

    int tid = omp_get_thread_num();

    addToClusterBF(parent, point, tid);
}

void Tree::noChoice(TreeNode *parent, double point){
    // A new cluster MUST be opened for the point.
    // Tries to break it by force and returns 1 if it holds up 0 if it breaks
    int tid = omp_get_thread_num();

    openClusterBF(parent, point, tid);
}

void Tree::threeChoices(TreeNode *parent, double point){
    // Grows right, left and opens a new cluster.
    // Returns the number of new nodes where succesfully added
    int tid = omp_get_thread_num();

    growClusterRightBF(parent, point, tid);
    growClusterLeftBF(parent, point, tid);
    openClusterBF(parent, point, tid);

}


int Tree::openClusterBF(TreeNode *parent, double point, int tid){


    newNode( tid, point, parent );
    nodes[tid].front().openCluster( point);

    if(checkPartitioning()){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode( tid, point, parent );
        nodes[tid].front().openCluster( point);
        //Add it as a child to the parent node
        parent->addChild( &nodes[tid].front() );
        //Set live to false - needed for later...
        nodeQueue.push_back( &nodes[tid].front() );
        nodes[tid].front().queueLink = nodeQueue.end();
    }else{
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

    normalize(&nodes[tid].front());
    nCount++;
}

int Tree::growClusterRightBF(TreeNode *parent, double point, int tid){
    newNode(tid, point, parent);
    nodes[tid].front().growClusterRight( point);

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, point, parent);
        nodes[tid].front().growClusterRight(point);

        //Add it as a child to the parent node
        parent->addChild( &nodes[tid].front() );

        //Add it the the node queue
        nodeQueue.push_back( &nodes[tid].front() );
        nodes[tid].front().queueLink = nodeQueue.end();
    } else {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild(&nodes[tid].front());
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

    normalize(&nodes[tid].front());
}

int Tree::growClusterLeftBF(TreeNode *parent, double point, int tid){
    newNode( tid, point, parent );
    nodes[tid].front().growClusterLeft( point );

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, point, parent);
        nodes[tid].front().growClusterLeft( point );

        //Add it as a child to the parent node
        parent->addChild( &nodes[tid].front() );

        //Add it the the node queue
        nodeQueue.push_back( &nodes[tid].front() );
        nodes[tid].front().queueLink = nodeQueue.end();
    } else {
        //If it COULD be forced:
        //Add the node as child to the parent
        parent->addChild( &nodes[tid].front() );
        //Set live to false - needed for later...
        nodes[tid].front().live = false;
    }

    normalize(&nodes[tid].front());


}

int Tree::addToClusterBF(TreeNode *parent, double point, int tid){

    newNode(tid, point, parent);
    nodes[tid].front().addPointToCluster( point );

    if( checkPartitioning() ){
        //If the partitioning CAN'T be forced, do the following:
        //Erase the new node, and add a new one without the forced points
        nodes[tid].pop_front();
        newNode(tid, point, parent);
        nodes[tid].front().addPointToCluster(point);
        //Add it as a child to the parent node
        parent->addChild(&nodes[tid].front());
        //Add it the the node queue
        nodeQueue.push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = nodeQueue.end();
    } else {
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

bool Tree::startDF_experimental(int levelsOfBF, int levelsOfDF) {
    int tid = omp_get_thread_num();

    if( nodeQueue.size() > 1 ){
        nodeQueue.sort( node_compare_sequence ); //Sort queue wrt. sequence of points
    }
    sequenceTree(); //sort queue into miniqueues sorted by sequence of points

    list<TreeNode*>::iterator nodeIt = sequencedTreeQueue.front().begin();
    list<TreeNode>::iterator temp = nodes[tid].begin();

    string currentHash = nodeQueue.front()->getPointHash();

    int proofCount = 0;
    int tryCount = 0;
    list< list<TreeNode*> >::iterator listIt = sequencedTreeQueue.begin();
    int proofsToTry = sequencedTreeQueue.size() ;
    cout << "Possibilities for proofs: " << proofsToTry << endl;
    for( listIt ; listIt != sequencedTreeQueue.end() ; listIt++ ){
        ++tryCount;
        if(tryCount % 100 == 0){
            cout << tryCount << "/" << proofsToTry << endl;
        }
        if(!miniQueueDF( (*listIt) , levelsOfBF , levelsOfDF ) ){
            proofCount++;
            listProofSequenceToTextFile( proofSequences.back());
//            cout << "proofed -----------------------------------------"  << endl;
        }
    }

    if(proofCount > 0){
        normalizeSolutions();
        printSolutionSequences();

        listProofsToFiles(proofSequences, ratio);

        return false;
    }
    return true;

    analyzeTree(levelsOfBF);
    exit(1);
}

bool Tree::miniQueueDF (list<TreeNode*> miniQueue, int levelsOfBF, int levelsOfDF ) {

    int tid = omp_get_thread_num();
    list<TreeNode*>::iterator nodeIt = miniQueue.begin();

    makeNewSequenceReady();

//    cout << "Size of miniQueue " << miniQueue.size() << endl;

    for( nodeIt ; nodeIt != miniQueue.end() ; nodeIt++ ) {


        depthFirstQueue[tid].push_front( (*nodeIt) );

        if( addPointRecursive( levelsOfBF , levelsOfDF ) ){

            depthFirstQueue[tid].clear();
            miniQueue.clear();
            removePartialSequences();
//            cout << "Failed to proof sub" << endl;
            return true; ///Fail
        }
//        cout << "subproof found" << endl;
        (*nodeIt)->live = false;
        succesfulNodes.push_back(*nodeIt);
        depthFirstQueue[tid].clear();
    }

//    cout << "Found full proof "<< endl;
    return false; ///Success
}


void Tree::destroySubtree(TreeNode* node) {

    if(node->children.size() == 0){
        return;
    }

    list<TreeNode*>::iterator childIt;
    int tid = omp_get_thread_num();

    for( childIt = node->children.begin() ; childIt != node->children.end() ; childIt++ )
    {
        destroySubtree( (*childIt) );
        destroyNode( (*childIt) );
    }
    node->children.clear();
}

//ADDS A GIVEN POINT TO NODE IN DF PART
int Tree::addPointDF_experimental(double point, TreeNode *node){
// The following functions do the same as when exploring full tree, exept they use another queue,
// that is used only for the subtree.
// Adds a point to the current node. It checks to see what possibilities there are for
// the particular point.
// It also tries to break the partitioning by forcing.
// If the force fails, the points added by the forcer are erased, by throwing out the node.
    int tid = omp_get_thread_num();
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

bool Tree::backtrackSolution(TreeNode* node, list<double> points, int levelsOfBF, int succesNumber) {
    //Backtracks through to the root of the tree, and explores to see if
    //all leafs end in ratios higher than threshold
//    node->content.listClusters();
//    cout << "Nodes ID: "<< node->nId << endl;
    int tid = omp_get_thread_num();
    FILE * pFile;
    string sNodeID = node->content.stringIt(node->nId);
    if(node->nId != 1 and node->parentNode != NULL )
    {
        cout << "nID: " << node->nId << endl;
        if(!backtrackSolution(node->parentNode, points, levelsOfBF, succesNumber))
            return false;
        else
            return true;
    }

    char filename[30];
    sprintf( filename , "res\\%d_%d_solution_%d .gml" , resPrefix , tid , succesNumber );

    if(node->depth == 0 or node->rootNode)
    {
//        cout << "node " << node->nId << " number of children " <<  node->children.size() << endl;
//        node->children.front()->content.listClusters();
//        node->children.back()->content.listClusters();
        if(print >= 1)
        {

            pFile = fopen (filename,"w"); //cleans the previous file
            fprintf(pFile, "graph \n [ \n");
            fprintf(pFile, "node \n [ \n id n%s \n ] \n ", sNodeID.c_str());
            fclose(pFile);
        }

        if(!forwardCheck(node, points, levelsOfBF, succesNumber))
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

bool Tree::forwardCheck(TreeNode* node, list<double> points, int levelsOfBF, int succesNumber) {
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

    for(i = node->children.begin(); i != node->children.end(); i++){
        if(deq((*i)->content.difference, difference) and (*i)->depth < levelsOfBF){

            if(print >= 1){
                (*i)->content.insertNodeLabel(false, (*i)->nId, succesNumber, resPrefix);
            }

            livingChildren++;
            if(!forwardCheck((*i), points, levelsOfBF, succesNumber)){
                return false;
            }
            if(print >= 1){
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);

            }
        }

        if((*i)->depth == levelsOfBF and deq((*i)->content.difference, difference)){
            if((*i)->live == true){
                return false;
            }
            (*i)->live = true;
            if(print >= 1){
                (*i)->content.insertNodeLabel(true, (*i)->nId, succesNumber, resPrefix);
            }

            list<double> newPoints;
            for(int j = 0; j < levelsOfBF; j++)
                newPoints.push_back(node->workingSequence[j]);

            if(!forwardCheck((*i), newPoints, levelsOfBF, succesNumber)){
                return false;
            }
            if(print >= 1){
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);
            }
        }

        if((*i)->depth > levelsOfBF){
            if(print >= 1 and (geq((*i)->content.calcRatio(), ratio) or (*i)->children.size() > 0)){
                (*i)->content.insertNodeLabel(false, (*i)->nId, succesNumber, resPrefix);
                (*i)->content.insertEdgeLabel((*i)->parentNode->nId, (*i)->nId, succesNumber, resPrefix);
            }
            forwardCheck((*i), points, levelsOfBF, succesNumber);
        }
    }
    return true;
}

int Tree::twoChoicesLeftDF(TreeNode *parent, double point){
// Tries to add a point by growing a cluster to the left, and trying to break it by force
// and then tries to open a new cluster and tries to break it by force
    int tid = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterLeftDF(parent, point, tid);
    nodesAdded += openClusterDF(parent, point, tid);

    return nodesAdded;
}

int Tree::twoChoicesRightDF(TreeNode *parent, double point){
    // Tries to add a point by growing a cluster to the left, and trying to break it by force
    // and then tries to open a new cluster and tries to break it by force
    // This is exactly the same as twoChoicesLeft, only with growth to the right insted.
    int tid = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterRightDF(parent, point, tid);
    nodesAdded += openClusterDF(parent, point, tid);
    return nodesAdded;
}

int Tree::oneChoiceDF(TreeNode *parent, double point) {
    // Add point the an existing cluster, and see if it breaks.
    // For further explaination see twoChoicesLeft()
    int tid = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += addToClusterDF(parent, point, tid);

    return nodesAdded;
}

int Tree::noChoiceDF(TreeNode *parent, double point){
// A new cluster MUST be opened for the point.
// Tries to break it by force and returns 1 if it holds up 0 if it breaks
    int tid = omp_get_thread_num();

    int nodesAdded = 0;

    nodesAdded += openClusterDF(parent, point, tid);

    return nodesAdded;
}

int Tree::threeChoicesDF(TreeNode *parent, double point){
    // Grows right, left and opens a new cluster.
    // Returns the number of new nodes where succesfully added
    int tid = omp_get_thread_num();
    int nodesAdded = 0;

    nodesAdded += growClusterRightDF(parent, point, tid);
    nodesAdded += growClusterLeftDF(parent, point, tid);
    nodesAdded += openClusterDF(parent, point, tid);

    return nodesAdded;
}



int Tree::openClusterDF(TreeNode *parent, double point, int tid){

    int nodesAdded = 0;
    //cout << "Must open new cluster..." << endl;
    newNode(tid, point, parent);
    nodes[tid].front().openCluster( point );

    if(checkPartitioning())
    {
        nodes[tid].pop_front();
        newNode(tid, point, parent);

        nodes[tid].front().openCluster(point);
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

int Tree::growClusterRightDF(TreeNode *parent, double point, int tid){
    int nodesAdded = 0;

    newNode(tid, point, parent);         //Create a new node, with data from the parent and the new point.
    nodes[tid].front().growClusterRight(point);

    if(checkPartitioning())                             //Check and try to break. If it holds up then:
    {
        nodes[tid].pop_front();                               //remove the node contaminated with extra points from the force
        newNode(tid, point, parent);      //create a new "clean" one.
        nodes[tid].front().growClusterRight(point);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());       //put it in the queue for points needing attention
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());                //add broken point as a child, but do NOT keep it for future point addition
    }

    return nodesAdded;


}

int Tree::growClusterLeftDF(TreeNode *parent, double point, int tid){

    int nodesAdded = 0;

    newNode(tid, point, parent);         //Create a new node, with data from the parent and the new point.
    nodes[tid].front().growClusterLeft(point);

    if(checkPartitioning())                             //Check and try to break. If it holds up then:
    {
        nodes[tid].pop_front();                               //remove the node contaminated with extra points from the force
        newNode(tid, point, parent);      //create a new "clean" one.
        nodes[tid].front().growClusterLeft(point);
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());       //put it in the queue for points needing attention
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    }
    else
    {
        parent->addChild(&nodes[tid].front());                //add broken point as a child, but do NOT keep it for future point addition
    }

    return nodesAdded;
}

int Tree::addToClusterDF(TreeNode *parent, double point, int tid){

    int nodesAdded = 0;

    newNode(tid, point, parent);
    nodes[tid].front().addPointToCluster(point );

    if( checkPartitioning() ) {
        nodes[tid].pop_front();
        newNode(tid, point, parent);
        nodes[tid].front().addPointToCluster(point );
        parent->addChild(&nodes[tid].front());
        depthFirstQueue[tid].push_back(&nodes[tid].front());
        nodes[tid].front().queueLink = depthFirstQueue[tid].end();
        nodesAdded++;
    } else {
        parent->addChild(&nodes[tid].front());
    }
    return nodesAdded;
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

//    cout << "SuccesQueue of " << succesfulNodes.size() << " is split into " << numberOfThreads << " of " << splits << endl;


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

//    for(int i = 0; i < numberOfThreads; i++)
//    {
//        cout << "Size of the parallel succesfulNodesQueue " << i << " " << parSuccesfulNodes[i].size() << endl;
//    }

    return 0;

}



/*********************************
****** EXPERIMENTAL DF ***********
*********************************/
bool Tree::addPointRecursive(int level, int maxLevel){ //Returns false if a subproof is found

    int tid = omp_get_thread_num();

    //If reached the maxlevel, just return true (FAIL)
    if(level == maxLevel) {
        return true;
    }

    //Get the next points for the level
    list<double> nextPoints;
    list<TreeNode*>::iterator nodeIt;
    for(nodeIt = depthFirstQueue[tid].begin(); nodeIt != depthFirstQueue[tid].end(); nodeIt++ ) {

        if((*nodeIt)->depth == level){

            nextPoints = getNextPoints(*nodeIt);
            break;
        }
    }

    //add the points
    int nodesAdded = 0;
    list<double>::iterator pointIt;
    for(pointIt = nextPoints.begin(); pointIt != nextPoints.end(); pointIt++) {
        nodesAdded = 0;
        nodesAdded = addLevel(level, maxLevel, *pointIt);

        if(nodesAdded == 0){
//            printDoubleList("Sorted Points", nodes[0].front().content.points);

            addSolutionSequence(&nodes[0].front());

            return false;
        }
        else if(!addPointRecursive(level+1, maxLevel)){

            return false;
        }
        clearLevel(level+1, 100);
    }
    return true;
}

int Tree::addLevel( int level , int maxLevel , double point ) {

    int nodesAdded = 0;
    int tid = omp_get_thread_num();
    list<TreeNode*>::iterator nodeIt;

    for(nodeIt = depthFirstQueue[tid].begin(); nodeIt != depthFirstQueue[tid].end(); nodeIt++) {
        if((*nodeIt)->depth == level) {
            nodesAdded += addPointDF_experimental(point, (*nodeIt));
        }
    }

    return nodesAdded;
}

void Tree::clearLevel(int level, double successPoint){

    int tid = omp_get_thread_num();

    list<TreeNode*>::iterator nodeIt = depthFirstQueue[tid].begin();

    int start = depthFirstQueue[tid].size();

    while(nodeIt != depthFirstQueue[tid].end()){

        if((*nodeIt)->depth == level){

            destroySubtree((*nodeIt));
            nodeIt = depthFirstQueue[tid].erase(nodeIt);
        } else {

            ++nodeIt;
        }
    }
}

void Tree::sequenceTree(){

    list<TreeNode*>::iterator nodeIt = nodeQueue.begin();
    string currentHash = nodeQueue.front()->getPointHash();
    sequencedTreeQueue.push_back(list<TreeNode*>());
    while( nodeIt != nodeQueue.end() ) {
            if(currentHash.compare( (*nodeIt)->getPointHash() ) == 0) {
                sequencedTreeQueue.back().push_back(*nodeIt);
            }
            else{

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

    list< list<double> > emptyList;
    proofSequences.push_back(emptyList);
}

void Tree::removePartialSequences(){

    proofSequences.pop_back();
}


void Tree::addSolutionSequence(TreeNode *node){



    proofSequences.back().push_back( (node->content.points) ) ;

}

void Tree::normalizeSolutions(){

    list< list< list<double> > >::iterator proofIt;

    for(proofIt = proofSequences.begin() ; proofIt != proofSequences.end(); proofIt++){

        normalizeProof(&(*proofIt));

    }


}

void Tree::printSolutionSequences(){

    list< list< list<double> > >::iterator proofIt = proofSequences.begin();

    list< list<double> >::iterator listIt;
    double offset;
    for(proofIt; proofIt != proofSequences.end(); proofIt++){
        cout << "-----------------" << endl;
        //offset = normalizeProof(&(*proofIt));
        for(listIt = (*proofIt).begin() ; listIt != (*proofIt).end() ; listIt++ ){
            printDoubleList( "Proof : " , *listIt, 0);

        }
    }
}

double Tree::normalizeProof(list < list<double> > *sequence){

    list< list<double> >::iterator listIt = sequence->begin();
    list<double>::iterator pointIt = (*listIt).begin();

    list<double> normalizedSequence;

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

    if (first.size() >= second.size())
    {
        return false;
    }
    if (first.size() < second.size())
        return true;
}

bool printDoubleList(string textToPrint, list<double> listToPrint, double offset){

    list<double>::iterator doubleIt = listToPrint.begin();
    cout << textToPrint << endl;



    for (doubleIt; doubleIt != listToPrint.end(); doubleIt++ ){

        cout << (*doubleIt)-offset << " , ";
    }
    cout << endl;

}

void listProofsToFiles(list< list< list<double> > > proofSequences, double ratioIn){

    list< list< list<double> > >::iterator proofIt = proofSequences.begin();

    list< list<double> >::iterator listIt;

    int proofCount = 0;

    for(proofIt; proofIt != proofSequences.end(); proofIt++){

        if((*proofIt).size() == 1){

            Tree( (*proofIt).front() , proofCount , ratioIn);
            ++proofCount;
        }
    }
}

void listProofSequenceToTextFile( list< list<double> > proofLists){
    FILE *pFile;
    list< list<double> >::iterator listIt;
    list<double>::iterator pointIt;
    cout << "listing a proof to the file" << endl;


    pFile = fopen ("KnownProofSequences.txt","a"); //Append to the file

    fprintf(pFile, "\n Proof Sequence: \n");

    for(listIt = proofLists.begin(); listIt != proofLists.end(); listIt++){
        fprintf(pFile, " { ");
        for(pointIt = (*listIt).begin(); pointIt != (*listIt).end(); pointIt++){
            fprintf(pFile, " %2.2f ", (*pointIt) );
        }
        fprintf(pFile, " }\n ");


    }




//    fprintf(pFile, "node \n [ \n id n%s \n ] \n ", sNodeID.c_str());
    fclose(pFile);

}



bool printDoubleList(string textToPrint, list<double> listToPrint) {
    printDoubleList(textToPrint , listToPrint, 0);
}


