#ifndef MOCKDATA_H
#define MOCKDATA_H

#include <vector>

class MockData {
    public:
        int key;
        int value;

        MockData(int k, int v): key(k), value(v){}
};

extern std::vector<MockData> DATA;

#endif // MOCKDATA_H
