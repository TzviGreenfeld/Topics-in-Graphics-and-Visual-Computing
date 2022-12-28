#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;

class Graph
{
public:
	Graph(int rows, int cols, int stepSize)
	{
		this->rows = int(rows / stepSize) * 2;
		this->cols = int(cols / stepSize);
		int n = this->rows * this->cols;
		printf("rows: %d, cols: %d, n: %d\n", this->rows, this->cols, n);

		// the graph is for triangles, we can init n nodes with 3 neighbors
		this->graph = vector<vector<int>>(n, vector<int>(3, -1));

		// init the graph
		for (int i = 0; i < n; i++)
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
		return row >= 0 && row < rows && col >= 0 && col < cols;
	}

	vector<int> getNeighbors(int i)
	{
		// return only the valid neighbors
		vector<int> neighbors;
		for (int neighbor : graph[i])
		{
			if (neighbor != -1)
			{
				neighbors.push_back(neighbor);
			}
		}
		return neighbors;
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

	int rows, cols, stepSize;
	vector<vector<int>> graph;
};