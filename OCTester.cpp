#include "OCTester.h"
int OCTester::failCount = 0;
int OCTester::successCount = 0;

OCTester::OCTester()
{

//    Tree({9.00 , 10.00 , 11.00 , 8.00 , 7.00 , 11.50 , 12.50 , 6.00 , 5.00 , 6.50 , 10.50 , 4 , 4.5 , 3 , 2 , 3.5 , 1 , 0 ,}, 101, 1.61);
//    exit(1);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 1.50 , 6.50 , 7.50 , 7.00 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 102, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 1.50 , 7.00 , 7.50 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 103, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 1.50 , 7.50 , 7.00 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 104, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 7.00 , 1.50 , 7.50 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 105, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 7.00 , 7.50 , 1.50 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 106, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 7.50 , 1.50 , 7.00 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 107, 1.61);
//    Tree({2.00 , 3.00 , 2.50 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 6.50 , 7.50 , 7.00 , 1.50 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 108, 1.61);
//    Tree({2.00 , 3.00 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 1.50 , 6.50 , 7.50 , 7.00 , 8.50 , 9.50 , 10.50 , 8.00 , 11.50}, 110, 1.61);
//    Tree({2, 3, 4, 1, 7, 8, 9, 6, 5}, 110, 1.61);
    //Tree({2.00 , 3.00 , 3.50 , 1.00 , 4.50 , 0.00 , 5.50 , 1.50 , 6.50 , 7.50 , 7.00 , 8.50 , 9.50 , 10.50 , 0.50 , 8.00 , 6.00 , 11.50}, 110, 1.61);
//    Tree({0.00}, 110, 1.60);
//    completeForceTest();
//    completeCheckPartitioningTest();
//    completeChildrenTest();
//    completeNodeTest();
//    completeChoiceTests();
//    completePointGenerationTests();
//    completeDepthFirstTests();
//    bottomLine();

}


/****************************/
/*** Complete test areas ****/
/****************************/

void OCTester::completeChildrenTest(){

    headlinePrint("CHILD TESTS");

    hardTwoChoicesRightTest()    ? successPrint("Two children right ") : failPrint("Two children Right ");
    addLevelBFTest()             ? successPrint("Add level BF ")       : failPrint("Add level BF ");
}

void OCTester::completeForceTest(){

    headlinePrint("FORCE TESTS");

    easyForceTest()         ? successPrint("Easy Force          ") : failPrint("Easy Force          ");
    startForceTest()        ? successPrint("Start Force         ") : failPrint("Start Force         ");
    endForceTest()          ? successPrint("End Force           ") : failPrint("End Force           ");
    hardForceTest()         ? successPrint("Hard Force          ") : failPrint("Hard Force          ");
    harderForceTest()       ? successPrint("Harder Force        ") : failPrint("Harder Force        ");
//    unkownForceTest()       ? successPrint("Unknown Force        ") : failPrint("Unknown Force        ");
}

void OCTester::completeCheckPartitioningTest(){

    headlinePrint("CHECK PARTITION TESTS");

    checkAboveRatioTest()   ? successPrint("Ratio is above ")   : failPrint("Ratio is above ");
    checkForceNeededTest()  ? successPrint("Force needed ")     : failPrint("Force needed ");
    checkUnforcableTest()   ? successPrint("Unforcable tree ")  : failPrint("Unforcable tree ");
    checkForRootNodeTest()  ? successPrint("Root node ")        : failPrint("Root node ");
}

void OCTester::completeNodeTest(){

    headlinePrint("NODE TESTS");

    createNewNodeTest()     ? successPrint("New Node      ") : failPrint("New Node      ");
    destroyNodeTest()       ? successPrint("Destroy Node  ") : failPrint("Destroy Node  ");

}

void OCTester::completeChoiceTests(){

    headlinePrint("CHOICE TESTS");

    threeChoiceTest()       ? successPrint("Three Choices    ") : failPrint("Three Choices   ");
    twoChoiceRightTest()    ? successPrint("Two Choices R    ") : failPrint("Two Choices R   ");
    twoChoiceLeftTest()     ? successPrint("Two Choices L    ") : failPrint("Two Choices L   ");
    oneChoiceTest()         ? successPrint("One Choice       ") : failPrint("One Choice      ");
    noChoiceTest()          ? successPrint("No Choice        ") : failPrint("No Choice       ");
    existingPointTest()     ? successPrint("Existing Point   ") : failPrint("Existing Point  ");

}

bool OCTester::startingSequence(){
//    list<double> deltas = {1, -0.5, -1};
//    Tree tree(0, deltas, 7357, 1.61, 2);

    //Set clusters
    Tree tree = getStarterTestTree(1.61);

//    cout << "Print setting is: " << tree.print << endl;

    tree.nodes[0].front().depth = 11;

//    cout << "Number of points, i.e. level: " << tree.nodes[0].front().content.nNumberOfPoints << endl;

    tree.startDF(11, 18);

//    exit(1);

    return true;
}

void OCTester::completePointGenerationTests(){

    headlinePrint("POINT GENERATION TESTS");

    threeUsualDeltasTest()      ? successPrint("Three Usual Deltas          ") : failPrint("Three Usual Deltas          ");
    withoutInputNodeTest()      ? successPrint("Next Points with no input   ") : failPrint("Next Points with no input   ");
    fromOnePointTest()          ? successPrint("From a single point         ") : failPrint("From a single point         ");

}


void OCTester::completeDepthFirstTests() {

    headlinePrint("DEPTH FIRST TESTS");

    oneNodeDFSuccessTest()              ? successPrint("One Node DF           ") : failPrint("One Node DF           ");
    twoNodeDFSuccessTest()              ? successPrint("Two Node DF           ") : failPrint("Two Node DF           ");
    threeNodeDFSuccessTest()            ? successPrint("Three Node DF         ") : failPrint("Three Node DF         ");
    threeNodeDFFailTest()               ? successPrint("Three Node DF No Proof") : failPrint("Three Node DF No Proof ");
    //~70 seconds
//    oneNodeHardDFSuccessTest()          ? successPrint("One Node DF Hard      ") : failPrint("One Node DF Hard          ");
    //~70 seconds
//    oneNodeHarderDFSuccessTest()        ? successPrint("One Node DF Harder    ") : failPrint("One Node DF Harder    ");
    //~70 seconds
//    oneNodeHardestDFSuccessTest()       ? successPrint("One Node DF Hardest   ") : failPrint("One Node DF Hardest   ");

//    oneNodeUnknownDFTest()              ? successPrint("Three Node DF No Proof") : failPrint("Three Node DF No Proof ");
}

/****************************/
/*** Force tests    *********/
/****************************/

bool OCTester::easyForceTest(){


    Tree tree = getEasyForcableTree(1.60);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

    list<double> forcePoints = nodeToUse->content.getForcePoints();


    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node" << endl;
        ++failCount;
        return false;
    }

    if(tree.nodes[0].front().content.forcedPoints.size() != 3 ){
        cout << "Not forced with the correct number of points" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

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

    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node" << endl;
        ++failCount;
        return false;
    }

    if(tree.nodes[0].front().content.forcedPoints.size() != 1 ){
        cout << "Not forced with the correct number of points" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

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

    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node" << endl;
        ++failCount;
        return false;
    }

    if(tree.nodes[0].front().content.forcedPoints.size() != 1 ){
        cout << "Not forced with the correct number of points" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

}

bool OCTester::hardForceTest(){
    ///Get ready
    Tree tree = getHardForcableTree(1.61);
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

    list<double> forcePoints = nodeToUse->content.getForcePoints();

    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node" << endl;
        ++failCount;
        return false;
    }

    if(tree.nodes[0].front().content.forcedPoints.size() != 4 ){
        cout << "Not forced with the correct number of points" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

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

    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node" << endl;
        ++failCount;
        return false;
    }

    if(tree.nodes[0].front().content.forcedPoints.size() != 6 ){
        cout << "Not forced with the correct number of points" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

}

bool OCTester::unkownForceTest(){
    ///Get ready
    Tree tree = getUnknownForceTree();
    TreeNode *nodeToUse = &tree.nodes[0].front();
    nodeToUse->content.calcRatio();
    nodeToUse->content.setAmbSpace();
    list<double> ambPoints = tree.nodes[0].front().content.getAmbPoints();

//    cout << "Number of ambiguous points: " << ambPoints.size() << endl;
    list<double> forcePoints = nodeToUse->content.getForcePoints();

//    cout << "Number of forcePoints: " << forcePoints.size() << endl;
//    cout << "Nodes in sorted list " << nodeToUse->content.sortedPoints.size() << endl;

    if(tree.force(nodeToUse, nodeToUse, forcePoints, ambPoints)){
        cout << "Failed to for a forcable node In Unkown" << endl;
        ++failCount;
        return false;
    }


    ++successCount;
    return true;

}

/****************************/
/*** Check Partitioning tests ***/
/****************************/

bool OCTester::checkAboveRatioTest(){

    Tree tree = getTreeAboveRatio();

    if( !tree.checkPartitioning() )
    {
        ++successCount;
        return true;
    }
    ++failCount;
    return false;

}

bool OCTester::checkForceNeededTest(){

    Tree tree = getHardForcableTree(1.61);
    tree.nodes[0].front().depth = 1;

    if( !tree.checkPartitioning() )
    {
        ++successCount;
        return true;
    }
    ++failCount;
    return false;
}

bool OCTester::checkUnforcableTest(){

    Tree tree = getHardTwoChoicesRightTree(1.61);
    tree.nodes[0].front().depth = 1;

    if( tree.checkPartitioning() )
    {
       ++successCount;
        return true;
    }
    ++failCount;
    return false;
}

bool OCTester::checkForRootNodeTest(){

    Tree tree = getHardTwoChoicesRightTree(1.61);

    if( tree.checkPartitioning() )
    {
        ++successCount;
        return true;
    }
    ++failCount;
    return false;
}

/****************************/
/***** Child Creation Tests    ******/
/****************************/

bool OCTester::hardTwoChoicesRightTest(){

    Tree tree = getHardTwoChoicesRightTree(1.61);
    TreeNode* nodeToUse = &tree.nodes[0].front();
    tree.addPointDF_experimental(10, nodeToUse);

    if(nodeToUse->children.size() == 1){
        ++successCount;
        return true;
    }
    ++failCount;
    return false;
}

bool OCTester::addLevelBFTest(){
    Tree tree = getTreeForBFLevel();

    tree.addLevelBF(2);

    if( tree.nodes[0].size() == 9 ){
        ++successCount;
        return true;
    }
    ++failCount;
    return false;

}

/****************************/
/*** Node tests ***/
/****************************/

bool OCTester::createNewNodeTest(){

    Tree tree = getSimpleTree();

    if(tree.nodes[0].size() != 1){
        cout << "Size of newly generated tree is not 1" << endl;
        ++failCount;
        return false;
    }

    tree.newNode( 0, 2.0, &tree.nodes[0].front() );

    if( tree.nodes[0].size() != 2 ){
        cout << "Size of tree after new node is not 2" << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::destroyNodeTest(){

    Tree tree = getSimpleTree();

    tree.newNode( 0, 2.0, &tree.nodes[0].front() );
    tree.destroyNode( &tree.nodes[0].front() );

    if( tree.nodes[0].size() != 1 ){
        cout << "Node was not erased" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;
}


/****************************/
/*** Choice tests ***/
/****************************/

bool OCTester::threeChoiceTest(){
   Tree tree = getChoiceTreeTwoThree();

    if(tree.nodes[0].front().content.pointInRange(4) != 3){
        cout << "Failed at 4" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(5.25) != 3){
            cout << "Failed at 5.25" << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::twoChoiceRightTest(){
   Tree tree = getChoiceTreeTwoThree();

    if(tree.nodes[0].front().content.pointInRange(3.75) != 2){
        cout << "Failed at 4" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(6.5) != 2){
            cout << "Failed at 6.5" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(6.25) != 2){
            cout << "Failed at 6.25" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;
}

bool OCTester::twoChoiceLeftTest(){
   Tree tree = getChoiceTreeTwoThree();

    if(tree.nodes[0].front().content.pointInRange(2.5) != -2){
        cout << "Failed at 2.5" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(2.75) != -2){
            cout << "Failed at 2.75" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(4.25) != -2){
            cout << "Failed at 4.25" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;
}

bool OCTester::oneChoiceTest(){
    Tree tree = getChoiceTreeZeroOne();

    if(tree.nodes[0].front().content.pointInRange(3.5) != 1){
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(5.5) != 1){
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::noChoiceTest(){
    Tree tree = getChoiceTreeZeroOne();

    if(tree.nodes[0].front().content.pointInRange(2.5) != 0){
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(4.5) != 0){
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.pointInRange(6.5) != 0){
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::existingPointTest(){
       Tree tree = getExistPointTree();

    if(tree.nodes[0].front().doesPointExist(-3)){
        cout << "Failed at 1" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().doesPointExist(-2)){
        cout << "Failed at -2" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().doesPointExist(-1)){
        cout << "Failed at -1" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().doesPointExist(0)){
        cout << "Failed at 0" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.doesPointExist(1)){
        cout << "Failed at 1" << endl;
        ++failCount;
        return false;
    }
    if(tree.nodes[0].front().content.doesPointExist(2)){
        cout << "Failed at 2" << endl;
        ++failCount;
        return false;
    }
        if(tree.nodes[0].front().content.doesPointExist(3)){
        cout << "Failed at 3" << endl;
        ++failCount;
        return false;
    }
    if(!tree.nodes[0].front().content.doesPointExist(-4)){
        cout << "Failed at -4" << endl;
        ++failCount;
        return false;
    }
    if(!tree.nodes[0].front().content.doesPointExist(-2.5)){
        cout << "Failed at -2.5" << endl;
        ++failCount;
        return false;
    }
    if(!tree.nodes[0].front().content.doesPointExist(0.2)){
        cout << "Failed at 0.2" << endl;
        ++failCount;
        return false;
    }
    if(!tree.nodes[0].front().content.doesPointExist(2.5)){
        cout << "Failed at 2.5" << endl;
        ++failCount;
        return false;
    }
    if(!tree.nodes[0].front().content.doesPointExist(4)){
        cout << "Failed at 4" << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;

}



/****************************/
/*** Point generation tests**/
/****************************/

bool OCTester::threeUsualDeltasTest(){
    Tree tree = getEasyForcableTree(1.6);

    TreeNode *nodeToUse = &tree.nodes[0].front();

    list<double> controlList = { 5 , 3.5 , 2.5 , 1.5 , 0.5 , 0 };
    list<double> nextPointList = tree.getNextPoints( nodeToUse );

    if ( nextPointList.size() != controlList.size() ){
        cout << "Failed at size of list" << endl;
        ++failCount;
        return false;
    }

    list<double>::iterator result;

    for ( result = nextPointList.begin(); result != nextPointList.end() ; result++ ){
        if ( (*result) != controlList.front() ){
            cout << "Didn't get point: "<< (*result) << endl;
            ++failCount;
            return false;
        }
        controlList.pop_front();
    }
    ++successCount;
    return true;

}

bool OCTester::withoutInputNodeTest(){
    Tree tree = getEasyForcableTree(1.6);



    list<double> controlList = { 5 , 3.5 , 2.5 , 1.5 , 0.5 , 0 };
    list<double> nextPointList = tree.getNextPoints();

    if ( nextPointList.size() != controlList.size() ){
        cout << "Failed at size of list" << endl;
        ++failCount;
        return false;
    }

    list<double>::iterator result;

    for ( result = nextPointList.begin(); result != nextPointList.end() ; result++ ){
        if ( (*result) != controlList.front() ){
            cout << "Didn't get point: "<< (*result) << endl;
            ++failCount;
            return false;
        }
        controlList.pop_front();
    }
    ++successCount;
    return true;


}

bool OCTester::fromOnePointTest(){
 Tree tree = getSimpleTree();

    list<double> controlList = { 4 , 3.5 , 2 };
    list<double> nextPointList = tree.getNextPoints();

    if ( nextPointList.size() != controlList.size() ){
        cout << "Failed at size of list" << endl;
        ++failCount;
        return false;
    }

    list<double>::iterator result;

    for ( result = nextPointList.begin(); result != nextPointList.end() ; result++ ){
        if ( (*result) != controlList.front() ){
            cout << "Didn't get point: "<< (*result) << endl;
            ++failCount;
            return false;
        }
        controlList.pop_front();
    }
    ++successCount;
    return true;

}


/****************************/
/***   Test depth first   ***/
/****************************/

bool OCTester::oneNodeDFSuccessTest(){

    Tree tree = getDFsearchTreeOneNode();

    if ( tree.startDF(0, 5) ){
        cout << "Didn't found proof, where there is one.." << endl;
        ++failCount;
        return false;
    }

    ++successCount;
    return true;
}

bool OCTester::twoNodeDFSuccessTest(){

    Tree tree = getDFsearchTreeTwoNode();

    if ( tree.miniQueueDF(tree.sequencedTreeQueue.front(), 1, 6 ) ){
        cout << "Didn't found proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::threeNodeDFSuccessTest(){

    Tree tree = getDFsearchTreeThreeNode();

    if ( tree.miniQueueDF(tree.sequencedTreeQueue.front(), 1, 6 ) ){
        cout << "Didn't found proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::threeNodeDFFailTest(){
   Tree tree = getFailingDFsearchTreeThreeNode();

    if ( !tree.miniQueueDF(tree.sequencedTreeQueue.front(), 1, 6 ) ){
        cout << "Found a proof where there isn't one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::oneNodeHardDFSuccessTest(){
    Tree tree = getHardDFsearchTreeOneNode();

    if ( tree.startDF(0, 7) ){
        cout << "Didn't found proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::oneNodeHarderDFSuccessTest(){
    Tree tree = getHarderDFsearchTreeOneNode();

    if ( tree.startDF(0, 9) ){
        cout << "Didn't found proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
//    tree.analyzeTree(0);
    return true;
}

bool OCTester::oneNodeHardestDFSuccessTest(){
    Tree tree = getHardestDFsearchTreeOneNode();

    if ( tree.startDF(0, 10) ){
        cout << "Didn't find proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}

bool OCTester::oneNodeUnknownDFTest(){
    Tree tree = getUnknownForceTree();

    if ( tree.startDF(0, 3) ){
        cout << "Didn't find proof, where there is one.." << endl;
        ++failCount;
        return false;
    }
    ++successCount;
    return true;
}


/****************************/
/*** Test Parallel Helper ***/
/****************************/

bool OCTester::splitSequenceQueue(){

    Tree tree(0, {1, -1, 0.5}, 1111, 1.6, 2);

    for(int level = 0; level < 3; level++)
    {
        cout << "Level: " << level << endl;
        cout << "NodeQueue size: " << tree.nodeQueue.size() << endl;
        cout << endl;
        tree.addLevelBF(level);
        tree.treeCleanup();
    }



    cout << "Size of BF queue after initial levels: "<< tree.nodeQueue.size() << endl;
    if(tree.nodeQueue.size() != 12){
        return false;
    }

    tree.sequenceTree();

    cout << "Size of sequenceQueue after prep: " << tree.sequencedTreeQueue.size() << endl;

    tree.splitSequenceTree();




    return false;
}

/****************************/
/*** Test data generators ***/
/****************************/

Tree OCTester::getHardTwoChoicesRightTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);

    return tree;

    return tree;
}

Tree OCTester::getEasyForcableTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(1, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);

//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

    return tree;
}

Tree OCTester::getStartForcableTree(double targetRatio){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(1, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(4.5);


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

    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(4.5);


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

    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().growClusterRight(8.5);
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

    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().growClusterRight(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);
    tree.nodes[0].front().growClusterRight(9.5);
    tree.nodes[0].front().openCluster(10);
    tree.nodes[0].front().growClusterRight(11);




//    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

    return tree;
}

Tree OCTester::getUnknownForceTree(){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(1);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().openCluster(3);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().growClusterRight(6);
    tree.nodes[0].front().openCluster(6.5);
    tree.nodes[0].front().openCluster(7.5);
    tree.nodes[0].front().growClusterRight(8.5);
    tree.nodes[0].front().openCluster(9.5);
    tree.nodes[0].front().openCluster(10.5);
    tree.nodes[0].front().growClusterRight(11.5);
    tree.nodes[0].front().openCluster(12.5);

    //    cout << endl;
//    cout << "Created tree with starting node containing " << tree.nodes[0].front().content.nNumberOfPoints << " points, and " << tree.nodes[0].front().content.nNumberOfClusters << " clusters" << endl;
//    cout << endl;

    return tree;
}


Tree OCTester::getStarterTestTree(double targetRatio){
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, targetRatio, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);
//    tree.nodes[0].front().openCluster(10);
//    tree.nodes[0].front().growClusterRight(11);
//    tree.nodes[0].front().openCluster(9.5);
    return tree;
}

Tree OCTester::getDFsearchTreeOneNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);

    return tree;
}

Tree OCTester::getHardDFsearchTreeOneNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);

    return tree;
}

Tree OCTester::getHarderDFsearchTreeOneNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
//    tree.nodes[0].front().growClusterRight(7);

    return tree;
}

Tree OCTester::getHardestDFsearchTreeOneNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
//    tree.nodes[0].front().openCluster(2.5);


    return tree;
}

Tree OCTester::getDFsearchTreeTwoNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);

    list<TreeNode*> miniQueue;

    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );
    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );



    tree.sequencedTreeQueue.push_front( miniQueue ) ;

//    cout << " Size of the miniqueue in sequence queue " << tree.sequencedTreeQueue.front().size() << endl ;

    return tree;
}

Tree OCTester::getDFsearchTreeThreeNode() {
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.61, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);

    list<TreeNode*> miniQueue;

    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );
    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );
    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );



    tree.sequencedTreeQueue.push_front( miniQueue ) ;

//    cout << " Size of the miniqueue in sequence queue " << tree.sequencedTreeQueue.front().size() << endl ;

    return tree;
}

Tree OCTester::getFailingDFsearchTreeThreeNode() {
    list<double> deltas = {1, 0.5, -1};
    list<TreeNode*> miniQueue;

    Tree tree(3, deltas, 7357, 1.61, 2);
    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);

    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );

    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);



    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );
    miniQueue.push_back( tree.newNode(0, 0, &tree.nodes[0].back() ) );



    tree.sequencedTreeQueue.push_front( miniQueue ) ;

//    cout << " Size of the miniqueue in sequence queue " << tree.sequencedTreeQueue.front().size() << endl ;

    return tree;
}

Tree OCTester::getSimpleTree(){
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.6, 2);

    return tree;
}

Tree OCTester::getChoiceTreeZeroOne(){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(3, deltas, 7357, 1.6, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().growClusterRight(6);

    return tree;
}

Tree OCTester::getChoiceTreeTwoThree(){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(3, deltas, 7357, 1.6, 2);

    tree.nodes[0].front().growClusterRight(3.5);
    tree.nodes[0].front().openCluster(4.5);
    tree.nodes[0].front().growClusterRight(5);
    tree.nodes[0].front().openCluster(5.5);
    tree.nodes[0].front().growClusterRight(6);


    return tree;
}

Tree OCTester::getExistPointTree(){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(-3, deltas, 7357, 1.6, 2);

    tree.nodes[0].front().growClusterRight(-2);
    tree.nodes[0].front().openCluster(-1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterRight(3);

    return tree;
}

Tree OCTester::getTreeAboveRatio(){
    list<double> deltas = {1, -0.5, -1};
    Tree tree(0, deltas, 7357, 1.6, 2);

    tree.nodes[0].front().openCluster(0.5);
    tree.nodes[0].front().growClusterRight(1.5);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(3.5);
    tree.nodes[0].front().openCluster(4);

    return tree;
}

Tree OCTester::getTreeForBFLevel(){
    list<double> deltas = {1, -1};
    Tree tree(2, deltas, 7357, 2, 2);
    tree.nodes[0].front().growClusterRight(3);

    TreeNode *rootNode = &tree.nodes[0].front();

    tree.nodeQueue.push_front( tree.newNode(0, 1.0, rootNode) );
    tree.nodeQueue.front()->openCluster(1.0);


    tree.nodeQueue.push_front( tree.newNode(0, 4.0, rootNode) );
    tree.nodeQueue.front()->openCluster(4.0);

    return tree;

}

Tree OCTester::getDefaultStartTree(){
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.63, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);

    return tree;
}

Tree OCTester::getNewLowerBoundTree(){
    list<double> deltas = {1, 0.5, -1};
    Tree tree(3, deltas, 7357, 1.63, 2);

    tree.nodes[0].front().growClusterRight(4);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().openCluster(6);
    tree.nodes[0].front().openCluster(2);
    tree.nodes[0].front().growClusterLeft(1);
    tree.nodes[0].front().openCluster(0);
    tree.nodes[0].front().openCluster(2.5);
    tree.nodes[0].front().growClusterRight(7);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9);
    tree.nodes[0].front().openCluster(10);
    tree.nodes[0].front().growClusterRight(11);


    return tree;

}

Tree OCTester::getNewBaseTree(){
//        2.00  3.00  2.50  3.50  1.00  4.50  0.00  5.50
//        [ 0.00 , 1.00 ] [ 2.00 , 3.00 ] [ 3.50 , 3.50 ] [ 4.50 , 5.50 ]
    list<double> deltas = {1, 0.5, -1};
    Tree tree(2, deltas, 7357, 1.63, 2);

    tree.nodes[0].front().growClusterRight(3);
//    tree.nodes[0].front().addPointToCluster(2.5);
    tree.nodes[0].front().openCluster(3.5);
    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().openCluster(4.5);
    tree.nodes[0].front().growClusterLeft(0);
    tree.nodes[0].front().growClusterRight(5.5);
    //Till here can easily be proven
    tree.nodes[0].front().openCluster(1.5);
    tree.nodes[0].front().openCluster(6.5);
    tree.nodes[0].front().openCluster(7.5);
    tree.nodes[0].front().growClusterLeft(7);
    tree.nodes[0].front().openCluster(8.5);
    tree.nodes[0].front().growClusterRight(9.5);
    tree.nodes[0].front().openCluster(10.5);



    return tree;

}


Tree OCTester::getSecondBaseTree(){
        // 2, 3, 4, 1, 7, 8, 9, 6, 5
    list<double> deltas = {1, 0.5, -1};
    Tree tree(2, deltas, 7357, 1.63, 2);

    tree.nodes[0].front().growClusterRight(3);
    tree.nodes[0].front().openCluster(4);
    tree.nodes[0].front().openCluster(1);
    tree.nodes[0].front().openCluster(7);
    tree.nodes[0].front().growClusterRight(8);
    tree.nodes[0].front().openCluster(9);
    tree.nodes[0].front().openCluster(6);

    return tree;

}

Tree OCTester::getPromisingTree(){
//9.00 , 10.00 , 11.00 , 8.00 , 7.00 , 11.50 , 12.50 , 6.00 , 5.00 , 6.50

    list<double> deltas = {1, 0.5, -1};
    Tree tree(9, deltas, 7357, 1.63, 2);

    tree.nodes[0].front().growClusterRight(10);
    tree.nodes[0].front().openCluster(11);
    tree.nodes[0].front().openCluster(8);
    tree.nodes[0].front().openCluster(7);
    tree.nodes[0].front().growClusterRight(11.5);
    tree.nodes[0].front().openCluster(12.5);
    tree.nodes[0].front().growClusterLeft(6);
    tree.nodes[0].front().openCluster(5);
    tree.nodes[0].front().addPointToCluster(6.5);

    return tree;
}

/****************************/
/*** I/O Functions ***/
/****************************/

void OCTester::successPrint(string testName){
    cout << " SUCCESS :  " << testName << endl;
}

void OCTester::failPrint(string testName){
    cout << " FAILED :   " << testName << endl;

}

void OCTester::headlinePrint(string headline){
    cout << endl;
    cout << "------------------         " << headline << endl;
    cout << endl;
}

void OCTester::bottomLine(){

    cout << endl;
    if(failCount > 0){
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!" << failCount << " TESTS FAILED " << "!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        }
    else {
        cout << "----------------------------------------------------------------------------" << endl;
        cout << "---------------------- All " << successCount << " Tests where completed! " << "----------------------" << endl;
        cout << "----------------------------------------------------------------------------" << endl;
    }
    cout << endl;

}
