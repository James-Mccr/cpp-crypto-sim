#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <algorithm>
#include <string>
#include <vector>

class OrderBook
{
public:
    /** construct, reading a csv data file */
    OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
    std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
    std::vector<OrderBookEntry> getOrders(OrderBookType type, std::string product, std::string timestamp);
    std::string getEarliestTime();
    std::string getNextTime(const std::string& timestamp);
    void insertOrder(OrderBookEntry& order);
    std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

    /** return the average price for the given orders */
    static double getAveragePrice(std::vector<OrderBookEntry>& orders);
    /** return the price of the highest bid in the sent set */
    static double getHighPrice(std::vector<OrderBookEntry>& orders);
    /** return the price of the lowest bid in the sent set */
    static double getLowPrice(std::vector<OrderBookEntry>& orders);
    
private:
    std::vector<OrderBookEntry> orders;

};