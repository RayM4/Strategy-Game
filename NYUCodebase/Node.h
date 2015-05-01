#include <vector>

class Node {
public:
	Node(float x, float y) : x(x), y(y){}
	float x;
	float y;
	bool visited;
	bool occupied;
	int cost;
	int curCost;
	Node *cameFrom;
	Node* top;
	Node* bot;
	Node* left;
	Node* right;
	//std::vector<Node*> connected;
};