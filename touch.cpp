#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include "rapidcsv/src/rapidcsv.h"
#include <future>

// Function to analyze data
std::tuple<float, float, float, float, float> analyzeData(const std::vector<float>& prices, const std::string& threadName, std::mutex& mtx, float& sumAverage, float& maxPrice, float& minPrice)
{
    float sum = 0;
    for (float price : prices)
    {
        sum += price;
    }
    float average = sum / prices.size();

    float localMaxPrice = *std::max_element(prices.begin(), prices.end());
    float localMinPrice = *std::min_element(prices.begin(), prices.end());

    std::lock_guard<std::mutex> lock(mtx);
    sumAverage += average;
    if (localMaxPrice > maxPrice) {
        maxPrice = localMaxPrice;
    }
    if (localMinPrice < minPrice) {
        minPrice = localMinPrice;
    }

    return std::make_tuple(average, localMaxPrice, localMinPrice, maxPrice, minPrice);
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    // Read the CSV file
    std::ifstream file("10000 Sales Records.csv");
    std::string line;
    std::vector<std::string> closePrices;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> values;
        std::string value;

        while (std::getline(iss, value, ','))
        {
            values.push_back(value);
        }

        closePrices.push_back(values[12]); // Assumes "Total Profit" is in the 13th column
    }

    // Convert string prices to floating-point numbers
    std::vector<float> floatPrices;
    for (const std::string& price : closePrices)
    {
        try
        {
            floatPrices.push_back(std::stof(price.substr(1)));
        }
        catch (const std::invalid_argument&)
        {
            // Skip this price
        }
    }

    //int numThreads = std::thread::hardware_concurrency();
    int numThreadsToUse = 5;

    // Divide the data into parts
    std::vector<std::vector<float>> parts;
    for (int i = 0; i < numThreadsToUse; ++i)
    {
        std::vector<float> part;
        for (int j = i; j < floatPrices.size(); j += numThreadsToUse)
        {
            part.push_back(floatPrices[j]);
        }
        parts.push_back(part);
    }

    std::mutex mtx;
    float sumAverage = 0;
    float maxPrice = std::numeric_limits<float>::min();
    float minPrice = std::numeric_limits<float>::max();

    std::vector<std::thread> threads;
    std::vector<std::future<std::tuple<float, float, float, float, float>>> futures;
    for (int i = 0; i < numThreadsToUse; ++i)
    {
        futures.push_back(std::async(std::launch::async, analyzeData, parts[i], "Thread " + std::to_string(i + 1), std::ref(mtx), std::ref(sumAverage), std::ref(maxPrice), std::ref(minPrice)));
    }

    // Join threads
    std::vector<std::tuple<float, float, float, float, float>> results(numThreadsToUse);
    for (size_t i = 0; i < futures.size(); ++i)
    {
        results[i] = futures[i].get();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    float average = sumAverage / numThreadsToUse;

    std::cout << "Average total profit: " << average << std::endl;
    std::cout << "Max total profit: " << maxPrice << std::endl;
    std::cout << "Min total profit: " << minPrice << std::endl;

    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Number of threads used: " << numThreadsToUse << std::endl;

    return 0;
}
