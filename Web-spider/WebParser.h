#pragma once
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <mutex>
#include "curl.h"
#include "WebGraph.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Normaliz.lib")

struct MemoryStruct {
  char *memory;
  size_t size;
};

class WebParser
{
public:
  WebParser(unsigned depth, unsigned threads, std::string str, std::string url)
    : searchDepth(depth), numOfThreads(threads), searchStr(str), startURL(url)
  {
    initWebParser();
  };
  void printGraph();
private:
  WebGraph pageGraph;
  std::mutex pages_mutex;
  unsigned searchDepth{ 0 };
  unsigned numOfThreads{ 0 };
  std::string startURL{ 0 };
  std::string searchStr{ 0 };

  void initWebParser();
  void createURLsGraph();
  void removeExists(std::vector<std::string>& urls, std::set<std::string>& existsUrls);
  std::map<std::string, std::string> loadPagePool(std::vector<std::string> urls);
  std::string loadPage(std::string url) const;
  std::vector<std::string> findURLs(std::string page) const;

  static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void* stream);
};