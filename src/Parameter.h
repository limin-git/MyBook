#pragma once


struct Parameter
{
    Parameter( const std::string& base, const std::string& proj, bool is_remove )
        : m_base( base ),
          m_proj( proj ),
          m_is_remove( is_remove )
    {
    }

    std::string m_base;
    std::string m_proj;

    // TODO: get them from enviroument
    std::string m_ACE_ver;
    std::string m_boost_ver;
    std::string m_mcl_ver;
    std::string m_MySQL_ver;
    std::string m_oracle_ver;

    bool m_is_remove;
};

typedef boost::shared_ptr<Parameter> ParameterPtr;
