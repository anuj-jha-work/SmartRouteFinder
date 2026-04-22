#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "httplib.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

struct Edge {
    string from;
    string to;
    int cost;
};

void from_json(const json& j, Edge& e) {
    j.at("from").get_to(e.from);
    j.at("to").get_to(e.to);
    j.at("cost").get_to(e.cost);
}

struct RequestData {
    vector<string> nodes;
    vector<Edge> edges;
    string source;
    string destination;
    string algorithm;
};

void from_json(const json& j, RequestData& r) {
    j.at("nodes").get_to(r.nodes);
    j.at("edges").get_to(r.edges);
    j.at("source").get_to(r.source);
    j.at("destination").get_to(r.destination);
    j.at("algorithm").get_to(r.algorithm);
}

struct ResponseData {
    vector<string> path;
    int cost;
};

void to_json(json& j, const ResponseData& r) {
    j = json{{"path", r.path}, {"cost", r.cost}};
}

unordered_map<string, vector<Edge>> buildGraph(const vector<string>& nodes, const vector<Edge>& edges) {
    unordered_map<string, vector<Edge>> graph;
    for (const auto& node : nodes) {
        graph[node] = vector<Edge>();
    }
    for (const auto& e : edges) {
        graph[e.from].push_back(e);
        Edge reverse_edge{e.to, e.from, e.cost};
        graph[e.to].push_back(reverse_edge);
    }
    return graph;
}

int calculatePathCost(const vector<string>& path, const unordered_map<string, vector<Edge>>& graph) {
    int cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        if (graph.count(path[i]) > 0) {
            for (const auto& edge : graph.at(path[i])) {
                if (edge.to == path[i + 1]) {
                    cost += edge.cost;
                    break;
                }
            }
        }
    }
    return cost;
}

vector<string> buildPath(const string& start, const string& end, unordered_map<string, string>& parent) {
    vector<string> path;
    string at = end;
    while (!at.empty() && parent.count(at) > 0) {
        path.push_back(at);
        at = parent[at];
    }
    if (at == start) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    if (path.empty() || path[0] != start) {
        return vector<string>();
    }
    return path;
}

ResponseData dijkstra(const string& start, const string& end, const unordered_map<string, vector<Edge>>& graph) {
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    
    // priority queue stores pairs of {dist, node_name}
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

    for (const auto& pair : graph) {
        dist[pair.first] = INT_MAX;
    }
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto entry = pq.top();
        pq.pop();
        int current_dist = entry.first;
        string curr = entry.second;

        if (curr == end) break;
        if (current_dist > dist[curr]) continue;

        if (graph.count(curr) > 0) {
            for (const auto& edge : graph.at(curr)) {
                int newDist = dist[curr] + edge.cost;
                if (newDist < dist[edge.to]) {
                    dist[edge.to] = newDist;
                    prev[edge.to] = curr;
                    pq.push({newDist, edge.to});
                }
            }
        }
    }

    vector<string> path = buildPath(start, end, prev);
    int cost = path.empty() ? -1 : dist[end];
    return {path, cost};
}

bool dfsHelper(const string& current, const string& end, const unordered_map<string, vector<Edge>>& graph,
               unordered_set<string>& visited, unordered_map<string, string>& parent) {
    if (current == end) return true;
    visited.insert(current);
    if (graph.count(current) > 0) {
        for (const auto& edge : graph.at(current)) {
            if (visited.find(edge.to) == visited.end()) {
                parent[edge.to] = current;
                if (dfsHelper(edge.to, end, graph, visited, parent)) return true;
            }
        }
    }
    return false;
}

ResponseData dfs(const string& start, const string& end, const unordered_map<string, vector<Edge>>& graph) {
    unordered_set<string> visited;
    unordered_map<string, string> parent;
    bool found = dfsHelper(start, end, graph, visited, parent);
    vector<string> path = buildPath(start, end, parent);
    int cost = found ? calculatePathCost(path, graph) : -1;
    return {path, cost};
}

ResponseData bfs(const string& start, const string& end, const unordered_map<string, vector<Edge>>& graph) {
    queue<string> q;
    unordered_map<string, string> parent;
    unordered_set<string> visited;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        if (current == end) break;

        if (graph.count(current) > 0) {
            for (const auto& edge : graph.at(current)) {
                if (visited.find(edge.to) == visited.end()) {
                    visited.insert(edge.to);
                    parent[edge.to] = current;
                    q.push(edge.to);
                }
            }
        }
    }

    vector<string> path = buildPath(start, end, parent);
    int cost = path.empty() ? -1 : calculatePathCost(path, graph);
    return {path, cost};
}

int main() {
    httplib::Server svr;

    vector<string> public_dirs = {
        "./public",
        "public",
        "/opt/render/project/src/public"
    };

    string resolved_public_dir;
    for (const auto& dir : public_dirs) {
        if (filesystem::exists(dir) && filesystem::is_directory(dir)) {
            resolved_public_dir = dir;
            break;
        }
    }

    bool mounted = false;
    if (!resolved_public_dir.empty()) {
        mounted = svr.set_mount_point("/", resolved_public_dir.c_str());
        cout << "Static directory: " << resolved_public_dir << " (mounted=" << (mounted ? "true" : "false") << ")" << endl;
    } else {
        cerr << "Warning: public directory not found; static hosting may fail." << endl;
    }

    svr.Get("/", [resolved_public_dir](const httplib::Request&, httplib::Response& res) {
        if (resolved_public_dir.empty()) {
            res.status = 500;
            res.set_content("public/index.html not found", "text/plain");
            return;
        }

        string index_path = resolved_public_dir + "/index.html";
        ifstream file(index_path);
        if (!file) {
            res.status = 404;
            res.set_content("index.html not found", "text/plain");
            return;
        }

        stringstream buffer;
        buffer << file.rdbuf();
        res.set_content(buffer.str(), "text/html; charset=utf-8");
    });

    svr.Get("/healthz", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("ok", "text/plain");
    });

    svr.Post("/find-path", [](const httplib::Request& req, httplib::Response& res) {
        // Set CORS headers
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        try {
            auto j = json::parse(req.body);
            RequestData data = j.get<RequestData>();

            auto graph = buildGraph(data.nodes, data.edges);

            ResponseData result;
            string algo = data.algorithm;
            transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

            if (algo == "dfs") {
                result = dfs(data.source, data.destination, graph);
            } else if (algo == "bfs") {
                result = bfs(data.source, data.destination, graph);
            } else {
                result = dijkstra(data.source, data.destination, graph);
            }

            json res_json = result;
            res.set_content(res_json.dump(), "application/json");

        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            json error_json = {{"path", vector<string>()}, {"cost", -1}};
            res.status = 400;
            res.set_content(error_json.dump(), "application/json");
        }
    });

    // Handle OPTIONS requests for CORS
    svr.Options("/find-path", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });

    int port = 8081;
    const char* render_port = getenv("PORT");
    if (render_port != nullptr) {
        try {
            port = stoi(render_port);
        } catch (...) {
            // Fall back to local default if PORT is invalid.
        }
    }

    cout << "Starting Server on http://localhost:" << port << endl;
    svr.listen("0.0.0.0", port);

    return 0;
}
