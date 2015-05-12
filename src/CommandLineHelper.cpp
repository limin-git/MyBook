#include "StdAfx.h"
#include "CommandLineHelper.h"


CommandLineHelper::CommandLineHelper( int argc, char* argv[] )
{
    std::string base = "TA_BASE";
    std::string proj = "3001";
    bool remove = false;

    if ( argc == 1 )
    {
        // DO NOTHING
    }
    else if ( argc == 2 )
    {
        remove = is_remove( argv[1] );

        if ( false == remove )
        {
            display_usage();
            return;
        }
    }
    else if ( argc == 3 )
    {
        base = argv[1];
        proj = argv[2];
    }
    else if ( argc == 4 )
    {
        base = argv[1];
        proj = argv[2];
        remove = is_remove( argv[3] );

        if ( false == remove )
        {
            display_usage();
            return;
        }
    }

    bool should_make = true;

    if ( ! boost::filesystem::exists( base ) )
    {
        std::cout << "invalid path: " << base << "\n";
        should_make = false;
    }

    if ( ! boost::filesystem::exists( proj ) )
    {
        std::cout << "invalid path: " << proj << "\n";
        should_make = false;
    }

    {
        boost::filesystem::path p1 = boost::filesystem::path(base).parent_path();
        boost::filesystem::path p2 = boost::filesystem::path(proj).parent_path();

        if ( p1 != p2 )
        {
            std::cout << "path mismatch" << std::endl;
            should_make = false;
        }
    }

    if ( false == should_make )
    {
        display_usage();
        return;
    }

    m_parameter = ParameterPtr( new Parameter( base, proj, remove ) );
}


bool CommandLineHelper::is_remove( const char* str )
{
    std::string s = str;
    boost::to_lower(s);
    return ( "clean" == s || "clear" == s || "remove" == s || "delete" == s || "del" == s );
}


void CommandLineHelper::display_usage()
{
    std::cout << "Usage:\n";
    std::cout << "\tMakeSDK clean\n";
    std::cout << "\tMakeSDK TA_BASE 3001\n";
    std::cout << "\tMakeSDK TA_BASE 3001 clean\n";
    std::cout << std::flush;
}
