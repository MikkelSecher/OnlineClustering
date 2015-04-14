#include "OCTester.h"

OCTester::OCTester()
{
    int successCount = 0;
    int failCount = 0;

    //startingSequence();
    failCount += completeForceTest();
    failCount += completeChildrenTest();

    startingSequence();

    if(failCount== 0){
        cout << endl;
        cout << "*********************************" << endl;
        cout << "***** ALL TESTS SUCCESFULL ******" << endl;
        cout << "*********************************" << endl;
    }else{
        cout << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "!!!!! " << failCount << " TESTS FAILED !!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }
}
int OCTester::completeChildrenTest(){
    int successCount = 0;
    int failCount = 0;
    cout << endl;
    cout << "*********************************" << endl;
    cout << "****** DOING CHILD TESTS ********" << endl;
    cout << "*********************************" << endl;
    cout << endl;

    failCount += hardTwoChoicesRightTest();
    return failCount;
}

int OCTester::completeForceTest(){

    int successCount = 0;
    int failCount = 0;

    cout << "*********************************" << endl;
    cout << "****** DOING FORCE TESTS ********" << endl;
    cout << "*********************************" << endl;
    cout << endl;
    if(easyForceTest()){
        cout << "'Easy Force'               -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'Easy Force'               -- FAILED" << endl;
        failCount++;
    }

    if(startForceTest()){
        cout << "'Start Force'              -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'Start Force'              -- FAILED" << endl;
        failCount++;
    }

    if(endForceTest()){
        cout << "'End Force'                -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'End Force'                -- FAILED" << endl;
        failCount++;
    }

    if(hardForceTest()){
        cout << "'Hard Force'               -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'Hard Force'               -- FAILED"<< endl;
        failCount++;
    }

    if(harderForceTest()){
        cout << "'Harder Force Test'        -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'Harder Force'             -- FAILED "<< endl;
        failCount++;
    }
    return failCount;
}

bool OCTester::startingSequence(){
//    list<double> deltas = {1, -0.5, -1};
//    Tree tree(0, deltas, 7357, 1.61, 2);

    //Set clusters
    Tree tree = getStarterTestTree(1.63);

    cout << "Print setting is: " << tree.print << endl;

    tree.nodes[0].front().depth = 11;

    cout << "Number of points, i.e. level: " << tree.nodes[0].front().content.nNumberOfPoints << endl;

    tree.startDF_experimental(11, 19);

    exit(1);

    return true;
}

bool OCTester::easyForceTest(){
    Tree tree = getEasyForcableTree(1.60);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

//    cout << "Number of ambiguous points: " << ambPoints.size() << endl;
    list<double> forcePoints = nodeToUse->content.getForcePoints();

//    cout << "Number of forcePoints: " << forcePoints.size() << endl;
//    cout << "Nodes in sorted list " << nodeToUse->content.sortedPoints.size() << endl;

    if(!tree.force_experimental(nodeToUse, nodeToUse, forcePoints, ambPoints))
    {
        return true;
    }else{
        return false;
    }
    return false;

}

bool OCTester::endForceTest(){
    Tree tree = getEndForcableTree(1.60);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

//    cout << "Number of ambiguous points: " << ambPoints.size() << endl;
    list<double> forcePoints = nodeToUse->content.getForcePoints();

//    cout << "Number of forcePoints: " << forcePoints.size() << endl;
//    cout << "Nodes in sorted list " << nodeToUse->content.sortedPoints.size() << endl;

    if(!tree.force_experimental(nodeToUse, nodeToUse, forcePoints, ambPoints))
    {
        return true;
    }else{
        return false;
    }
    return false;
}

bool OCTester::startForceTest(){
    Tree tree = getStartForcableTree(1.60);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

//    cout << "Number of ambiguous points: " << ambPoints.size() << endl;
    list<double> forcePoints = nodeToUse->content.getForcePoints();

//    cout << "Number of forcePoints: " << forcePoints.size() << endl;
//    cout << "Nodes in sorted list " << nodeToUse->content.sortedPoints.size() << endl;

    if(!tree.force_experimental(nodeToUse, nodeToUse, forcePoints, ambPoints))
    {
        return true;
    }else{
        return false;
    }
    return false;
}

bool OCTester::hardForceTest(){
    ///Get ready
    Tree tree = getHardForcableTree(1.61);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

    list<double> forcePoints = nodeToUse->content.getForcePoints();

    if(!tree.force_experimental(nodeToUse, nodeToUse, forcePoints, ambPoints))
    {
        return true;
    }else{
        return false;
    }
    return false;
}

bool OCTester::harderForceTest(){
    ///Get ready
    Tree tree = getHarderForcableTree(1.61);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

//    cout << "Number of ambiguous points: " << ambPoints.size() << endl;
    list<double> forcePoints = nodeToUse->content.getForcePoints();

//    cout << "Number of forcePoints: " << forcePoints.size() << endl;
//    cout << "Nodes in sorted list " << nodeToUse->content.sortedPoints.size() << endl;

    if(!tree.force_experimental(nodeToUse, nodeToUse, forcePoints, ambPoints))
    {
        return true;
    }else{
        return false;
    }
    return false;
}

int OCTester::hardTwoChoicesRightTest(){
    int successCount = 0;
    int failCount = 0;

    Tree tree = getHardTwoChoicesRightTree(1.61);
    TreeNode* nodeToUse = &tree.nodes[0].front();
    tree.addPointDF_experimental(10, nodeToUse);

    if(nodeToUse->children.size() == 1){
        cout << "'Hard Two Choices Right'   -- SUCCESS" << endl;
        successCount++;
    }else{
        cout << "'Hard Two Choices Right'   -- FAILED" << endl;
        failCount++;
    }
    return failCount;
}


/****************************/
/*** Test data generators ***/
/****************************/

Tree OCTester::getHardTwoChoicesRightTree(double targetRatio){
list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.openCluster(1);
    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(5);
    tree.nodes[0].front().content.openCluster(6);
    tree.nodes[0].front().content.growClusterRight(7);
    tree.nodes[0].front().content.openCluster(8);
    tree.nodes[0].front().content.openCluster(8.5);
    tree.nodes[0].front().content.growClusterRight(9);

    return tree;

    return tree;
}

Tree OCTester::getEasyForcableTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(1, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);

//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

    return tree;
}

Tree OCTester::getStartForcableTree(double targetRatio){
        list<double> deltas = {1, -0.5, -1};
    Tree tree(1, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(4.5);


//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

//    tree.nodes[0].front().content.listClusters();
//    tree.nodes[0].front().content.listPoints();

    return tree;
}

Tree OCTester::getEndForcableTree(double targetRatio){
        list<double> deltas = {1, -0.5, -1};
    Tree tree(1, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(4.5);


//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

//    tree.nodes[0].front().content.listClusters();
//    tree.nodes[0].front().content.listPoints();

    return tree;
}

Tree OCTester::getHardForcableTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.openCluster(1);
    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(5);
    tree.nodes[0].front().content.openCluster(6);
    tree.nodes[0].front().content.growClusterRight(7);
    tree.nodes[0].front().content.openCluster(8);
    tree.nodes[0].front().content.growClusterRight(8.5);
//
//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

//    tree.nodes[0].front().content.listClusters();
//    tree.nodes[0].front().content.listPoints();

    return tree;


}

Tree OCTester::getHarderForcableTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().content.openCluster(1);
    tree.nodes[0].front().content.growClusterRight(2);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.openCluster(3);
    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(5);
    tree.nodes[0].front().content.openCluster(6);
    tree.nodes[0].front().content.growClusterRight(7);
    tree.nodes[0].front().content.openCluster(8);
    tree.nodes[0].front().content.openCluster(8.5);
    tree.nodes[0].front().content.growClusterRight(9);
    tree.nodes[0].front().content.growClusterRight(9.5);
    tree.nodes[0].front().content.openCluster(10);
    tree.nodes[0].front().content.growClusterRight(11);

//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

    return tree;
}

Tree OCTester::getStarterTestTree(double targetRatio){
list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, targetRatio, 2);

//    tree.nodes[0].front().content.openCluster(1);
//    tree.nodes[0].front().content.growClusterRight(2);
//    tree.nodes[0].front().content.openCluster(2.5);
//    tree.nodes[0].front().content.openCluster(3);
//    tree.nodes[0].front().content.growClusterRight(4);
//    tree.nodes[0].front().content.openCluster(5);
//    tree.nodes[0].front().content.openCluster(6);
//    tree.nodes[0].front().content.growClusterRight(7);
//    tree.nodes[0].front().content.openCluster(8);
//    tree.nodes[0].front().content.openCluster(8.5);
//    tree.nodes[0].front().content.growClusterRight(9);


    tree.nodes[0].front().content.growClusterRight(4);
    tree.nodes[0].front().content.openCluster(5);
    tree.nodes[0].front().content.openCluster(6);
    tree.nodes[0].front().content.openCluster(2);
    tree.nodes[0].front().content.growClusterLeft(1);
    tree.nodes[0].front().content.openCluster(0);
    tree.nodes[0].front().content.openCluster(2.5);
    tree.nodes[0].front().content.growClusterRight(7);
    tree.nodes[0].front().content.openCluster(8);
    tree.nodes[0].front().content.openCluster(8.5);
    tree.nodes[0].front().content.growClusterRight(9);
    tree.nodes[0].front().content.openCluster(10);
    tree.nodes[0].front().content.growClusterRight(11);
    tree.nodes[0].front().content.openCluster(9.5);




    return tree;
}


