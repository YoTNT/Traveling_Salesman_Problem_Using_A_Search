#include <iostream>
#include <string>

using namespace std;

class AstarNode
{
	public:
		int ID;
		int numEdgeFromStar;
		int gStar;	// the total cost from start node to this node
		int hStar;	// the estimate cost from this node to a goal node
		int fStar;	// gStar + hStar
		AstarNode* next;
		AstarNode* parent;

		// Constructors
		AstarNode();
		AstarNode(int);
};

AstarNode :: AstarNode()
{
	this->ID = 0;
	this->numEdgeFromStar = 0;
	this->gStar = 0;
	this->fStar = 0;
	this->fStar = 0;
	this->next = NULL;
	this->parent = NULL;
}

// The constructor need to be confirmed
AstarNode :: AstarNode(int id)
{
	this->ID = id;
	this->numEdgeFromStar = 0;
	this->next = NULL;
	this->parent = NULL;
}


class AStarSearch
{
	int numNodes;
	int star;
	int whichHFunction;
	int** costMatrix;
	AstarNode* OpenList;
	AstarNode* CloseList;
	int* childAry;

	// Constructors
	AStarSearch(int, int, int);

	// Methods
	void loadMatrix(string);
	void copyChildList(int);
	int computeHstar(AstarNode *, int);
	int computeGstar(AstarNode *);
	int computeFstar(AstarNode *);
	bool IsOnCloseList(AstarNode *, AstarNode *);
	void OpenInsert(AstarNode *);
	void print(string);
	AstarNode * OpenRemove();
	void ClosePush(AstarNode *);
	void CloseDelete(AstarNode *);
	bool checkPath(AstarNode *);
	void tracePath(AstarNode *, string);	// File name is needed


};

















int main(int argc, char ** argv)
{

	return 0;
}
