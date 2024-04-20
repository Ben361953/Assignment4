#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

struct Edge {
    int x;
    int y;
    int value;

    Edge(int x, int y, int value) {
        this->x = x;
        this->y = y;
        this->value = value;
    }
};

bool compare1(const Edge &edge1, const Edge &edge2) {
    return edge1.value < edge2.value;
}

bool compare2(const Edge &edge1, const Edge &edge2) {
    return edge1.value > edge2.value;
}

vector<vector<char>> parse_matrix(const string &str) {
    vector<vector<char>> matrix;
    vector<char> row;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ',') {
            matrix.push_back(row);
            row.clear();
        } else {
            row.push_back(str[i]);
        }
    }
    matrix.push_back(row);
    return matrix;
}

vector<vector<int>> parse_country(const string &str) {
    vector<vector<char>> matrix = parse_matrix(str);
    int n = matrix.size();
    int m = matrix[0].size();
    vector<vector<int>> country = vector(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            country[i][j] = matrix[i][j] - '0';
        }
    }
    return country;
}

vector<vector<int>> parse_values(const string &str) {
    vector<vector<char>> matrix = parse_matrix(str);
    int n = matrix.size();
    int m = matrix[0].size();
    vector<vector<int>> country = vector(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] >= 'A' && matrix[i][j] <= 'Z') {
                country[i][j] = matrix[i][j] - 'A';
            } else {
                country[i][j] = matrix[i][j] - 'a' + 26;
            }
        }
    }
    return country;
}

void find_component(int cur, const vector<vector<int>> &country, vector<int> &vis, vector<int> &component) {
    if (vis[cur]) {
        return;
    }
    vis[cur] = 1;
    component.push_back(cur);
    int n = country.size();
    for (int nxt = 0; nxt < n; nxt++) {
        if (!country[cur][nxt]) {
            continue;
        }
        find_component(nxt, country, vis, component);
    }
}

int ft[1005], num[1005], rk[1005];

void init(int n) {
    for (int i = 1; i <= n; i++) {
        ft[i] = i;
        num[i] = rk[i] = 1;
    }
}

int find_set(int x) {
    int r = x;
    while (r != ft[r]) r = ft[r];
    while (x != ft[x]) {
        int q = ft[x];
        ft[x] = r;
        x = q;
    }
    return r;
}

bool union_set(int x, int y) {
    int sx = find_set(x), sy = find_set(y);
    if (sx == sy) return false;
    if (rk[sx] < rk[sy]) {
        ft[sx] = sy;
        num[sy] += num[sx];
    } else {
        ft[sy] = sx;
        num[sx] += num[sy];
        if (rk[sx] == rk[sy]) rk[sx]++;
    }
    return true;
}

int main() {
    string str;
    cin >> str;
    vector<vector<int>> country = parse_country(str);
    cin >> str;
    vector<vector<int>> build = parse_values(str);
    cin >> str;
    vector<vector<int>> destroy = parse_values(str);
    // Get the number of nodes
    int n = country.size();
    // Get connected components
    vector<vector<int>> components;
    vector<int> vis = vector<int>(n);
    for (int i = 0; i < n; i++) {
        if (vis[i]) {
            continue;
        }
        vector<int> component;
        find_component(i, country, vis, component);
        components.push_back(component);
    }
    // Find the maximum spanning tree of the country
    int destroy_value = 0;
    vector<Edge> destroy_edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (country[i][j]) {
                destroy_value += destroy[i][j];
                destroy_edges.emplace_back(i, j, destroy[i][j]);
            }
        }
    }
    init(n);
    sort(destroy_edges.begin(), destroy_edges.end(), compare2);
    for (int i = 0; i < destroy_edges.size(); i++) {
        Edge edge = destroy_edges[i];
        if (union_set(edge.x, edge.y)) {
            destroy_value -= edge.value;
        }
    }
    // Find the maximum spanning tree of the components
    vector<Edge> build_edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (!country[i][j]) {
                build_edges.emplace_back(i, j, destroy[i][j]);
            }
        }
    }
    sort(build_edges.begin(), build_edges.end(), compare1);
    // First, connect the nodes in a component
    init(n);
    for (int i = 0; i < components.size(); i++) {
        for (int j = 1; j < components[i].size(); j++) {
            union_set(components[i][j - 1], components[i][j]);
        }
    }
    // Next, connect the nodes in different components
    int build_value = 0;
    for (int i = 0; i < build_edges.size(); i++) {
        Edge edge = build_edges[i];
        if (union_set(edge.x, edge.y)) {
            build_value += edge.value;
        }
    }
    // Output the cost for building roads and destroy roads
    cout << destroy_value + build_value << endl;
    return 0;
}
