#include "StdAfx.h"
#include "MyBook.h"


void main(int argc, char* argv[])
{
    //MyBook mybook(  "D:\\My Book" );
    MyBook mybook(  "C:\\Temp" );

    std::string book;
    std::string folder;

    std::cout << "添加新书：";

    while ( true )
    {
        std::cin >> book;
        std::cin >> folder;

        mybook.add_book( book, folder );

        std::cout << "添加新书：";
    }
}
