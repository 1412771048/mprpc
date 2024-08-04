#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<int>> tree;
vector<int> values;
vector<int> dp_increase;
vector<int> dp_decrease;
vector<int> parent;
vector<bool> visited;

void dfs(int node, int par) {
    visited[node] = true;
    dp_increase[node] = 1;  // Initialize chain length as 1 (the node itself)
    dp_decrease[node] = 1;
    parent[node] = par;

    cout << "Visiting node " << node << " with value " << values[node] << endl;

    for (int neighbor : tree[node]) {
        if (neighbor == par) continue;  // Skip the parent node

        cout << "Visiting neighbor " << neighbor << " of node " << node << endl;
        dfs(neighbor, node);

        // Update increase chain length based on parent-child relationship
        if (values[neighbor] >= values[node]) {
            dp_increase[node] = max(dp_increase[node], dp_increase[neighbor] + 1);
            cout << "Node " << node << ": Update increase chain length from neighbor " << neighbor << " to " << dp_increase[node] << endl;
        }

        // Update decrease chain length based on parent-child relationship
        if (values[neighbor] <= values[node]) {
            dp_decrease[node] = max(dp_decrease[node], dp_decrease[neighbor] + 1);
            cout << "Node " << node << ": Update decrease chain length from neighbor " << neighbor << " to " << dp_decrease[node] << endl;
        }
    }

    cout << "Node " << node << ": final increase chain length = " << dp_increase[node] << endl;
    cout << "Node " << node << ": final decrease chain length = " << dp_decrease[node] << endl;
}

int main() {
    int n = 5; // Number of nodes
    vector<int> example_values = {1, 2, 3, 4, 5}; // Values of the nodes
    vector<pair<int, int>> example_edges = {
        {1, 2}, {2, 3}, {1, 4}, {4, 5}
    }; // Edges

    // Initialize vectors
    values.resize(n + 1);
    tree.resize(n + 1);
    dp_increase.resize(n + 1, 0);
    dp_decrease.resize(n + 1, 0);
    parent.resize(n + 1, -1);
    visited.resize(n + 1, false);

    // Set example values
    for (int i = 1; i <= n; ++i) {
        values[i] = example_values[i - 1];
    }

    // Set example edges
    for (const auto& edge : example_edges) {
        int u = edge.first;
        int v = edge.second;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    // Start DFS from the root node (1)
    dfs(1, -1);

    // Find the longest chain length
    int longest_chain = 0;
    for (int i = 1; i <= n; ++i) {
        longest_chain = max(longest_chain, max(dp_increase[i], dp_decrease[i]));
    }

    cout << "Longest chain length: " << longest_chain << endl;

    return 0;
}
