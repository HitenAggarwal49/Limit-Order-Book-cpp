// This program simulates the stock market and does trades if a buyer and a seller can match.
// It prioritises the most eager buyer with the most eager seller and then prioritises on a first come first served basis.

/*
Orders are saved in this format:-
name||price||quantity
*/

#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
using namespace std;

// Defined a structure to represent an Order
struct Order
{
    string name;
    double price;
    int quantity;
};

// To store buy orders
vector<Order> buyOrders;
// To store sell orders
vector<Order> sellOrders;

// In this function, buys are sorted descendingly so that the most eager buyer is prioritised
// Similarly, sells are sorted ascendingly to prioritise the most eager seller
void sortPrice(vector<Order> &orders, bool BUY)
{
    stable_sort(orders.begin(), orders.end(), [BUY](const Order &left, const Order &right)
                { return BUY ? (left.price > right.price) : (left.price < right.price); });
}

// This function removes any orders which have no quantity left
void cleanUp(vector<Order> &orders)
{
    orders.erase(remove_if(orders.begin(), orders.end(), [](const Order &a)
                           { return a.quantity <= 0; }),
                 orders.end());
}

// This is the most important function
// It iterates over the sorted list of buyers and sellers and tries matching them with each other
// This is done such that if a buyer is eager to pay more than or equal to what a seller is selling for, a transaction happens
void transaction()
{
    cleanUp(buyOrders);
    cleanUp(sellOrders);
    sortPrice(buyOrders, true);
    sortPrice(sellOrders, false);

    for (Order &buy : buyOrders)
    {
        if (buy.quantity <= 0)
        {
            continue;
        }
        for (Order &sell : sellOrders)
        {
            if (sell.quantity <= 0)
            {
                continue;
            }
            if (buy.name == sell.name)
            {
                if (buy.price >= sell.price)
                {
                    int transQuant{min(buy.quantity, sell.quantity)};
                    buy.quantity = buy.quantity - transQuant;
                    sell.quantity = sell.quantity - transQuant;
                    cout << "Successful Transaction:-\nStock Traded:- " << buy.name << "\nPrice Traded= " << buy.price << "\nQuantity Traded = " << transQuant << "\nTotal Cost = " << transQuant * buy.price << "\n\n";
                }
            }
            if (buy.quantity <= 0)
            {
                break;
            }
        }
    }
    cleanUp(buyOrders);
    cleanUp(sellOrders);
}

void addOrder(vector<Order> &typeOrder, string name, double price, int quantity)
{
    typeOrder.push_back({name, price, quantity});
}

void saveOrders(const vector<Order> &orders, const string &filename)
{
    ofstream write(filename);
    if (!write)
    {
        cout << "Could not open file " << filename << "\n";
        return;
    }
    write << fixed << setprecision(2);
    for (auto &order : orders)
    {
        write << order.name << "||" << order.price << "||" << order.quantity << "\n";
    }
}

void saveAndExit()
{
    cout << "Exiting Program...\n";
    saveOrders(buyOrders, "buy_Orders.txt");
    saveOrders(sellOrders, "sell_Orders.txt");
    exit(0);
}

void loadOrders(vector<Order> &orders, const string &filename)
{
    ifstream read(filename);
    if (!read)
    {
        cout << "Could not open file " << filename << "\n";
        return;
    }
    string line;
    while (getline(read, line))
    {
        auto p1{line.find("||")};
        auto p2{line.find("||", p1 + 2)};
        if (p1 == string::npos || p2 == string::npos)
        {
            continue;
        }

        try // To just skip the lines that don't match the format
        {
            Order order;
            order.name = line.substr(0, p1);
            order.price = stod(line.substr(p1 + 2, p2 - (p1 + 2)));
            order.quantity = stoi(line.substr(p2 + 2));
            orders.push_back(order);
        }
        catch (const exception &)
        {
            continue;
        }
    }
}

void listOrders(const vector<Order> &orders)
{
    if (orders.empty())
    {
        cout << "No Orders Found\n";
        return;
    }
    for (auto &order : orders)
    {
        cout << order.name << "\n";
        cout << "Price: " << order.price << "\n";
        cout << "Quantity: " << order.quantity << "\n\n";
    }
}

// Reads one whole line. If input has been closed (Ctrl+D), saves and exits.
string readLine()
{
    string line;
    if (!getline(cin, line))
    {
        saveAndExit();
    }
    return line;
}

// Reads one whole line and checks it holds exactly one number
// (spaces around it are fine). Returns false for anything else.
bool readNumber(double &value)
{
    istringstream in{readLine()};
    char extra{};
    return (in >> value) && !(in >> extra);
}

bool readNumber(int &value)
{
    istringstream in{readLine()};
    char extra{};
    return (in >> value) && !(in >> extra);
}

// To check whether a character counts as whitespace for trim()
bool isWhitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

// To remove the leading and trailing whitespaces
// Importantly, also converts multiple consecutive whitespaces to a single whitespace
void trim(string &convertString)
{
    unsigned int i = 0;
    for (i = 0; i + 1 < convertString.length(); i++)
    {
        if (convertString[i] == ' ' && convertString[i] == convertString[i + 1])
        {
            convertString.erase(i, 1);
            i--; // Otherwise the character next to i will slide into this and leave unchecked
        }
    }

    while (!convertString.empty() && isWhitespace(convertString[0]))
        convertString.erase(0, 1);

    while (!convertString.empty() && isWhitespace(convertString.back()))
        convertString.pop_back();
}

void title(string &convertString)
{
    bool wordStart = true;
    for (unsigned int i = 0; i < convertString.length(); i++)
    {
        if (convertString[i] == ' ')
            wordStart = true;
        else if (wordStart)
        {
            convertString[i] = toupper(convertString[i]);
            wordStart = false;
        }
        else
            convertString[i] = tolower(convertString[i]);
    }
}

void style(string &convertString)
{
    trim(convertString);
    title(convertString);
}

void getDetails(vector<Order> &order)
{
    // To store the name of the stock
    string name{};
    // To store the required price of each share
    double price{};
    // To store the desired number of shares at the desired price
    int quantity{};

    cout << R"(
       
    Please enter the following details:-
       
    1) The name of the stock:- )";
    name = readLine();
    style(name);
    if (name.empty())
    {
        cout << "Invalid Name. Please try again.\n";
        return;
    }

    cout << "\n2) Desired value of each share:- ";
    if (!readNumber(price) || !isfinite(price) || price <= 0)
    {
        cout << "Invalid Price. Please try again.\n";
        return;
    }

    cout << "\n3) Number of Shares:- ";
    if (!readNumber(quantity) || quantity <= 0)
    {
        cout << "Invalid Number of Shares. Please try again.\n";
        return;
    }

    cout << "\n";
    addOrder(order, name, price, quantity);
}

void prompt()
{
    // To store user choice
    int choice{};
    cout << R"(
    
    **Stock Exchange!**
    
    What would you like to do?
    1) Buy a Stock
    2) Sell a Stock
    3) View Buy Orders
    4) View Sell Orders
    5) Exit

    )";

    if (!readNumber(choice))
    {
        cout << "Invalid choice!!\n";
        return;
    }

    switch (choice)
    {

    case 1:
    {
        getDetails(buyOrders);
        transaction();
        break;
    }

    case 2:
    {
        getDetails(sellOrders);
        transaction();
        break;
    }

    case 3:
    {
        listOrders(buyOrders);
        break;
    }

    case 4:
    {
        listOrders(sellOrders);
        break;
    }

    case 5:
    {
        saveAndExit();
    }

    default:
    {
        cout << "Invalid Choice!!\n";
        return;
    }
    }
}

int main()
{
    cout << fixed << setprecision(2);
    loadOrders(buyOrders, "buy_Orders.txt");
    loadOrders(sellOrders, "sell_Orders.txt");
    transaction(); // To match the orders which may already be present in buys and sells lists
    while (true)
    {
        prompt();
    }
}