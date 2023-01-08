#include <vector>
#include <queue>
#include <unordered_map>
#include "Triangle.h"
using namespace std;

class Graph
{
public:
	Graph(int rows, int cols, int stepSize, vector<Triangle *> triangles)
	{
		this->rows = int(rows / stepSize) * 2;
		this->cols = int(cols / stepSize);
		this->triangles = triangles;
		this->n = triangles.size();
		printf("rows: %d, cols: %d, n: %d\n", this->rows, this->cols, n);

		// the graph is for triangles, we can init n nodes with 3 neighbors
		this->graph = vector<vector<int>>(n, vector<int>(3, -1));

		// init the graph
		for (int i = 0; i < triangles.size(); i++)
		{
			// if i - 1 is in the same row, then i - 1 is the left neighbor
			if (sameRow(i, i - 1) && inBounds(i - 1))
			{
				graph[i][0] = i - 1;
			}

			// if i is odd, then i + (2 * cols) - 1  is the top neighbor
			// if i is even, then i - (2 * cols) - 1 is the bottom neighbor
			int colNeighbor;
			colNeighbor = (i % 2 == 0) ? (i - (this->rows - 1)) : (i + (this->rows - 1));
			if (inBounds(colNeighbor))
			{
				graph[i][1] = colNeighbor;
			}


			// if i + 1 is in the same row, then i + 1 is the right neighbor
			if (sameRow(i, i + 1) && inBounds(i + 1))
			{
				graph[i][2] = i + 1;
			}
		}
		initWeights();
	}

	bool sameRow(int i, int j)
	{
		return int(i / cols) == int(j / cols);
	}

	bool sameCol(int i, int j)
	{
		return i % cols == j % cols;
	}

	bool inBounds(int i)
	{
		int row = i / cols;
		int col = i % cols;
		return row >= 0 && row < rows && col >= 0 && col < cols && i < n;
	}

	vector<int> getNeighbors(int i)
	{
		vector<int> neighbors;
		for (int neighbor : graph[i])
		{
			// return only the valid neighbors
			if (neighbor != -1)
			{
				neighbors.push_back(neighbor);
			}
		}
		return neighbors;
	}


	float EuclideanDistance(array<float, 3> p1, array<float, 3> p2) {
		float dx = p1[0] - p2[0];
		float dy = p1[1] - p2[1];
		float dz = p1[2] - p2[2];
		return sqrt(dx * dx + 100.0 * (dy * dy) + dz * dz);

	}

	void initWeights() {
		// init weights with -1
		this->weights = vector<vector<float>>(n, vector<float>(3, -1));

		for (unsigned int i = 0; i < graph.size(); i++) {
			for (unsigned int j = 0; j < 3; j++) {
				if (graph[i][j] != -1) {
					int neighborId = graph[i][j];
					weights[i][j] = EuclideanDistance(triangles[i]->getCenter(), triangles[neighborId]->getCenter());
				}
			}
		}

	}

	struct Vertex
	{
		int id;
		float distance;
		Vertex(int id, float distance) : id(id), distance(distance) {}
	};

	struct VertexCompare
	{
		bool operator()(Vertex a, Vertex b)
		{
			return a.distance > b.distance;
		}
	};


	vector<int> dijkstra(int start, int end)
	{
		// Initialize the distance table with the distances from the start vertex
		unordered_map<int, float> distance;
		for (int i = 0; i < n; i++)
		{
			distance[i] = (i == start) ? 0 : numeric_limits<float>::infinity();
		}

		// Initialize the priority queue with the vertices and their distances from the start vertex
		priority_queue<Vertex, vector<Vertex>, VertexCompare> pq;
		for (int i = 0; i < n; i++)
		{
			pq.emplace(i, distance[i]);
		}

		// Initialize the predecessor map to store the shortest path
		unordered_map<int, int> predecessor;

		// Run the main loop of Dijkstra's algorithm
		while (!pq.empty())
		{
			Vertex u = pq.top();
			pq.pop();
			if (u.id == -1) {
				continue;
			}

			// Stop when we reach the end vertex
			if (u.id == end)
			{
				break;
			}

			// Relax the distances of the neighbors of u
			vector<int> neighbors = graph[u.id];
			for (int v : neighbors)
			{
				if (v != u.id) {
					int neighborIndex = -1;
					for (int k = 0; k < 3; k++) {
						if (neighbors[k] == v) {
							neighborIndex = k;
						}
					}
					float alt = distance[u.id] + weights[u.id][neighborIndex]; // use the weights variable to look up the weight of the edge
					if (alt < distance[v])
					{
						distance[v] = alt;
						predecessor[v] = u.id;
						pq.emplace(v, alt);
					}
				}
			}
		}

		// Construct the shortest path by following the predecessor map
		vector<int> path;
		int current = end;
		while (current != start)
		{
			path.push_back(current);
			current = predecessor[current];
		}
		path.push_back(start);
		reverse(path.begin(), path.end());
		printf("dijkstra done1\n");
		for (int i = 0; i < path.size(); i++) {
			printf("%d\n", path.at(i));
		}
		printf("dijkstra done2\n");
		return path;

	}


	vector<int> BFS(int i, int j)
	{
		// Check if the indices are valid
		if (i < 0 || i >= graph.size() || j < 0 || j >= graph.size())
		{
			return {};
		}

		// Queue for BFS
		queue<int> q;
		q.push(i);

		// Map to store the predecessor of each node in the path
		unordered_map<int, int> predecessor;
		predecessor[i] = -1;

		// BFS loop
		while (!q.empty())
		{
			int current = q.front();
			q.pop();

			// Check if we have reached node j
			if (current == j)
			{
				// Construct the path by following the predecessor chain
				vector<int> path;
				int curr = j;
				while (curr != -1)
				{
					path.push_back(curr);
					curr = predecessor[curr];
				}

				// Reverse the path and return it
				reverse(path.begin(), path.end());
				return path;
			}

			// Enqueue the neighbors of the current node
			for (int neighbor : graph[current])
			{
				if (neighbor != -1) {
					// Only enqueue the neighbor if it hasn't been visited before
					if (predecessor.count(neighbor) == 0)
					{
						q.push(neighbor);
						predecessor[neighbor] = current;
					}
				}
			}
		}

		// If we reach here, it means we have not found a path from i to j
		return {};
	}

	int n;
	int rows, cols, stepSize;
	vector<vector<int>> graph;
	vector<vector<float>> weights;
	vector<Triangle *> triangles;
};

