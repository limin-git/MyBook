#pragma once
#include "CommonDefs.h"


class Library
{
public:

    Library( const path& p );
    void add_book( const path& book_path, const std::string& folder_name );
    void add_directory( const path& folder );

public:

    bool creaate_clc_folder( const std::string& folder_name );
    void create_index( const char top_class_name = 'Z' );
    void recreate_index();
    void remove_path( const path& book_path, bool is_output = false );
    void rename_remove_string( const std::string& s, const std::string& replace );
    void rename_regex( const std::string& s, const std::string& replace );
    void rename_impl( const std::vector<std::pair<std::map<std::string, path>::iterator, path> >& new_paths );
    bool is_parent_path( const path& parent, const path& p );
    bool is_clc_folder_name( const std::string& folder_name );
    bool is_clc_directory( const path& dir );
    bool is_valid_clc_directory( const path& p );
    bool is_book_exist( const path& file_path, bool is_output = false );
    bool is_equal_size( const path& lhs, const path& rhs );

public:

    path m_path;
    std::map<std::string, path> m_class_path_map;
    std::map<std::string, path> m_file_path_map;
    size_t m_file_count;
    size_t m_folder_count;
    boost::regex m_regex;

    bool m_path_exist;
    boost::filesystem::recursive_directory_iterator m_end;
    boost::filesystem::recursive_directory_iterator m_it;
    char m_current_top_class;
};
