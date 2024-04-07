/**
 * @file assignment.cpp
 * @author Marcus Robertsson (Marcus.asberg.98@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cmath>
#include <ctime>
#include <condition_variable>
#include <random>

// g++ -D DEALERS_size=2 -D Warehouse_size=8 Wharehouse.cpp -o main && ./main

#ifndef Warehouse_size
#error Please specify the size of Warehouse
#elif Warehouse_size < 8
#error The size of Warehouse must be at least 8.
#endif

#ifndef DEALERS_size
#error Please specify the amount of dealers
#elif DEALERS_size < 2
#error The amount of dealers must be at least 2.
#endif

class Vehicle
{
protected:
    static int Id_counter;
    int Id;
    std::string Model;

public:
    Vehicle()
    {
        Id = Id_counter++;
    }
    virtual void VehicleProperty() const = 0;
    virtual ~Vehicle() = default;

    void print_ID(void) const
    {
        printf("%d", Id);
    }
};

int Vehicle::Id_counter = 1001;

class Car : public Vehicle
{
protected:
    int Passenger;
    std::string Type = " Car ";

public:
    Car(int pass) : Passenger(pass)
    {
        Model = " XC70 ";
    }
    void VehicleProperty() const
    {
        print_ID();
        std::cout << Type << Model << Passenger << " Passangers" << std::endl;
    }
};

class Truck : public Vehicle
{
protected:
    int Load;
    std::string Type = " Truck ";

public:
    Truck(int lod)
    {
        this->Load = lod;
        Model = " Ford-F150 ";
    }
    void VehicleProperty() const
    {
        print_ID();
        std::cout << Type << Model << Load << "kg" << std::endl;
    }
};

template <typename T, int Size>
class Warehouse
{
private:
    int tail{0};
    int head{0};
    size_t counter{0};
    T warehouse[Size];

public:
    Warehouse() = default;
    Warehouse(const Warehouse &) = delete;
    Warehouse &operator=(const Warehouse &) = delete;

    void clear()
    {
        tail = 0;
        head = 0;
        counter = 0;
    }

    T read()
    {
        T value = nullptr;
        if (counter != 0)
        {
            value = warehouse[head];
            head = (head + 1) % Warehouse_size;
            counter--;
        }
        return value;
    }

    void write(const T &value)
    {
        warehouse[tail] = value;
        tail = (tail + 1) % Warehouse_size;
        if (counter == Warehouse_size)
        {
            head = (head + 1) % Warehouse_size;
        }
        else
        {
            counter++;
        }
    }

    int elements() const
    {
        return counter;
    }

    bool empty(void)
    {
        return (counter == 0);
    }

    bool full() const
    {
        return counter == Warehouse_size;
    }
};

std::mutex mtx;
std::condition_variable cv;

Warehouse<std::shared_ptr<Vehicle>, Warehouse_size> warehouse;

void production_line(void)
{
    std::random_device rando;
    std::mt19937 gen(rando());
    std::uniform_int_distribution<> dis(0, 1);
    while (true)
    {
        std::shared_ptr<Vehicle> vehicle;
        if (dis(gen) == 0)
        {
            vehicle = std::make_shared<Car>(4);
        }
        else
        {
            vehicle = std::make_shared<Truck>(5000);
        }

        std::unique_lock<std::mutex> lck{mtx};
        cv.wait(lck, []
                { return !warehouse.full(); });

        warehouse.write(vehicle);
        std::cout << "*Vehicle produced*" << std::endl;

        lck.unlock();
        cv.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen) * 1000));
    }
}

void retail(void)
{
    while (true)
    {
        std::random_device rando;
        std::mt19937 gen(rando());
        std::uniform_int_distribution<> dis(0, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen) * 1000));

        std::unique_lock<std::mutex> lck{mtx};
        cv.wait(lck, []
                { return !warehouse.empty(); });

        std::shared_ptr<Vehicle> vehicle = warehouse.read();
        std::cout << "****to retailer****" << std::endl;

        vehicle->VehicleProperty();

        lck.unlock();
        cv.notify_all();
    }
}
int main()
{

    std::thread manu_facturer_Thr(production_line);

    std::vector<std::thread> thr_retail;
    for (int i = 0; i < 4; ++i)
    {
        thr_retail.emplace_back(retail);
    }
    manu_facturer_Thr.join();
    for (auto &thread : thr_retail)
    {

        thread.join();
    }

    return 0;
}