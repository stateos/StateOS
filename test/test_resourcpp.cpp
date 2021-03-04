#include "test_resources.h"

stateos::Task        Tsk0(0, []{});
stateos::Task        Tsk1(1, []{});
stateos::TaskT<512>  Tsk2(2, []{});
stateos::TaskT<1024> Tsk3(3, []{});
stateos::Task        Tsk4(4, []{});
stateos::Task        Tsk5(5, []{});
