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
long long generator_seed; //pseudoaleatory generator seed
long long int sequence_size, total_operations, rq_frequency, up_frequency, M; //input values
string F; //string to retain f function result

//random generator implementation
struct RNG {
    long long int A = 1664525, C = 1013904223, R = pow(2, 32), xKn = -1;
    long long int next() {
        return xKn == -1 ? xKn = generator_seed : xKn = (long long int)(A * xKn + C) % R;
    }
};

//sparse table implementation takne from the class notes
class SparseTable {
public:
    vector<vector<long long int> > ST;
    function<long long int(long long int,long long int)> f;
    int v_size, st_size;
    SparseTable(long long int size, function<long long int(long long int,long long int)>& ff) {
        v_size = size;
        st_size = floor(log2(v_size)) + 1;
        ST.assign(st_size, vector<long long int>(v_size, 0));
        f = ff;
    }

// F(mxn) from sparse table -> m = floor(logn) + 1, 0 <= l <= r <= n
// output ff(A[l;r])
    long long int range_query(int l, int r) {
        long long int aux = 0;
        if(F != "SUM"){
            aux = ST[0][l];
        }
        while (l < r) {
            int calc = floor(log2(r-l));
            aux = f(aux, ST[calc][l]);
            l += pow(2, calc);
        }
        return aux;
    }

    //F(mxn); f1: u -> w; u: w^2 -> w; 0 <= j < ; v
    //output: refreshed sparse table, A[j] <- v
    long long update(long long int newValue, int pos_att_value) {
        int k = 1;
        ST[0][pos_att_value] = newValue;
        for (int i = 1; i < st_size; i++) {
            int maxPos = max(pos_att_value - 2*k + 1, 0);
            for (; maxPos <= pos_att_value; maxPos++) {
                ST[i][maxPos] = f(ST[i - 1][maxPos], ST[i - 1][maxPos + k]);
            }
            k = 2*k;
        }
        return range_query(pos_att_value, sequence_size);
    }
};

//min, max and sum functions -> to decide which one'll be computed by the sparse table
long long int sumValue(long long int firstOp, long long int secOp) {
    return firstOp + secOp;
}
long long int maxValue(long long int firstOp, long long int secOp) {
    return (firstOp > secOp) ? firstOp : secOp;
}
long long int minValue(long long int firstOp, long long int secOp) {
    return (firstOp < secOp) ? firstOp : secOp;
}

//main code
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
    int left_range, right_range, pos_new_value;
    long long int aux_RNG, result, new_value;
    //while we have inputs to receive, this whole piece of code will continue to do so
    while(cin >> generator_seed >> sequence_size >> F >> total_operations >> rq_frequency >> up_frequency) {
        RNG randomNum;
        M = 4 * sequence_size;
        function<long long int(long long int,long long int)> f;
        f = (F.compare("MAX") == 0) ? maxValue : (F.compare("MIN") == 0) ? minValue : sumValue;

        SparseTable sparseTable =  SparseTable(sequence_size,f);

        cout << "caso " << caseCnt << "\n";

        //using this vector as an aux to keep the original sequence
        //and then transfer this sequence to the first line of the
        //sparse table on the preprocess phase
        vector<long long int> aux(sequence_size);

        for (int i = 0; i < sequence_size; i++) {
            aux[i] = randomNum.next() % M;
        }

        //preprocessing phase
        for (int i = 0; i < sparseTable.v_size; i++) {
            sparseTable.ST[0][i] = aux[i];
        }

        //each ST[i][j] cell keeps the operation between the elements at j and j + 2^i - 1 positions
        //k is used to calculate the elements interval that'll be considered on the operations
        //this was taken from the class notes
        int k = 1;
        for (int i = 1; i < sparseTable.st_size; i++) {
            for (int j = 0; j < sparseTable.v_size; j++) {
                if (j + k < sparseTable.v_size)  {
                    sparseTable.ST[i][j] = f(sparseTable.ST[i-1][j], sparseTable.ST[i-1][j+k]);
                } else {
                    sparseTable.ST[i][j] = sparseTable.ST[i - 1][j];
                }
            }
            k = k * 2;
        }
        //end of preprocessing phase

        //finishing touches -> consults and refreshing operations on the Sparse Table
        //creating output pattern

        /*if aux_RNG % (rq_frequency + up_frequency) is less than rq_frequency,
        a range query is performed by selecting a random left endpoint left_range
        and a random right endpoint right_range (such that left_range < right_range)
        and passing these values to the range_query function of the SparseTable.
        Otherwise, an update operation is performed by selecting a random position
        pos_new_value and a random value new_value and passing these values to the update
        function of the SparseTable*/

        for (int i = 0; i < total_operations; i++) {
            aux_RNG = randomNum.next();
            if (aux_RNG % (rq_frequency + up_frequency) < rq_frequency) {
                left_range = randomNum.next() % sequence_size;
                right_range = left_range + 1 + (randomNum.next() % (sequence_size - left_range));
                result = sparseTable.range_query(left_range, right_range);
                cout << result << "\n";
            } else {
                pos_new_value = randomNum.next() % sequence_size;
                new_value = randomNum.next() % M;
                result = sparseTable.update(new_value, pos_new_value);
                cout << result << "\n";
            }
        }
        caseCnt++;
        cout << "\n";
    }
}