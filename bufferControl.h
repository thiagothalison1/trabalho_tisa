struct buffer_data
{
    double waterTemp;
    double waterLevel;
};

void insertValue (double waterLevel, double waterTemp);

struct buffer_data * waitFullBuffer (void);