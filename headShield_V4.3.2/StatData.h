#include <vector>
#include <algorithm>

struct StatData
{
    unsigned int min;
    unsigned int max;
    unsigned int average;
};

class Statistics
{
private:
    std::vector<int> values;
    unsigned long sum = 0;
    unsigned int requiredSize;
    StatData data;

public:
    Statistics(unsigned int size) : requiredSize(size), values() {}

    bool addValue(int value)
    {
        values.push_back(value);
        sum += value;

        if (values.size() == requiredSize)
        {
            // Calculate the statistics
            auto minmax = std::minmax_element(values.begin(), values.end());
            data.min = *minmax.first;
            data.max = *minmax.second;
            data.average = sum / values.size();

            // Clear the vector and reset the sum
            values.clear();
            sum = 0;
            return true;
        }
        return false;
    }

    StatData getStats()
    {
        return data;
    }
};