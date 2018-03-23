#include "connection.h"

void writeBoilerInfo(struct boiler_info *newInfoValues);

void readBoilerInfo(struct boiler_info *info);

void monitorTemperature(double tempLowerBoundV, double tempUpperBoundV);
