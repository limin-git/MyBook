#pragma once
#include "CommonDefs.h"


class MyBook
{
public:

    MyBook( const path& p );

    bool create_sub_directories( const std::string& folder_name );

    void add_book( const path& book_path, const std::string& folder_name );

public:

    static bool is_parent_sub_folder( const path& left, const path& right );

public:

    path m_path;
    std::map<std::string, path> m_path_map;
    size_t m_file_count;
    size_t m_folder_count;
    boost::regex m_regex;
};
