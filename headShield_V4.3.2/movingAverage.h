#pragma once

#include <vector>

class MovingAverage
{
private:
    int size;                // size of the moving average
    std::vector<int> values; // array to hold the measurements
    int index = 0;           // current index into the array
    int sum = 0;             // sum of the measurements
    int count = 0;           // count of measurements taken

public:
    MovingAverage(int size)
        : size(size)
    {
        values.resize(size, 0);
    }
    /*
    void add(int value)
    {
        if (count < size)
            count++; // increase count until we have 'size' elements
        else
            sum -= values[index];   // subtract the old value from the sum
        sum += value;               // add the new value to the sum
        values[index] = value;      // store the new value in the array
        index = (index + 1) % size; // move to the next position in the array
    }
*/
    void add(int value)
    {
        if (count < size)
            count++;
        else
            sum -= values[index];

        sum += value;
        values[index] = value;
        index = (index + 1) % size;
    }

    void addMajority(bool value)
    {
        if (count < size)
        {
            count++; // increase count until we have 'size' elements
        }
        else
        {
            sum -= values[index]; // subtract the old value from the sum
        }
        sum += value;               // add the new value to the sum
        values[index] = value;      // store the new value in the array
        index = (index + 1) % size; // move to the next position in the array
    }

    bool getMajority()
    {
        return !(sum <= count / 2);
    }
    float average()
    {
        if (count == 0) // if there are less than 'size' measurements
            return -1;  // return -1
        else
            return (float)sum / count; // calculate and return the average
    }

    void clear()
    {
        values.clear();
        values.resize(size, 0);
        index = 0;
        sum = 0;
        count = 0;
    }

};
