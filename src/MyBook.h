#pragma once
#include "CommonDefs.h"


class MyBook
{
public:

    MyBook( const path& p );

    bool create_folder( const std::string& folder_name );

    void add_book( const path& book_path, const std::string& folder_name );
    void add_folder( const path& folder );

public:

    bool is_parent_sub_path( const path& parent, const path& sub );
    bool is_folder_name( const std::string& folder_name );
    bool is_valid_path( const path& p );
    bool is_book_exist( const path& file_path, bool is_output = false );
    void initialize( const std::string& folder_name );

public:

    path m_path;
    std::map<std::string, path> m_class_path_map;
    std::map<std::string, path> m_file_path_map;
    size_t m_file_count;
    size_t m_folder_count;
    boost::regex m_regex;

    bool m_path_valid;
    boost::filesystem::recursive_directory_iterator m_end;
    boost::filesystem::recursive_directory_iterator m_it;
    char m_top;
};
