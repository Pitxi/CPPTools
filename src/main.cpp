#include "htmltools.hpp"
#include <glibmm/error.h>
#include <iostream>

int main(int argc, char** argv)
{
    std::locale::global(std::locale(""));
    std::cout << Tools::html_entities_decode("Brøderbund&nbsp;Software,&nbsp;Inc.") << std::endl;

    return 0;
}
