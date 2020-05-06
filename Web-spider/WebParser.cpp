#include <atomic>
#include <regex>
#include <thread>
#include <algorithm>
#include <windows.h>
#include "WebParser.h"

void WebParser::initWebParser()
{
  try
  {
    createURLsGraph();
    pageGraph.findStr(searchStr);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what();
  }
}

std::map<std::string, std::string> WebParser::loadPagePool(std::vector<std::string> urls)
{
  auto counter = urls.size();
  std::map<std::string, std::string> pages;

  if (counter > numOfThreads)
  {
    std::vector<std::thread> threads;
    auto steps = counter / numOfThreads;
    auto lastStepSize{ 0 };

    if (counter % numOfThreads != 0)
    {
      lastStepSize = counter - numOfThreads * steps;
      ++steps;
    }
    auto offset{ 0 };
    std::atomic<int> index = 0;
    for (unsigned i = 0; i < steps; i++)
    {
      threads.clear();
      auto jobs = numOfThreads;
      if (i == steps - 1 && lastStepSize)
        jobs = lastStepSize;
      for (unsigned j = 0; j < jobs; j++)
      {
        std::lock_guard<std::mutex> guard(pages_mutex);
        threads.push_back(std::thread([&] {
          auto url = urls.at(index++);
          pages.insert(std::make_pair(url, loadPage(url)));
          }));
      }
      for (auto& thread : threads)
      {
        thread.join();
      }
      offset += numOfThreads;
    }
  }
  else
  {
    for (auto& url : urls)
    {
      pages.insert(std::make_pair(url, loadPage(url)));
    }
  }
  return pages;
}

std::string WebParser::loadPage(std::string url) const
{
  CURL *curl_handle;
  CURLcode res;

  struct MemoryStruct chunk;

  chunk.memory = (char*)malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  res = curl_easy_perform(curl_handle);
  std::string page(chunk.memory, chunk.size);

  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();

  return page;
}

std::vector<std::string> WebParser::findURLs(std::string page) const
{
  const std::regex r_url(R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))");
  std::vector<std::string> urls;
  for (std::sregex_iterator it = std::sregex_iterator(page.begin(), page.end(), r_url);
    it != std::sregex_iterator(); it++)
  {
    std::smatch match;
    match = *it;
    urls.push_back(match.str(0));
  }

  return urls;
}

void WebParser::createURLsGraph()
{
  std::set<std::string> existsUrls;
  std::multimap<unsigned, std::string> URLsTree;

  unsigned nodeId{ 0 };
  unsigned parentNodeId{ 0 };
  unsigned index{ 0 };

  URLsTree.insert(std::make_pair(0, startURL));
  pageGraph.nodes.push_back(Node(nodeId, startURL, loadPage(startURL)));
  if (pageGraph.nodes.at(0).page.empty())
    std::cout << "Empty page!" << std::endl;
  for (auto it1 = URLsTree.lower_bound(index), it2 = URLsTree.upper_bound(index);
    it1 != it2 && index != searchDepth;
    ++it1, ++parentNodeId)
  {
    unsigned line = URLsTree.count(index);

    while (line != 0)
    {
      std::vector<std::string> urls = findURLs(loadPage(it1->second));
      removeExists(urls, existsUrls);
      std::copy(urls.begin(), urls.end(), std::inserter(existsUrls, existsUrls.end()));
      if (urls.size() != 0)
      {
        std::map<std::string, std::string> pages = loadPagePool(urls);
        for (auto& url : urls)
        {
          if (pages.count(url))
          {
            ++nodeId;
            URLsTree.insert(std::make_pair(index + 1, url));
            pageGraph.nodes.push_back(Node(nodeId, url, pages.at(url)));
            pageGraph.edges.push_back({ parentNodeId, nodeId });
            std::cout << "UPLOADED: " << url << std::endl;
          }
        }
        --line;
        if (line != 0)
        {
          ++it1;
          ++parentNodeId;
        }
        urls.clear();
      }
      else
      {
        --line;
        if (line != 0)
        {
          ++it1;
          ++parentNodeId;
        }
      }
    }
    ++index;
  }
  pageGraph.makeAdjList();
}

void WebParser::removeExists(std::vector<std::string>& urls, std::set<std::string>& existsUrls)
{
  if (urls.size())
  {
    std::sort(urls.begin(), urls.end());
    urls.erase(std::unique(urls.begin(), urls.end()), urls.end());
    for (auto& u : urls)
    {
      if (existsUrls.find(u) != existsUrls.end())
        urls.erase(std::remove(urls.begin(), urls.end(), u), urls.end());
    }
  }
}

size_t WebParser::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == nullptr) {
    throw std::exception("out of memory!");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void WebParser::printGraph()
{
  WebGraph::printGraph(pageGraph, pageGraph.nodes.size());
}