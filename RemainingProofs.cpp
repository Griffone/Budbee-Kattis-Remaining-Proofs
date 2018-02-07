/*
	Grigory Glukhov's solution to Budbee Kattis Programming Aptitude test, part B:

	2018-02-07
*/

#include <iostream>
#include <list>

using namespace std;

typedef unsigned short	vtxID;
#define UNDEFINED_INDEX	0x8000
#define SCCC_OUTGOING	0x1
#define SCCC_INCOMING	0x2

#define min(a, b)		(a < b) ? a : b

// Main means of solving the problem
class Graph {
public:
	Graph(const vtxID & vertexCount);
	~Graph();

	void insert(const vtxID & src, const vtxID & dst);

	unsigned int getRemainingProofCount();

private:
	list<vtxID>	*verts;
	vtxID		vtxCount;

	// Parts required for finding connected components.
	vtxID		index, *indicies, *lowLinks;
	bool		*onStack;
	list<vtxID>	stack;
	void strongConnect(const vtxID & vertex, list<list<vtxID>> & retList);
};

// Isolated test case
inline unsigned int testCase() {
	vtxID vertexCount, edgeCount;
	cin >> vertexCount >> edgeCount;

	Graph g = Graph(vertexCount);
	vtxID a, b;
	for (vtxID i = 0; i < edgeCount; i++) {
		cin >> a >> b;
		g.insert(a - 1, b - 1);
	}
	return g.getRemainingProofCount();
}

int main(int argc, char *argv[]) {
	// Optimize io speed
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	// Read number of cases
	short testCases;
	cin >> testCases;

	// Solve each case individually
	for (short i = 0; i < testCases; i++)
		cout << testCase() << '\n';

	// Left for testing purposes
	cin >> testCases;
}

Graph::Graph(const vtxID & vc) : vtxCount(vc) {
	verts = new list<vtxID>[vc];
	indicies = new vtxID[vc];
	lowLinks = new vtxID[vc];
	onStack = new bool[vc];
}

Graph::~Graph() {

#ifdef _WIN32
	delete[vtxCount] verts;
	delete[vtxCount] indicies;
	delete[vtxCount] lowLinks;
	delete[vtxCount] onStack;
#else // _WIN32
	delete[] verts;
	delete[] indicies;
	delete[] lowLinks;
	delete[] onStack;
#endif // _WIN32

}

void Graph::insert(const vtxID & src, const vtxID & dst) {
	verts[src].push_back(dst);
}

unsigned int Graph::getRemainingProofCount() {

	// Setup/reset
	index = 0;
	stack.clear();
	vtxID *vertSCC = new vtxID[vtxCount];
	for (vtxID i = 0; i < vtxCount; i++) {
		indicies[i] = UNDEFINED_INDEX;
		onStack[i] = false;
	}

	// Tarjan find all strongly connected components
	list<list<vtxID>> sccList = list<list<vtxID>>();
	for (vtxID i = 0; i < vtxCount; i++)
		if (indicies[i] == UNDEFINED_INDEX)
			strongConnect(i, sccList);

	// Generate Strongly Connected Components:
	auto sccCount = sccList.size();
	char *components = new char[sccCount];
	for (vtxID i = 0; i < sccCount; i++) {
		components[i] = 0;
		list<vtxID> scc = sccList.front();
		sccList.pop_front();
		// Map verticies to SCCs
		for (auto it = scc.begin(); it != scc.end(); ++it)
			vertSCC[*it] = i;
	}

	// Check connections of SCCs:
	for (vtxID i = 0; i < vtxCount; i++)
		for (auto it = verts[i].begin(); it != verts[i].end(); ++it)
			if (vertSCC[*it] != vertSCC[i]) {
				components[vertSCC[i]] |= SCCC_OUTGOING;
				components[vertSCC[*it]] |= SCCC_INCOMING;
			}

	// Finally count both ends of components:
	vtxID endsNoOutgoing = 0;
	vtxID endsNoIncoming = 0;
	
	for (vtxID i = 0; i < sccCount; i++) {
		if ((components[i] & SCCC_OUTGOING) == 0)
			endsNoOutgoing++;
		if ((components[i] & SCCC_INCOMING) == 0)
			endsNoIncoming++;
	}

	// Clean up
#ifdef _WIN32
	delete[sccCount] components;
	delete[vtxCount] vertSCC;
#else // _WIN32
	delete[] components;
	delete[] vertSCC;
#endif // _WIN32


	// Ask Grigory why this is the answer. Short version: this is absolute minimal number of moves you need to make everything a single SCC.
	if (sccCount == 1)
		return 0;
	return endsNoIncoming > endsNoOutgoing ? endsNoIncoming : endsNoOutgoing;
}

void Graph::strongConnect(const vtxID & v, list<list<vtxID>> & retList) {
	indicies[v] = index;
	lowLinks[v] = index;
	index++;
	stack.push_front(v);
	onStack[v] = true;

	for (auto it = verts[v].begin(); it != verts[v].end(); ++it) {
		if (indicies[*it] == UNDEFINED_INDEX) {
			strongConnect(*it, retList);
			lowLinks[v] = min(lowLinks[*it], lowLinks[v]);
		} else if (onStack[*it])
			lowLinks[v] = min(indicies[*it], lowLinks[v]);
	}

	// If a vertex is a root node, generate SCC
	if (lowLinks[v] == indicies[v]) {
		list<vtxID> scc = list<vtxID>();
		vtxID w;
		do {
			w = stack.front();
			stack.pop_front();
			onStack[w] = false;
			scc.push_back(w);
		} while (w != v);
		retList.push_back(scc);
	}
}