#include "Helper.h"

int delay_us = 100000;
int speed_factor = 1;

void* increment(void* arg);

int main() {
    // Initialize activities
    activity activities[MAX_ACTIVITIES] = {
    {"Breakfast", {8, 50}, {9, 30}, 0},
    {"Morning walk", {9, 00},{10, 15}, 0},
    {"House cleaning", {10, 20},{10, 55}, 0},
    {"Lunch", {11, 00},{12, 00}, 0},
    {"Afternoon nap", {13, 45},{15, 00}, 0},
    {"Grocery shopping", {15, 20}, {15, 45}, 0},
    {"Cooking", {16, 15}, {17, 30}, 0},
    {"Dinner", {17, 45},{18, 30}, 0},
    {"Evening reading", {19, 00},{21, 30}, 0},
    {"Get medicine", {21, 30},{21, 45}, 0}
        };

    get_speed_factor(&speed_factor);
    // Get initial time
    get_time(&time_info);

    do_terminal_setting();

    pthread_t tid;
    pthread_create(&tid, NULL, &increment, &total_time);
    // Loop until end of day (i.e. 24:00)
    while (time_info.local_time->tm_hour < 24) {
        for (int8_t i = 0 ; i < MAX_ACTIVITIES; i++) {
            if (!activities[i].done) {
                is_scheduled(&activities[i], i, time_info.local_time);
                is_due_soon(&activities[i], i, time_info.local_time);
            }
        }       
        get_non_blocking_inputs(activities, &time_info);
        usleep(delay_us);
        get_time(&time_info);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_settings);

    return 0;
}

void* increment(void* arg) {
    double* total_time = (double*)arg;
    while (1) {
        usleep(delay_us);  // sleep for 10ms
        *total_time += speed_factor * ((double)delay_us / 1000000.0);
    }
}