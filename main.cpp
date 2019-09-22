#include "Antintruder.h"
#include <iostream>
#include "ftp.h"

int main() {
    Antintruder ai("Intruder_", 0.04, 30.0, Interval(10), Interval(10));
    ai.run();
}
