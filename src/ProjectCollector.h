#pragma once
#include "CommonDefs.h"


class ProjectCollector
{
public:

    ProjectCollector( const path& p, const std::string& configuration_name = "Debug" );

    static void collect_in_thread( std::set<path>& project_includes, const path& project_path );

private:

    void extract_files();
    void extract_additional_include_directories();

public:

    path m_path;
    std::string m_str;
    path m_current_path;
    std::vector<path> m_additional_include_directories;
    std::vector<path> m_files;
    std::string m_configuration_name;
};
