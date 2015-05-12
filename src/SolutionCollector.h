#pragma once
#include "CommonDefs.h"


class SolutionCollector
{
public:

    SolutionCollector( const path& p );

    static void collect_in_thread( std::set<path>& solution_includes, const path& p );

private:

    void extract_project_paths();

public:

    path m_path;
    path m_current_path;
    std::string m_str;
    std::vector<path> m_projects;
};
