#pragma once


using namespace boost::filesystem;


struct Utility
{
    static std::string get_string_from_file( const path& p, size_t lines = 0 );
    static std::ostream& output_paths( std::ostream& os, const std::set<path>& paths );
};
