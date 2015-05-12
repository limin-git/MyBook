#pragma once
#include "CommonDefs.h"


struct FileCollector
{
public:

    FileCollector( const path& p, const path& current_path, const std::vector<path>& additional_directories );

    static boost::shared_ptr<boost::thread> create_FileCollectorThread( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional );

private:

    void simple_collect();
    void recursive_collect();
    path search_path( const path& include, const path& parent );
    const std::vector<path>& collect_from_file( const path& file_path ); 

public:

    path m_path;
    path m_current_path;
    std::vector<path> m_additional_directories;
    std::queue<path> m_queue;
    std::set<path> m_includes;
};
