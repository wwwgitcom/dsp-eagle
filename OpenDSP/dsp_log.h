#pragma once
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
namespace OpenDSP
{
    class dsp_log
    {
    private:
        FILE* m_handle;
        std::vector<std::string> m_include_filters;
        std::vector<std::string> m_exclude_filters;
        dsp_log();
        ~dsp_log();
    public:
        static dsp_log& Instance();
        void add_include_filter(std::string keyword);
        void add_include_filter(const char * keyword);
        void add_exclude_filter(std::string keyword);
        void add_exclude_filter(const char * keyword);
        void clear_filters();
        void operator()(const char * format, ... );
    };
#if 1
#define log(...)
#else
    extern dsp_log& log;
#endif
}