#include <string.h>
#include "entities/graph.h"
#include "dataset.h"
#include "ui.h"

using namespace std;

int main(int argc, char *argv[]) {
    bool sort = argc < 2 || strcmp(argv[1], "--no-sort") != 0;

    try {
        UI ui(sort);
        ui.start();
    } catch (exception ignored) {
        return 1;
    }
 
    return 0;
}