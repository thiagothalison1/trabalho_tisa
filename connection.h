struct boiler_info
{
    double airTemp;
    double waterTemp;
    double waterInTemp;
    double waterOutFlow;
    double waterLevel;
    double waterInFlow;
};

extern char* AIR_TEMPERATURE;
extern char* WATER_TEMPERATURE;
extern char* WATER_IN_TEMPERATURE;
extern char* WATER_OUT_FLOW;
extern char* WATER_LEVEL;
extern char* WATER_IN_FLOW;
extern char* HEAT_FLOW;

void openConnection(char *host, int port);

void sendMessage(char *message, char *buffer);

void getBoilerInfo(struct boiler_info *info);

void setBoilerControl(char *controlType, double value);