#include "StdAfx.h"
#include "SolutionCollector.h"
#include "ProjectCollector.h"
#include "Utility.h"


SolutionCollector::SolutionCollector( const path& p )
    : m_path( p )
{
    std::cout << m_path.string() << std::endl;
    m_current_path = m_path.parent_path();
    m_str = Utility::get_string_from_file( m_path );

    if ( false == m_str.empty() )
    {
        extract_project_paths();
    }
}


void SolutionCollector::extract_project_paths()
{
    // extract additional include directories
    static const boost::regex e
    (
        "(?x)"
        "^ Project"
        "\\( \" \\{ .+? \\} \" \\) \\s+ = \\s+"     // ("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = 
        "\" [^\"]+ \", \\s+ "                       // "core.alarm.idl", 
        " \" ( [^\"]+ ) \" "                        // $1: "..\..\transactive\core\alarm\IDL\core.alarm.idl.TA_AlarmAgentCorbaDef.vcproj"
        " .+? "
        "^ EndProject"
    );

    boost::sregex_iterator it( m_str.begin(), m_str.end(), e );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        path project_relative_path = it->str(1);
        path p = boost::filesystem::system_complete( m_current_path / project_relative_path );
        std::cout << p.string() << std::endl;
        m_projects.push_back( p );
    }
}


void SolutionCollector::collect_in_thread( std::set<path>& solution_includes, const path& p )
{
    SolutionCollector slution( p );
    std::vector<path>& projects = slution.m_projects;

    for ( size_t i = 0; i < projects.size(); ++i )
    {
        std::set<path> project_includes;
        ProjectCollector::collect_in_thread( project_includes, projects[i] );
        solution_includes.insert( project_includes.begin(), project_includes.end() );
    }
}
