#include <iostream>
#include <cstdio>
#include <queue>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

static long long A = 1664525, C = 1013904223, R = (long long)pow(2, 32);
static long long generator_seed, xKn;

static long long random_generator() {
    // random generator formula given by the quest
    xKn = (A * xKn + C) % R;
    return xKn;
}

struct MinQueue {
    queue<long long> q;
    vector<long long> m;
    vector<int> pos;
    int push_cnt = 0;
    int pop_cnt = 0;

    int size() {
        return q.size();
    }

    int size_min() {
        return m.size();
    }

    long long min() {
        return m[0];
    }

    int minPos() {
        return pos[0] - pop_cnt;
    }

    void enqueue(long long num) {
        push_cnt++;
        q.push(num);
        auto it = lower_bound(m.begin(), m.end(), num);
        if (it == m.end()) {
            m.push_back(num);
            pos.push_back(push_cnt - 1);
        }
        else {
            int index = it - m.begin();
            m.erase(m.begin() + index, m.end());
            pos.erase(pos.begin() + index, pos.end());
            m.push_back(num);
            pos.push_back(push_cnt - 1);
        }
    }

    long long dequeue() {
        pop_cnt++;
        long long num = q.front();
        q.pop();
        if (m[0] == num) {
            m.erase(m.begin());
            pos.erase(pos.begin());
        }
        return num;
    }
};

int main() {
    MinQueue min_queue;
    int burn_in = 0, op_num = 0, op_freq = 0;
    scanf("%lld %d %d %d", &generator_seed, &burn_in, &op_num, &op_freq);
    xKn = generator_seed;
    min_queue.enqueue(generator_seed);
    for (int i = 1; i < burn_in; i++) {
        min_queue.enqueue(random_generator());
    }

    for (int i = 0; i < op_num; i++) {
        long long newRandomNum = random_generator();
        if (newRandomNum % 10 < op_freq) {
            min_queue.enqueue(random_generator());
        }
        else {
            min_queue.dequeue();
        }
        printf("%d %d\n", min_queue.size_min(), min_queue.minPos());
    }
    return 0;
}