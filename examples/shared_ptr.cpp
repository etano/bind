#include "utils/bind.hpp"
#define N 9

void norm(bind::shared_ptr<double>& norm, bind::array<double>& vec){
    for(int i = 0; i < vec.size(); i++) *norm += vec[i]*vec[i];
    *norm = std::sqrt(*norm);
}

int main(){
    bind::shared_ptr<double> n(0);
    bind::array<double> vec(N);
    vec.fill(1);

    bind::cpu(norm, n, vec);
    bind::sync();

    std::cout << "Norm: " << n << "\n";
    return 0;
}