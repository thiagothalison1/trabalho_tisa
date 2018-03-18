extern char* AIR_TEMPERATURE;
extern char* WATER_TEMPERATURE;
extern char* WATER_IN_TEMPERATURE;
extern char* WATER_OUT_FLOW;
extern char* WATER_LEVEL;
extern char* WATER_IN_FLOW;
extern char* HEAT_FLOW;

struct boiler_info
{
    double airTemp;
    double waterTemp;
    double waterInTemp;
    double waterOutFlow;
    double waterLevel;  
};

void openConnection(char *host, int port);

char* sendMessage(char *message);

void getBoilerInfo(struct boiler_info *info);

char* setBoilerControl(char *controlType, double value);