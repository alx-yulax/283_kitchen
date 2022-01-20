#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <mutex>

std::mutex access;

enum class Dishes {
    PIZZA,
    SOUP,
    STEAK,
    SALAD,
    SUSHI
};

class Order {
    Dishes dish;
public:
    Order() {
        std::srand(std::time(nullptr));
        switch (std::rand() % 5) {
            case 0:
                dish = Dishes::PIZZA;
                break;
            case 1:
                dish = Dishes::SOUP;
                break;
            case 2:
                dish = Dishes::STEAK;
                break;
            case 3:
                dish = Dishes::SALAD;
                break;
            case 4:
                dish = Dishes::SUSHI;
                break;
        }
    }

    std::string getNameDish() {
        if (dish == Dishes::PIZZA) {
            return "pizza";
        } else if (dish == Dishes::SOUP) {
            return "soup";
        } else if (dish == Dishes::STEAK) {
            return "steak";
        } else if (dish == Dishes::SALAD) {
            return "salad";
        } else if (dish == Dishes::SUSHI) {
            return "sushi";
        } else {
            return "unknown";
        }
    }

};

class Kitchen {
    std::vector<Order *> orders;
    std::vector<Order *> ordersToDelivery;
    Order *orderInProcess;
    bool open;

public:
    Kitchen() : orderInProcess(nullptr), open(true) {
        std::cout << "Kitchen is waiting order..." << std::endl;
    }

    void takeNewOrder() {
        access.lock();
        if (open) {
            orders.push_back(new Order);
            std::cout << "-------------waiter: new order------------" << std::endl;
            for (int i = 0; i < orders.size(); ++i) {
                std::cout << orders[i]->getNameDish() << std::endl;
            }
            access.unlock();
            std::cout << std::endl;
        }
    }

    void cooking() {
        access.lock();
        if (open) {
            orderInProcess = orders[0];
            orders.erase(orders.begin());
            std::cout << "-------------kitchen: cooking-------------" << std::endl;
            std::cout << orderInProcess->getNameDish() << std::endl;
            std::cout << std::endl;
        }
        access.unlock();
    }

    void done() {
        access.lock();
        if (open) {
            ordersToDelivery.push_back(orderInProcess);
            orderInProcess = nullptr;

            std::cout << "-------------kitchen: done---------------" << std::endl;
            for (int i = 0; i < ordersToDelivery.size(); ++i) {
                std::cout << ordersToDelivery[i]->getNameDish() << std::endl;
            }
            std::cout << std::endl;
        }
        access.unlock();
    }

    int getOrdersCount() {
        access.lock();
        int ordersSize = orders.size();
        access.unlock();
        return ordersSize;

    }

    void delivery(int number) {
        std::cout << "#" << number << "----------Courier: delivery-------------" << std::endl;
        access.lock();
        for (int i = 0; i < ordersToDelivery.size(); ++i) {
            std::cout << ordersToDelivery[i]->getNameDish() << std::endl;
            delete ordersToDelivery[i];
            ordersToDelivery[i] = nullptr;
        }
        ordersToDelivery.clear();
        access.unlock();
        std::cout << std::endl;
    }

    void close() {
        access.lock();
        open = false;
        for (int i = 0; i < orders.size(); ++i) {
            delete orders[i];
            orders[i] = nullptr;
        }
        orders.clear();

        for (int i = 0; i < ordersToDelivery.size(); ++i) {
            delete ordersToDelivery[i];
            ordersToDelivery[i] = nullptr;
        }
        ordersToDelivery.clear();

        delete orderInProcess;
        orderInProcess = nullptr;
        std::cout << "Sorry kitchen is closed." << std::endl;
        access.unlock();
    }

};

void cook(Kitchen *kitchen) {

    if (kitchen->getOrdersCount() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
        std::srand(std::time(nullptr));
        kitchen->cooking();
        std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 16 + 10));
        kitchen->done();
    }
    std::thread cookThread(cook, kitchen);
    cookThread.detach();
}

void getNewOrder(Kitchen *kitchen) {
    std::srand(std::time(nullptr));
    std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 11 + 5));
    kitchen->takeNewOrder();

    std::thread NewOrderThread(getNewOrder, kitchen);
    NewOrderThread.detach();
}

int main() {

    Kitchen *kitchen = new Kitchen;

    std::thread NewOrderThread(getNewOrder, kitchen);
    NewOrderThread.detach();

    std::thread cookThread(cook, kitchen);
    cookThread.detach();

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        kitchen->delivery(i);
    }

    kitchen->close();
    delete kitchen;
    kitchen = nullptr;

    return 0;
}
