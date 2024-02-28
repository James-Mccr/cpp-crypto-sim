#include "MerkelMain.h"
#include <exception>
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"

MerkelMain::MerkelMain()
{


}

void MerkelMain::init()
{
    wallet.insertCurrency("BTC", 10);
    currentTime = orderBook.getEarliestTime();
    int input;
    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;
    // 7 price change
    std::cout << "7: Average prices " << std::endl;

    std::cout << "============== " << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (const std::string& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Mark and offer - enter the amount " << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> tokens;
    try
    {
        tokens = CSVReader::tokenise(input, ',');
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to tokenise input: " << input << std::endl;
        std::cout << e.what() << std::endl;
        return;
    }

    if (tokens.size() != 3)
    {
        std::cout << "Unexpected number of tokens! Expected 3 but there were " << tokens.size() << std::endl;
        return;
    }

    try
    {
        OrderBookEntry obe = CSVReader::stringsToOBE(
            tokens[1], 
            tokens[2], 
            currentTime, 
            tokens[0], 
            OrderBookType::ask,
            "simuser");

        if (wallet.canFulfillOrder(obe))
        {
            std::cout << "Wallet looks good" << std::endl;
            orderBook.insertOrder(obe);
        }
        else 
        {
            std::cout << "Wallet has insufficient funds" << std::endl;
        }
        
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to convert input to order book entry!" << std::endl;
        std::cout << e.what() << std::endl;
        return;
    }

}

void MerkelMain::enterBid()
{
    std::cout << "Make a bid - enter the amount" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> tokens;
    try
    {
        tokens = CSVReader::tokenise(input, ',');
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to tokenise input: " << input << std::endl;
        std::cout << e.what() << std::endl;
        return;
    }

    if (tokens.size() != 3)
    {
        std::cout << "Unexpected number of tokens! Expected 3 but there were " << tokens.size() << std::endl;
        return;
    }

    try
    {
        OrderBookEntry obe = CSVReader::stringsToOBE(
            tokens[1], 
            tokens[2], 
            currentTime, 
            tokens[0], 
            OrderBookType::bid,
            "simuser");

        if (wallet.canFulfillOrder(obe))
        {
            std::cout << "Wallet looks good" << std::endl;
            orderBook.insertOrder(obe);
        }
        else 
        {
            std::cout << "Wallet has insufficient funds" << std::endl;
        }   
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to convert input to order book entry!" << std::endl;
        std::cout << e.what() << std::endl;
        return;
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet << std::endl;
}

void MerkelMain::printAveragePrices(OrderBookType type)
{
    std::vector<std::string> products = orderBook.getKnownProducts();

    std::cout << "Average " << OrderBookEntry::orderBookTypeToString(type) << std::endl;
    for(const std::string& product : products)
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(type, product, currentTime);
        double averagePrice = orderBook.getAveragePrice(entries); 
        if (averagePrice == 0) continue; // products with 0 price do not exist in the current timestamp
        std::cout << product << ": " << averagePrice << std::endl;
    }
    std::cout << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string& p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                wallet.processSale(sale);
            }
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }       
    if (userOption == 7)
    {
        printAveragePrices(OrderBookType::ask);
        printAveragePrices(OrderBookType::bid);
    }
}
 
