#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include "CSVReader.h"
#include "Wallet.h"

std::string BoolToString(bool b)
{
    return b ? "Success" : "Fail";
}

int main()
{
    MerkelMain app{};
    app.init();

}