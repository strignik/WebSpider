#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <string>

using AdjacencyList = std::vector<std::vector<int>>;

struct Node
{
  Node(unsigned id, std::string u, std::string p) : id(id), url(u) ,page(p) {};
  unsigned id;
  std::string url;
  std::string page;

  bool operator==(Node& rhs) {
    return this->id == rhs.id && this->url == rhs.url;
  };
};

struct Edge
{
  unsigned src, dst;
};

class WebGraph
{
public:
  WebGraph() {};
  void makeAdjList();
  void findStr(std::string str);
  static void printGraph(WebGraph const& graph, int N);

  std::vector<Node> nodes;
  std::vector<Edge> edges;
private:
  unsigned numOfNodes;
  AdjacencyList adjList;

};

