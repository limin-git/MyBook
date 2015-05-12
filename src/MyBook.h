#pragma once
#include "CommonDefs.h"


class MyBook
{
public:

    MyBook( const path& p );

    bool create_path( const path& current, const std::string& add );

public:

    static bool left_is_parent_of_right( const path& left, const path& right );

public:

    path m_path;
    std::map<std::string, path> m_path_map;
};
