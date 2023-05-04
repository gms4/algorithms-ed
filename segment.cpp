#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <queue>
#include <limits>
#include <string>
#include <bits/stdc++.h>

using namespace std;

//variables
long long generator_seed; //rng seeder
long long int sequence_size, total_operations, rq_frequency, up_frequency, print_frequency, M; //M = 4*sequence_size
string F; //operator text

//random generator implementation
struct RNG {
    long long int A = 1664525, C = 1013904223, R = pow(2, 32), xKn = -1;
    long long int next() {
        return xKn == -1 ? xKn = generator_seed : xKn = (long long int)(A * xKn + C) % R;
    }
};

//min, max and sum functions -> to decide which one'll be computed by the segmented tree
long long int sumValue(long long int firstOp, long long int secOp) {
    return firstOp + secOp;
}
long long int maxValue(long long int firstOp, long long int secOp) {
    return (firstOp > secOp) ? firstOp : secOp;
}
long long int minValue(long long int firstOp, long long int secOp) {
    return (firstOp < secOp) ? firstOp : secOp;
}

class SegTree {
    public:
        long long *nodePointer;
        long long int elemCount;
        long long int neutral_element;
        function<long long int(long long int,long long int)> f;
        
        SegTree(long long int segTree[], long long int size,  function<long long int(long long int,long long int)>& ff) {
            elemCount = size;
            nodePointer = new long long[elemCount*2];
            f = ff;
            build(segTree);
        };

    /*
    Building the segmented tree from the original array:
    It fills the leaves of the tree with the values from 
    the array and then calculates the values of the intermediate 
    nodes by performing the operation defined 
    by the lambda function f on the values of the children
    */
    void build(long long int array[]){
        for (int i = 0; i < elemCount; i++){
            nodePointer[i+sequence_size] = array[i];
        };

        for (int i = elemCount-1; i > 0; i--){
            nodePointer[i] = f(nodePointer[2*i], nodePointer[2*i+1]);
        };
    };
    /*
    Descend the whole tree to refresh leaves
    Refresh parents getting back
    */
    void update(long long int i, long long int newValue) {
        i = elemCount + i;
        nodePointer[i] = newValue;
        i = floor(i/2);
        while (i >= 1) {
            nodePointer[i] = f(nodePointer[2*i], nodePointer[2*i+1]);
            i = floor(i/2);
        };
    };

    /*
    - Bottom-Up
    - Begins at T[n+l], T[n+r], extremeties
    - If l node is your parent's left child, the parent covers the segment
    - If l node is your parent's right child, the parent's segment  surpasses
      the left limit
    */
    long long int range_query(long long int l, long long int r){
        long long int lIntervalEx = elemCount+l;
        long long int rIntervalEx = elemCount+r;
        long long int neutral_aux = neutral_element;

        while (lIntervalEx < rIntervalEx) {
            if (lIntervalEx % 2 != 0) {
                neutral_aux = f(neutral_aux, nodePointer[lIntervalEx]);
                lIntervalEx++;
            } 
            if (rIntervalEx % 2 != 0) {
                rIntervalEx--;
                neutral_aux = f(neutral_aux, nodePointer[rIntervalEx]);
            }
            lIntervalEx = lIntervalEx/2;
            rIntervalEx = rIntervalEx/2;
        }
        return neutral_aux;
    };
};

int main() {
    //sync_with_stdio(false) turns off I/O flux sincronization between CPP and SO
    ios_base::sync_with_stdio(false);
    /*cin.tie(NULL) and cout.tie(NULL) lines
    disassociate the cin and cout streams from the standard input/output streams,
    respectively. This disassociation is important to prevent the program from stalling
    if there's an input/output operation on one stream while the other stream is waiting
    for input/output operations.*/
    cin.tie(NULL);
    cout.tie(NULL);
    int caseCnt = 0;
    while(cin >> generator_seed >> sequence_size >> F >> total_operations >> rq_frequency >> up_frequency >> print_frequency) {
        RNG randomNum;
        M = 4*sequence_size;
        long long int neutral_value = 0, result = 0;

        //function gets text input and attributes value according to it
        //dictates what's the operation and what's the neutral value 
        function<long long int(long long int, long long int)> f;
        f = (F.compare("MIN") == 0) ? minValue : (F.compare("MAX") == 0) ? maxValue : sumValue;
        neutral_value = (F.compare("MIN") == 0) ? LLONG_MAX : (F.compare("MAX") == 0) ? LLONG_MIN : 0;

        cout << "caso " << caseCnt << "\n";
        long long int aux[sequence_size];
        
        for (int i = 0; i < sequence_size; i++) {
            aux[i] = randomNum.next() % M;
        };

        int print_frequency_count = 0;
        SegTree SegTree(aux, sequence_size, f);
        SegTree.neutral_element = neutral_value;
        for (int i = 0; i < total_operations; i++) {
            long long int aux_RNG = randomNum.next();
            if (aux_RNG % (rq_frequency + up_frequency) < rq_frequency) {
                int left_range = randomNum.next() % sequence_size;
                int right_range = left_range + 1 + (randomNum.next() % (sequence_size-left_range));
                result = SegTree.range_query(left_range, right_range);
                if (print_frequency_count % print_frequency == 0) {
                    cout << result << "\n";
                }  
            } else {
                long long int pos_new_value = randomNum.next() % sequence_size;
                long long int new_value = randomNum.next() % M;
                SegTree.update(pos_new_value, new_value);
                if (print_frequency_count % print_frequency == 0){
                    result = SegTree.range_query(pos_new_value, sequence_size);
                    cout << result << "\n";
                }
            }
            print_frequency_count++;
        }
        caseCnt++;
        cout << "\n";
    };    
}