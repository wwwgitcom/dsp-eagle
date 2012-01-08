#include "dsp_plot.h"

namespace OpenDSP
{
    void dsp_plot::data(char *  cmd, ...)
    {
        va_list ap ;

        va_start(ap, cmd);
        vsprintf(local_data, cmd, ap);
        va_end(ap);

        strcat(local_data, "\n");

        fputs(local_data, pipe);
    }

    void dsp_plot::command(char *  cmd, ...)
    {
        va_list ap;
        va_start(ap, cmd);
        vsprintf(local_cmd, cmd, ap);
        va_end(ap);
        strcat(local_cmd, "\n");

        fprintf(pipe,local_cmd);
        fflush(pipe);
    }

    void dsp_plot::begin_plot()
    {
        fprintf(pipe,"plot '-'\n");
    }

    void dsp_plot::begin_plot(char *  cmd, ...)
    {
        va_list ap;
        memset(local_cmd, 0, GNPL_COMM_SIZE);
        va_start(ap, cmd);
        vsprintf(local_cmd, cmd, ap);
        va_end(ap);

        strcat(local_cmd, "\n");
        fprintf(pipe,"plot '-' %s\n", local_cmd);
    }

    void dsp_plot::end_plot()
    {
        fprintf(pipe,"e\n");
        fflush(pipe);
    }

    dsp_plot::dsp_plot()
    {
        //pipe = _popen("C:\\Tools\\gp442win32\\gnuplot\\binary\\gnuplot.exe","w");
        pipe = _popen("C:\\DiskD\\TooBox\\gnuplot\\binary\\gnuplot.exe","w");

        setvbuf(pipe, NULL, _IONBF, 0 );
    }

    dsp_plot::~dsp_plot()
    {
        _pclose(pipe);
    }
}