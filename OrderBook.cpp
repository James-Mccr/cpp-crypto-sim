#include <map>
#include "OrderBook.h"
#include "OrderBookEntry.h"

OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

    /** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::map<std::string, bool> productMap;
    for (const OrderBookEntry& e : orders)
    {
        productMap[e.product] = true;
    }

    std::vector<std::string> products;
    for (const auto& e : productMap)
    {
        products.push_back(e.first);
    }

    return products;
}

    /** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
    std::string product,
    std::string timestamp)
{
    std::vector<OrderBookEntry> entries;
    for (const OrderBookEntry& e : orders)
    {
        if (e.orderType != type) continue;
        if (e.product != product) continue;
        if (e.timestamp != timestamp) continue;
        entries.push_back(e);
    }
    return entries;
}

    /** return the price of the highest bid in the sent set */
double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = -1;
    for (const OrderBookEntry& e : orders)
    {
        if (e.price <= max) continue;
        max = e.price;
    }
    return max;
}

    /** return the price of the lowest bid in the sent set */
double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = -1;
    for (const OrderBookEntry& e : orders)
    {
        if (min != -1 && e.price >= min) continue;
        min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    std::string minTime = orders[0].timestamp;
    for (const OrderBookEntry& e : orders)
    {
        if (e.timestamp >= minTime) continue;
        minTime = e.timestamp;
    }
    return minTime;
}

std::string OrderBook::getNextTime(const std::string& timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = getEarliestTime();
    }

    return next_timestamp;
}

/** return the average price for the given orders */
double OrderBook::getAveragePrice(std::vector<OrderBookEntry> &orders)
{
    // no orders, default to 0
    if (orders.empty()) return 0;

    // sum order prices
    double sum{};
    for (const OrderBookEntry& entry : orders)
    {
        sum += entry.price;
    }
    return sum / orders.size();
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);
    std::vector<OrderBookEntry> sales{};
    
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

    for(auto& ask : asks)
    {
        for (auto& bid : bids)
        {
            if (bid.amount == 0) continue;
            if (bid.price >= ask.price)
            {
                OrderBookEntry sale{ask.price,0,timestamp,product,OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                if (bid.amount == ask.amount)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break;
                }
                else if (bid.amount > ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount -= ask.amount;
                    break;
                }
                else 
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount -= bid.amount;
                    bid.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;
}