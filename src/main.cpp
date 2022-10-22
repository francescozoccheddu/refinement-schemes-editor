#include <RSE/App.hpp>

#include <iostream>

int main(int _argc, char** _argv)
{
    RSE::App app{};
    if (_argc == 2)
    {
        app.open(_argv[1]);
    }
    else if (_argc > 2)
    {
        std::cerr << "0 or 1 arguments expected" << std::endl;
        return 1;
    }
    return app.launch();
}
