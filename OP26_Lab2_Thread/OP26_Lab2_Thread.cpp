#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <string>

using namespace std;

mutex printMutex;

class Ant {
public:
    string name;
    double x, y, speed;

    Ant(string n, double px, double py, double s) : name(n), x(px), y(py), speed(s) {}
    virtual ~Ant() {}
    virtual void move(int steps) = 0;

protected:
    void log(int step, int total) {
        lock_guard<mutex> lock(printMutex);
        cout << name << " | Step " << step << "/" << total << " | Pos: (" << x << ", " << y << ")" << endl;
    }
};

class Worker : public Ant {
    double startX, startY;
    bool returning = false;
public:
    Worker(string n, double px, double py, double s) : Ant(n, px, py, s), startX(px), startY(py) {}

    void move(int steps) override {
        for (int i = 1; i <= steps; ++i) {
            log(i, steps);
            double tx = returning ? startX : 0, ty = returning ? startY : 0;
            double dx = tx - x, dy = ty - y;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist <= speed) {
                x = tx; y = ty;
                returning = !returning;
            }
            else {
                x += (dx / dist) * speed;
                y += (dy / dist) * speed;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

class Warrior : public Ant {
    double radius, centerX, centerY, angle = 0;
public:
    Warrior(string n, double px, double py, double s, double r)
        : Ant(n, px + r, py, s), radius(r), centerX(px), centerY(py) {
    }

    void move(int steps) override {
        for (int i = 1; i <= steps; ++i) {
            log(i, steps);
            angle += speed / radius;
            x = centerX + radius * cos(angle);
            y = centerY + radius * sin(angle);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

int main() {
    vector<thread> army;
    int steps = 20;

    Worker w1("Worker_1", 10, 15, 2.0), w2("Worker_2", 25, 5, 2.5);
    Warrior v1("Warrior_1", 30, 30, 1.5, 5.0), v2("Warrior_2", 0, 0, 1.0, 10.0);

    army.emplace_back(&Worker::move, &w1, steps);
    army.emplace_back(&Worker::move, &w2, steps);
    army.emplace_back(&Warrior::move, &v1, steps);
    army.emplace_back(&Warrior::move, &v2, steps);

    for (auto& t : army) t.join();

    return 0;
}
