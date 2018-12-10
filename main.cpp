#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class AstarNode
{
	public:
		int ID;
		int numEdgeFromStart;
		int gStar;	// the total cost from start node to this node
		int hStar;	// the estimate cost from this node to a goal node
		int fStar;	// gStar + hStar
		AstarNode* next;
		AstarNode* parent;

		// Constructors
		AstarNode();
		AstarNode(int);
		AstarNode(int, int, int, int);
};

AstarNode :: AstarNode()
{
	this->ID = 0;
	this->numEdgeFromStart = 0;
	this->gStar = 0;
	this->hStar = 999;
	this->fStar = 999;
	this->next = NULL;
	this->parent = NULL;
}

AstarNode :: AstarNode(int id)
{
	this->ID = id;
	this->numEdgeFromStart = 0;
	this->gStar = 0;
	this->hStar = 0;
	this->fStar = 0;
	this->next = NULL;
	this->parent = NULL;
}

AstarNode :: AstarNode(int id, int hS, int gS, int fS)
{
	this->ID = id;
	this->hStar = hS;
	this->gStar = gS;
	this->fStar = fS;
	this->next = NULL;
	this->parent = NULL;
}

class edgeNode
{
	public:
		int edge;
		edgeNode* next;
	
		edgeNode();
		edgeNode(int);
};

edgeNode :: edgeNode()
{
	this->edge = -1;
	this->next = NULL;
}

edgeNode :: edgeNode(int e)
{
	this->edge = e;
	this->next = NULL;
}

class edgeLinkedList
{
	public:
		edgeNode* head;

		edgeLinkedList();
		void insert(edgeNode *);
		int findEdge(int);
};

edgeLinkedList :: edgeLinkedList()
{
	this->head = new edgeNode();
}

void edgeLinkedList :: insert(edgeNode *e)
{
	edgeNode* ptr = this->head;
	// empty linked list (exclusive dummy)
	if(ptr->next == NULL)
	{
		ptr->next = e;
		return;
	}

	// while the next one is not empty
	while(ptr->next != NULL)
	{
		// if e is smaller than the next one
		if(e->edge < ptr->next->edge)
		{
			// insert before the next one
			e->next = ptr->next;
			ptr->next = e;
			return;
		}
		// otherwise go to next step
		else
			ptr = ptr->next;
	}
	// e is the largest one, insert to the end
	ptr->next->next = e;
	return;
}

int edgeLinkedList :: findEdge(int index)
{
	edgeNode* ptr = this->head;
	for(int i = 0; i < index; i++)
	{
		if(ptr == NULL)
		{
			cout << "ERROR, edgeLinkedList reach to the end before finishing searching" << endl;
			return -1;
		}
		ptr = ptr->next;
	}
	return ptr->edge;
}




class AStarSearch
{
	public:
		int numNodes;
		int start;
		int whichHFunction;
		int** costMatrix;
		AstarNode* OpenList;
		AstarNode* CloseList;
		int* childAry;

		// Variables for h*
		int minimum = 99;
		int average = 0;
		int median = 0;

		// Constructors
		AStarSearch(int, int, int);

		// Methods
		void loadMatrix(string, string);
		void copyChildList(int);
		int computeHstar(AstarNode *, int);
		int computeGstar(AstarNode *);
		int computeFstar(AstarNode *);
		bool IsOnCloseList(AstarNode *, AstarNode *&);
		void OpenInsert(AstarNode *);
		void printOpen(string);
		void printClose(string);
		AstarNode * OpenRemove();
		void ClosePush(AstarNode *);
		void CloseDelete(AstarNode *);
		bool checkPath();
		void tracePath(string);	// File name is needed
		int sizeOfClose();
};

AStarSearch :: AStarSearch(int n, int s, int h)
{
	this->numNodes = n;
	this->start = s;
	this->whichHFunction = h;

	// Dynamic allocating costMatrix
	// n+1 by n+1, 1st column and row won't be used
	this->costMatrix = new int*[n+1];
	for(int i = 0; i < n+1; i++)
		costMatrix[i] = new int[n+1];
	// Initializing
	for(int i = 0; i < n+1; i++)
		for(int j = 0; j < n+1; j++)
			costMatrix[i][j] = -1;
	for(int i = 0; i < n+1; i++)
		costMatrix[i][i] = 0;

	// Dynammic alocating copyChildList
	// n+1, 1st element childAry[0] won't be used
	this->childAry = new int[n+1];
	// Initializing
	for(int i = 0; i < n+1; i++)
		childAry[i] = 0;
	
	AstarNode* dummy = new AstarNode();
	this->OpenList = dummy;	// Alocating OpenList with a dummy node
	this->CloseList = NULL;				// Alocating CloseList without dummy node
}

void AStarSearch :: loadMatrix(string input_file_name, string output_file_name)
{
	string line, word;
	int node1, node2, edge;
	int total = 0;
	int numOfNodes;
	int edges = 0;;
	edgeLinkedList* el = new edgeLinkedList();
	ifstream inFile;
	inFile.open(input_file_name);
	ofstream outFile(output_file_name, ifstream::trunc);	// Opening the output file and clean all first
	
	line = "** Below is the input graph for this program **\r\n\r\n";
	outFile << line;
	inFile >> word;	// Getting the number of nodes
	outFile << word << "\r\n";
	numOfNodes = atoi(word.c_str());

	while(inFile >> word)
	{
		// 1st node's ID
		line = word + " ";
		node1 = atoi(word.c_str());

		// 2nd node's ID
		inFile >> word;
		line += word + " ";
		node2 = atoi(word.c_str());

		// Edge cost
		inFile >> word;
		line += word + "\r\n";
		edge = atoi(word.c_str());
		// Updating costMatrix
		this->costMatrix[node1][node2] = edge;
		this->costMatrix[node2][node1] = edge;
		// Finding minimum
		if(this->minimum > edge)
			this->minimum = edge;
		// Calculating total for average
		total += edge;
		edges++;

		// Creating linked list for median
		edgeNode* e = new edgeNode(edge);
		el->insert(e);

		// Printing to output file
		outFile << line;
	}

	// Finding average
	this->average = total / edges;
	// Finding median
	this->median = el->findEdge(edges / 2);

	line = "\r\n** The start node is " + std::to_string(this->start) + "\r\n";
	outFile << line;
	line = "** h* function used is choice 1 or 2 or 3\r\n\r\n";
	outFile << line;
	
	inFile.close();
	outFile.close();

	return;
}

void AStarSearch :: copyChildList(int index)
{
	for(int i = 1; i <= this->numNodes; i++)
		this->childAry[i] = this->costMatrix[index][i];

	return;
}

int AStarSearch :: computeHstar(AstarNode * node, int h)
{
	int remainder = this->numNodes - node->numEdgeFromStart;
	int result = 0;
	switch(h)
	{
		// h1: Average * Remainder
		case 1:
			result = this->average * remainder; 
			break;
		// h2: Minimum * Remainder
		case 2:
			result = this->minimum * remainder;
			break;
		// h3: Median * Remainder
		case 3:
			result = this->median * remainder;
			break;
		default:
			cout << "ERROR, illegal input for h!" << endl;
			break;

	}
	return result;
}

// Total cost from source node to this current node
int AStarSearch :: computeGstar(AstarNode *node)
{
	// No parent, it's the source node
	if(node->parent == NULL)
		return 0;
	else
		return node->parent->gStar + this->costMatrix[node->parent->ID][node->ID];
}

int AStarSearch :: computeFstar(AstarNode *node)
{
	node->fStar = node->hStar + node->gStar;
	return node->fStar;
}

// Inserting node to the OPEN(with dummy header)
// Order: increasing order with fStar
void AStarSearch :: OpenInsert(AstarNode *node)
{
	AstarNode* ptr = this->OpenList;	// Points to the header(dummy)

	// OPEN is empty
	if(ptr->next == NULL)
	{
		node->next = ptr->next;
		ptr->next = node;
		return;
	}

	// Otherwise search the next node
	while(ptr->next != NULL)
	{
		// Smaller than the next node, insert before next node
		if(node->fStar < ptr->next->fStar)
		{
			node->next = ptr->next;
			ptr->next = node;
			return;
		}
		else
			// Check the next one
			ptr = ptr->next;
	}

	// The node need to put to the end of OPEN
	node->next = ptr->next;
	ptr->next = node;	
	return;
}

void print_append(string content, string file_name)
{
	ofstream outFile(file_name, fstream::app);
	outFile << content;
	outFile.close();
}

void file_cleaner(string file_name)
{
	ofstream outFile(file_name, ifstream::trunc);
	outFile.close();
}

void AStarSearch :: printOpen(string output_file_name)
{
	string line = "*** OPEN list ***\r\n";
	line += "OPEN -> dummy -> ";

	AstarNode* ptr = this->OpenList->next;
	while(ptr != NULL)
	{
		line += "(" + std::to_string(ptr->ID) +
			    ", " + std::to_string(ptr->fStar) +
			    ") -> ";
		ptr = ptr->next;
	}
	line += "NULL\r\n\r\n";
	print_append(line, output_file_name);
	
	return;
}

AstarNode * AStarSearch :: OpenRemove()
{
	AstarNode* ptr = this->OpenList->next;

	// Empty list
	if(ptr == NULL)
	{
		cout << "ERROR, cannot remove from an empty OPEN!" << endl;
		return NULL;
	}
	
	// Reconnect the head before return
	// Clean the next of the returning node
	this->OpenList->next = NULL;

	return ptr;
}

bool AStarSearch :: IsOnCloseList(AstarNode *cNode, AstarNode *&oNode)
{
	AstarNode* ptr = this->CloseList;

	while(ptr != NULL)
	{
		// CurrentNode is in the Close List
		if(cNode->ID == ptr->ID)
		{
			oNode = ptr;
			return true;
		}
		ptr = ptr->next;
	}
	// CurrentNode is not in the Close List
	return false;
}

void AStarSearch :: CloseDelete(AstarNode *node)
{
	AstarNode *ptr = this->CloseList;

	// If the first node in Close List is the node to found
	// And the Close List is not empty
	if(ptr != NULL and node->ID == ptr->ID)
	{
		this->CloseList = ptr->next;
		return;
	}

	// Otherwise check the node next to the current checking node
	// between the 2nd node to the (n-1)th node.
	while(ptr->next != NULL)
	{
		// Node is found
		if(node->ID == ptr->next->ID)
		{
			// Skip the found node
			ptr->next = ptr->next->next;
			return;
		}
		
		ptr = ptr->next;
	}

	// No more node, but can't found the node
	return;
}

void AStarSearch :: ClosePush(AstarNode *node)
{
	AstarNode* tail = this->CloseList;
	node->next = tail;
	this->CloseList = node;
	
	return;
}

void AStarSearch :: printClose(string output_file_name)
{
	string line = "*** CLOSE list ***\r\n";
	line += "CLOSE -> ";
     
	AstarNode* ptr = this->CloseList;
	while(ptr != NULL)
	{   
        line += "(" + std::to_string(ptr->ID) +
        	    ", " + std::to_string(ptr->fStar) +
	            ") -> ";
      	ptr = ptr->next;
    }
    line += "NULL\r\n\r\n";
    print_append(line, output_file_name);
    
    return;
}

bool AStarSearch :: checkPath()
{
	if(this->sizeOfClose() < this->numNodes)
		return true;
	else
		return false;
}

void AStarSearch :: tracePath(string output_file_name)
{
	int total_cost = 0;
	string line = "** The search result of the path:\r\n\r\n";
	line += "Start from node " + std::to_string(this->start) + " using h" +
			std::to_string(this->whichHFunction) + "* search\r\n\r\n";

	AstarNode *ptr = this->CloseList;
	int this_id, next_id;

	int start_id = ptr->ID;

	while(ptr != NULL and ptr->next != NULL)
	{
//		cout << "In while loop of checkPath" << endl;
		this_id = ptr->ID;
		next_id = ptr->next->ID;
		line += std::to_string(this_id) + "\t";
		line += std::to_string(next_id) + "\t";
		total_cost += this->costMatrix[this_id][next_id];
		line += std::to_string(this->costMatrix[this_id][next_id]) + "\r\n";
	
		ptr = ptr->next;
	}

	int end_id = ptr->ID;

	line += std::to_string(end_id) + "\t";
	line += std::to_string(start_id) + "\t";
	total_cost += this->costMatrix[end_id][start_id];
	line += std::to_string(this->costMatrix[end_id][start_id]) + "\r\n";



	line += "\r\nThe total cost of the simple-path is " + std::to_string(total_cost) + "\r\n";

	print_append(line, output_file_name);

	return;
}

int AStarSearch :: sizeOfClose()
{
	int size = 0;
	AstarNode* ptr = this->CloseList;
	while(ptr != NULL)
	{
		size++;
		ptr = ptr->next;
	}
	return size;
}


int main(int argc, char ** argv)
{
	string inputFileName = argv[1];
	string outputFileName = argv[2];
	string debugFileName = argv[3];

	file_cleaner(argv[2]);
	file_cleaner(argv[3]);

	int currentStep = 0;

	// Getting the number of nodes from input file.
	fstream inFile;
	inFile.open(inputFileName);
	int numOfNodes;
	if(inFile >> numOfNodes)
		inFile.close();

	// Getting the start node ID from user
	int startID;
	bool numNotLegal = true;
	while(numNotLegal)
	{
		cout << "Please input your start node's ID(between 1 to " << numOfNodes << "):" << endl;
		cin >> startID;
		if(startID >= 1 and startID <= numOfNodes)
			numNotLegal = false;
		else
			cout << "Illegal input!" << endl;
	}

	// Getting HFunction from user
	int h;
	bool hNotLegal = true;
	while(hNotLegal)
	{
		cout << "Please provide the HFunction you are using(1 <= H <= 3):" << endl;
		cin >> h;
		if(h >= 1 and h <= 3)
			hNotLegal = false;
		else
			cout << "Illegal input!" << endl;
	}

	// Creating an AStarSearch object
	AStarSearch* s = new AStarSearch(numOfNodes, startID, h);

	s->loadMatrix(inputFileName, outputFileName);

	// Assignment source node
	AstarNode* SNode = new AstarNode(startID);
	SNode->numEdgeFromStart = currentStep;		// This step is important before doing computeHstar !!!!
	SNode->hStar = s->computeHstar(SNode, h);
	SNode->gStar = s->computeGstar(SNode);
	SNode->fStar = s->computeFstar(SNode);

	s->OpenInsert(SNode);

	s->printOpen(argv[3]);

	AstarNode* currentNode = SNode;

	while(s->checkPath())
	{
		currentNode = s->OpenRemove();

		int matrixIndex = currentNode->ID;

		s->copyChildList(matrixIndex);

		currentStep++;	// Making sure the kid has the right numEdgeFromStart

		for(int childIndex = 1; childIndex <= s->numNodes; childIndex++)
		{	
			if(s->childAry[childIndex] > 0)
			{
				AstarNode* childNode = new AstarNode(childIndex);
				childNode->numEdgeFromStart = currentStep; 	// Updating the numEdgeFromStart for 
				childNode->hStar = s->computeHstar(childNode, h);
				childNode->parent = currentNode;			// Updating parent of childNode 
															// before using computeGstar function.
															// Making sure every kid has parent.
				childNode->gStar = s->computeGstar(childNode);
				childNode->fStar = s->computeFstar(childNode);

				AstarNode* oldNode = NULL;
				if(!s->IsOnCloseList(childNode, oldNode))
				{
					s->OpenInsert(childNode);
					s->printOpen(debugFileName);
				}
				else
				{	
					// Although childNode was processed, but this fStar is better
					if(childNode->fStar < oldNode->fStar)
					{	
						s->CloseDelete(oldNode);
						currentStep--;	// Take back a step
						s->OpenInsert(childNode);
						s->printOpen(debugFileName);
					}
				}
			}
		}
		s->ClosePush(currentNode);
		s->printClose(debugFileName);
	}
	s->tracePath(outputFileName);

	return 0;
}
