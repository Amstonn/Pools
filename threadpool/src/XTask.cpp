#include "XTask.h"

using namespace std;
bool MyTask::Init(){
    for(long i=0;i<10;i++){
        cout << i+1 << "-";
    }
    cout << endl;
    return true;
}
