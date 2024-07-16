#include "../bin/httpserver.hpp"

using std::cout;
using std::endl;
using std::shared_ptr;

void Usage(std::string s)
{
  cout << "Usage:\n\t" << s << " port" << endl;
}

int main(int argc, char ** argv)
{
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(4);
  }

  shared_ptr<httpserver> httpserv(new httpserver(atoi(argv[1])));
  httpserv->activate();

}