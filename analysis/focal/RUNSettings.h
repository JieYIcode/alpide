#ifndef RUNSETTINGS
#define RUNSETTINGS

#include <vector>
#include <map>
#include <string>
#include <cstring>

struct cmp_runsettings
{
   bool operator()(char const *a, char const *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};

struct RUNSettings{
    int run;
    int system_rate;
    int event_rate;
    int cluster_size;
    int n_events;
    int strobe_inactive;
    int grid_mask_constant;
    RUNSettings(int _run, int _system_rate, int _event_rate, int _cluster_size, int _n_events, int _strobe_inactive, int _grid_mask_constant) : 
        run(_run), system_rate(_system_rate), 
        event_rate(_event_rate), 
        cluster_size(_cluster_size), 
        n_events(_n_events), 
        strobe_inactive(_strobe_inactive), 
        grid_mask_constant(_grid_mask_constant){};
    RUNSettings(int _run, int _system_rate, int _event_rate, int _cluster_size, int _n_events, int _strobe_inactive) : 
        run(_run), 
        system_rate(_system_rate), 
        event_rate(_event_rate), 
        cluster_size(_cluster_size), 
        n_events(_n_events), 
        strobe_inactive(_strobe_inactive), 
        grid_mask_constant(0){};
};


std::map<std::string , std::vector<RUNSettings>> runsettings = {

 
    { std::string("fixed_20230421"),
        {

            RUNSettings(0,10000,1000,4,45000,100),
            RUNSettings(1,20000,1000,4,45000,100),
            RUNSettings(2,10000,1000,3,45000,100),
            RUNSettings(3,20000,1000,3,45000,100),
            
            RUNSettings(4,10000,1000,2,45000,100),
            RUNSettings(5,20000,1000,2,45000,100),
            RUNSettings(6,10000,1000,6,45000,100),
            RUNSettings(7,20000,1000,6,45000,100),

            RUNSettings(8,5000,1000,4,45000,100),
            RUNSettings(9,50000,1000,4,45000,100),
            RUNSettings(10,5000,1000,3,45000,100),
            RUNSettings(11,50000,1000,3,45000,100),

            RUNSettings(12,10000,2000,4,45000,100),
            RUNSettings(13,20000,2000,4,45000,100),
            RUNSettings(14,10000,2000,3,45000,100),
            RUNSettings(15,20000,2000,3,45000,100),
            RUNSettings(16,10000,2000,6,45000,100),
            RUNSettings(17,20000,2000,6,45000,100),

            RUNSettings(18,10000,1000,4,45000,1000),
            RUNSettings(19,20000,1000,4,45000,1000),
            RUNSettings(20,10000,1000,3,45000,1000),
            RUNSettings(21,20000,1000,3,45000,1000),
            RUNSettings(22,10000,1000,6,45000,1000),
            RUNSettings(23,20000,1000,6,45000,1000),

            RUNSettings(24,10000,1000,4,45000,2000),
            RUNSettings(25,20000,1000,4,45000,2000),
            RUNSettings(26,10000,1000,3,45000,2000),
            RUNSettings(27,20000,1000,3,45000,2000),
            RUNSettings(28,10000,1000,6,45000,2000),
            RUNSettings(29,20000,1000,6,45000,2000),

            RUNSettings(30,10000,2000,4,45000,2000),
            RUNSettings(31,20000,2000,4,45000,2000),
            RUNSettings(32,10000,2000,3,45000,2000),
            RUNSettings(33,20000,2000,3,45000,2000),
            RUNSettings(34,10000,2000,6,45000,2000),
            RUNSettings(35,20000,2000,6,45000,2000)
        }
    },

    { std::string("masks_20230426"),
        {
            RUNSettings(0,10000,1000,4,45000,100,2),
            RUNSettings(1,20000,1000,4,45000,100,2),
            RUNSettings(2,10000,1000,3,45000,100,2),
            RUNSettings(3,20000,1000,3,45000,100,2),
            RUNSettings(4,10000,1000,6,45000,100,2),
            RUNSettings(5,20000,1000,6,45000,100,2),

            RUNSettings(6,10000,1000,4,45000,100,3),
            RUNSettings(7,20000,1000,4,45000,100,3),
            RUNSettings(8,10000,1000,3,45000,100,3),
            RUNSettings(9,20000,1000,3,45000,100,3),
            RUNSettings(10,10000,1000,6,45000,100,3),
            RUNSettings(11,20000,1000,6,45000,100,3),

            RUNSettings(12,10000,1000,4,45000,100,4),
            RUNSettings(13,20000,1000,4,45000,100,4),
            RUNSettings(14,10000,1000,3,45000,100,4),
            RUNSettings(15,20000,1000,3,45000,100,4),
            RUNSettings(16,10000,1000,6,45000,100,4),
            RUNSettings(17,20000,1000,6,45000,100,4)
        }
    },

    { std::string("pPb_20230427"),
        {
            RUNSettings(0,10000,1000,4,38000,100,0),
            RUNSettings(1,20000,1000,4,38000,100,0),
            RUNSettings(2,10000,1000,3,38000,100,0),
            RUNSettings(3,20000,1000,3,38000,100,0),
            RUNSettings(4,10000,1000,6,38000,100,0),
            RUNSettings(5,20000,1000,6,38000,100,0),

            RUNSettings(6,10000,2000,4,38000,100,0),
            RUNSettings(7,20000,2000,4,38000,100,0),
            RUNSettings(8,10000,2000,3,38000,100,0),
            RUNSettings(9,20000,2000,3,38000,100,0),
            RUNSettings(10,10000,2000,6,38000,100,0),
            RUNSettings(11,20000,2000,6,38000,100,0),


        }
    }

};

#endif