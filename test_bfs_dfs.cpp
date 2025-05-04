#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
using namespace chrono;

class Graph {
    int V;
    vector<vector<int>> adjMatrix;

public:
    Graph(const vector<vector<int>> &matrix) {
        V = matrix.size();
        adjMatrix = matrix;
    }

    void sequentialBFS(int source) {
        vector<bool> visited(V, false);
        queue<int> q;
        visited[source] = true;
        q.push(source);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            cout << u << " ";

            for (int v = 0; v < V; ++v) {
                if (adjMatrix[u][v] && !visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
    }

    void sequentialDFSUtil(int v, vector<bool>& visited) {
        visited[v] = true;
        cout << v << " ";
        for (int u = 0; u < V; ++u) {
            if (adjMatrix[v][u] && !visited[u]) {
                sequentialDFSUtil(u, visited);
            }
        }
    }

    void sequentialDFS(int source) {
        vector<bool> visited(V, false);
        sequentialDFSUtil(source, visited);
    }

    void parallelBFS(int source) {
        vector<bool> visited(V, false);
        queue<int> q;
        visited[source] = true;
        q.push(source);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            cout << u << " ";

            #pragma omp parallel for
            for (int v = 0; v < V; ++v) {
                if (adjMatrix[u][v]) {
                    #pragma omp critical
                    {
                        if (!visited[v]) {
                            visited[v] = true;
                            q.push(v);
                        }
                    }
                }
            }
        }
    }

    
    void parallelDFSUtil(int v, vector<bool>& visited) {
        bool alreadyVisited;
    
        // Check and update visited flag in critical section
        #pragma omp critical
        {
            alreadyVisited = visited[v];
            if (!visited[v]) {
                visited[v] = true;
                cout << v << " ";
            }
        }
    
        // If already visited, skip further recursion
        if (alreadyVisited) return;
    
        for (int u = 0; u < visited.size(); ++u) {
            if (adjMatrix[v][u]) {
                #pragma omp task
                parallelDFSUtil(u, visited);
            }
        }
    }
    
    
    void parallelDFS(int source) {
        vector<bool> visited(V, false);
        #pragma omp parallel
        {
            #pragma omp single
            {
                parallelDFSUtil(source, visited);
            }
        }
    }
    
};

int main() {
    // Static graph with 6 vertices
    vector<vector<int>> matrix = {
        {0, 1, 1, 0, 0, 0},  // Node 0 connected to 1, 2
        {1, 0, 0, 1, 1, 0},  // Node 1 connected to 0, 3, 4
        {1, 0, 0, 0, 0, 1},  // Node 2 connected to 0, 5
        {0, 1, 0, 0, 0, 0},  // Node 3 connected to 1
        {0, 1, 0, 0, 0, 0},  // Node 4 connected to 1
        {0, 0, 1, 0, 0, 0}   // Node 5 connected to 2
    };

    Graph g(matrix);
    int source = 0;

    // Sequential BFS
    auto start = high_resolution_clock::now();
    cout << "Sequential BFS: ";
    g.sequentialBFS(source);
    cout << endl;
    auto stop = high_resolution_clock::now();
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " µs\n";

    // Sequential DFS
    start = high_resolution_clock::now();
    cout << "Sequential DFS: ";
    g.sequentialDFS(source);
    cout << endl;
    stop = high_resolution_clock::now();
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " micro sec\n";

    // Parallel BFS
    start = high_resolution_clock::now();
    cout << "Parallel BFS: ";
    g.parallelBFS(source);
    cout << endl;
    stop = high_resolution_clock::now();
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " micro sec\n";

    // Parallel DFS
    start = high_resolution_clock::now();
    cout << "Parallel DFS: ";
    g.parallelDFS(source);
    cout << endl;
    stop = high_resolution_clock::now();
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " µs\n";

    return 0;
}

/*
Compile:
g++ -fopenmp -O2 bfs_dfs_static.cpp -o bfs_dfs_static
./bfs_dfs_static
*/
