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
std::tuple<float, float, float, float, float> analyzeData(const std::vector<float>& prices, const std::string& threadName, std::mutex& mtx, float& sumAverage)
{
 float sum = 0;
 for (float price : prices)
 {
     sum += price;
 }
 float average = sum / prices.size();

 float maxPrice = *std::max_element(prices.begin(), prices.end());
 float minPrice = *std::min_element(prices.begin(), prices.end());

 mtx.lock();
 sumAverage += average;
 mtx.unlock();

 return std::make_tuple(average, maxPrice, minPrice, maxPrice, minPrice);
}

int main()
{
 auto start = std::chrono::high_resolution_clock::now();

 // Read the CSV file
 std::ifstream file("data.csv");
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

     closePrices.push_back(values[3]); // Assumes "Close" prices are in the fourth column
 }

 // Convert string prices to floating-point numbers
 std::vector<float> floatPrices;
 for (const std::string& price : closePrices)
 {
     try
     {
         floatPrices.push_back(std::stof(price));
     }
     catch (const std::invalid_argument&)
     {
         // Skip this price
     }
 }

 int numThreads = std::thread::hardware_concurrency();
 int numThreadsToUse = numThreads * 20 / 100; // Use 40% of the system's resources

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
/*
 // Create and start threads
 std::vector<std::thread> threads;
 std::vector<std::tuple<float, float, float, float, float>> results(numThreadsToUse);
 for (int i = 0; i < numThreadsToUse; ++i)
 {
     threads.push_back(std::thread(analyzeData, parts[i], "Thread " + std::to_string(i + 1), std::ref(mtx), std::ref(sumAverage)));
 }

 // Join threads
 for (std::thread& thread : threads)
 {
     thread.join();
 }

 auto end = std::chrono::high_resolution_clock::now();
 std::chrono::duration<double> elapsed = end - start;

 float average = sumAverage / numThreadsToUse;
 float maxPrice = *std::max_element(results.begin(), results.end(), [](const std::tuple<float, float, float, float, float>& a, const std::tuple<float, float, float, float, float>& b) {
     return std::get<3>(a) < std::get<3>(b);
 });
float minPrice = *std::min_element(results.begin(), results.end(), [](const std::tuple<float, float, float, float, float>& a, const std::tuple<float, float, float, float, float>& b) {
    return std::get<4>(a) > std::get<4>(b);
});

std::cout << "Average close price: " << average << std::endl;
std::cout << "Max close price: " << maxPrice << std::endl;
std::cout << "Min close price: " << minPrice << std::endl;

std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
std::cout << "Number of threads used: " << numThreadsToUse << std::endl;

 return 0;*/
 std::vector<std::thread> threads;
std::vector<std::future<std::tuple<float, float, float, float, float>>> futures;
for (int i = 0; i < numThreadsToUse; ++i)
{
    futures.push_back(std::async(std::launch::async, analyzeData, parts[i], "Thread " + std::to_string(i + 1), std::ref(mtx), std::ref(sumAverage)));
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
float maxPrice = std::get<3>(*std::max_element(results.begin(), results.end(), [](const std::tuple<float, float, float, float, float>& a, const std::tuple<float, float, float, float, float>& b) {
    return std::get<3>(a) < std::get<3>(b);
}));
float minPrice = std::get<4>(*std::min_element(results.begin(), results.end(), [](const std::tuple<float, float, float, float, float>& a, const std::tuple<float, float, float, float, float>& b) {
    return std::get<4>(a) > std::get<4>(b);
}));

std::cout << "Average close price: " << average << std::endl;
std::cout << "Max close price: " << maxPrice << std::endl;
std::cout << "Min close price: " << minPrice << std::endl;

std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
std::cout << "Number of threads used: " << numThreadsToUse << std::endl;

return 0;
}


