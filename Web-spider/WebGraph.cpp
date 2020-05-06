#include "WebGraph.h"

void WebGraph::makeAdjList()
{
  adjList.resize(nodes.size());
  for (auto &edge : edges)
  {
    adjList[edge.src].push_back(edge.dst);
  }
}

void WebGraph::findStr(std::string str)
{
  std::vector<std::string> found;
  std::vector<std::string> notFound;
  std::vector<bool> mark(adjList.size(), false);
  std::queue<unsigned> q;

  q.push(0);
  mark.at(0) = true;

  while (!q.empty())
  {
    auto f = q.front();
    q.pop();

    auto node = nodes.at(f);
    if (node.page.find(str) != std::string::npos)
    {
      found.push_back(node.url);
    }
    else
    {
      notFound.push_back(node.url);
    }

    for (auto it = adjList.at(f).begin(); it != adjList.at(f).end(); ++it) {
      if (!mark.at(*it)) {
        q.push(*it);
        mark.at(*it) = true;
      }
    }
  }
  std::cout << "Pages containing string " << "\"" << str << "\":" << std::endl << std::endl;
  for (auto& n : found)
    std::cout << n << std::endl;

  std::cout << std::endl;

  std::cout << "Pages not containing string " << "\"" << str << "\":" << std::endl << std::endl;
  for (auto& n : notFound)
    std::cout << n << std::endl;
}

void WebGraph::printGraph(WebGraph const & graph, int N)
{
  for (auto i = 0; i < N; i++)
  {
    // print current vertex number
    std::cout << i << " --> ";

    // print all neighboring vertices of vertex i
    for (auto v : graph.adjList[i])
      std::cout << v << " ";
    std::cout << std::endl;
  }
}
