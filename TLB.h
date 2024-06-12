#ifndef _TLB_
#define _TLB_

#include <iostream>
#include <vector>
#include <bits/stdc++.h>
//#include <unordered_map>

// Lớp TLBEntry đại diện cho một mục trong TLB

// Lớp TLB mô phỏng TLB
class TLB {
//private:
    std::vector< std ::pair<int, int> > tlbEntries;
    int capacity;

public:
    TLB(int capacity) : capacity(capacity) {}

    int lookup(int pageNumber) {
        for(auto v : tlbEntries){
            if(v.first == pageNumber) return v.second;
        }
        return -1;
    }

    void update(int pageNumber, int frameNumber) {
        if (tlbEntries.size() >= capacity) {
            // Giả sử TLB đơn giản với chính sách loại bỏ mục đầu tiên (FIFO)
            tlbEntries.erase(tlbEntries.begin());
        }
        tlbEntries.push_back(std::make_pair(pageNumber, frameNumber));
    }
};

#endif