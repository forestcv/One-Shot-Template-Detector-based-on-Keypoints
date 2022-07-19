#include "application.h"

int main()
{
    std::string pathToSettings = "../settings";
    Application app(pathToSettings);
    app.start();
    while(true)
    {

    }
    return 0;
}