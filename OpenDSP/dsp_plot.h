#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

namespace OpenDSP
{
#define GNPL_COMM_SIZE 2000
#define tmp_name "c:\\tmpdataXXXXXXX"

    class dsp_plot
    {
    public:
        dsp_plot();
        ~dsp_plot();
        FILE *pipe;
        FILE *input;
        char  gnuplot_fifo[(sizeof tmp_name)];
        char  local_cmd[GNPL_COMM_SIZE];
        char  local_data[GNPL_COMM_SIZE];

        void begin_plot();
        void begin_plot(char *  cmd, ...);
        void end_plot();
        void data(char *  cmd, ...);
        void command(char *  cmd, ...);
    };

    

    inline void gnuplot_command(dsp_plot &Window, char inpar[])
    {
        fprintf(Window.pipe,"%s\n",inpar);
    }


    inline void open_3d_gnuplot(dsp_plot &Window,char inpar[])
    { 
        fprintf(Window.pipe,"splot '%s' %s",Window.gnuplot_fifo,inpar);
        fprintf(Window.pipe,"\n");
        fflush(Window.pipe);
    }	

}