#include "WebGraph.h"
#include "WebParser.h"

void start();

int main()
{
  try
  {
    start();
  }
  catch (const std::exception & ex)
  {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}

void start()
{
  std::string url{};
  std::string phrase{};
  unsigned depth{ 0 };
  unsigned threads{ 0 };

  std::cout << "Enter the starting address or 'e' for select one of the proposed: ";
  std::cin >> url;
  if (url == "e")
  {
    char choice;
    std::cout << "Chose one address to search: " << std::endl;
    std::cout << "1 - https://example.com" << std::endl
      << "2 - https://www.google.com" << std::endl
      << "3 - https://en.wikipedia.org" << std::endl
      << "4 - https://en.cppreference.com" << std::endl
      << "5 - https://stackoverflow.com" << std::endl;
    std::cin >> choice;
    switch (choice)
    {
    case '1':
      url = "https://example.com";
      break;
    case '2':
      url = "https://www.google.com";
      break;
    case '3':
      url = "https://en.wikipedia.org/wiki/Main_Page";
      break;
    case '4':
      url = "https://en.cppreference.com";
      break;
    case '5':
      url = "https://stackoverflow.com";
      break;
    default:
      break;
    }
  }
  std::cin.ignore();
  std::cout << std::endl;
  std::cout << "Enter a search phrase: ";
  std::getline(std::cin, phrase);
  std::cout << std::endl;
  std::cout << "Enter search depth: ";
  std::cin >> depth;
  std::cout << std::endl;
  std::cout << "Enter the number of download threads: ";
  std::cin >> threads;
  std::cout << std::endl;

  WebParser p(depth, threads, phrase, url);

  char choice;

  std::cout << std::endl;
  std::cout << "Print pages graph? (y/n): ";
  std::cin >> choice;
  std::cout << std::endl;
  switch (choice)
  {
  case 'y':
    p.printGraph();
    break;
  case 'n':
    break;
  default:
    break;
  }

  std::cout << "Do a new search? (y/n): ";
  std::cin >> choice;

  switch (choice)
  {
  case 'y':
    start();
    break;
  case 'n':
    break;
  default:
    break;
  }
}