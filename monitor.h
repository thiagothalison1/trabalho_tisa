#include "connection.h"

void writeBoilerInfo(struct boiler_info *newInfoValues);

void readBoilerInfo(struct boiler_info *info);

void monitorTemperature(double tempLowerBoundV, double tempUpperBoundV, double *temperatureInfo);

void monitorLevel(double levelLowerBoundV, double levelUpperBoundV, double *levelInfo);
