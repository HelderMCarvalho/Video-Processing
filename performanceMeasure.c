//
// Created by Helder Carvalho on 07/03/2021.
//

/*
 * Measurement:
 *  Start:                  double begin = get_cpu_time();
 *  Stop and show time:     double end = get_cpu_time(); printf("Time measured: %.3f seconds.\n", (end - begin));
 *
 *  Source: https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
 *  Example 8
*/

#include <processthreadsapi.h>

double get_cpu_time() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return (double) (d.dwLowDateTime | ((unsigned long long) d.dwHighDateTime << 32)) * 0.0000001;
    } else {
        //  Handle error
        return 0;
    }
}
