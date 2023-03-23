#ifndef HEADER_HELPER_H
#define HEADER_HELPER_H

// Include any necessary headers here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>

// Declare any constants here
#define MAX_ACTIVITIES 10 ///< Maximum number of activities that can be scheduled
#define MAX_LENGTH 20 ///< Maximum length of an activity name or time string

// Define struct for activity time
typedef struct {
    int hour; ///< Hour of the activity start or end time
    int minute; ///< Minute of the activity start or end time
} atime;

// Define struct for activity
typedef struct {
    char name[MAX_LENGTH]; ///< Name of the activity
    atime start_time; ///< Start time of the activity
    atime end_time; ///< End time of the activity
    int done; ///< Flag indicating if the activity has been completed
} activity;

typedef struct {
    struct tm* local_time; ///< Pointer to the current local time
    time_t current_time; ///< The current system time
}Time;

// Declare any global variables here
extern struct termios old_terminal_settings, new_terminal_settings; ///< Terminal settings structs for resetting terminal settings
extern int old_file_desc_flag, new_file_desc_flag; ///< File descriptor flags for resetting terminal settings
extern Time time_info; ///< Struct for holding the current time information
extern double total_time; ///< Total time elapsed since program start

/**
 * @brief Gets user input for speed factor
 *
 * This function prompts the user to input a speed factor between 1 and 30,
 * which determines how many times faster the program should run. It repeatedly
 * asks for input until a valid speed factor is entered.
 *
 * @param[in,out] speed_factor Pointer to integer variable to store speed factor
 */
void get_speed_factor(int* speed_factor); ///< Function for getting the speed factor from user input

/**
 * @brief Gets the current local time and adjusts it by the total time elapsed
 *
 * This function retrieves the current local time and adjusts it by the total time elapsed
 * since the timer started. It also stores the current time and the local time in the
 * Time struct.
 *
 * @param[in,out] time_info Pointer to the Time struct containing time information
 *
 * @return Returns a pointer to the struct tm containing the adjusted local time
 */
struct tm* get_time(Time* time_info); ///< Function for getting the current local time

/**
 * @brief Sets terminal settings and stdin file descriptor for non-blocking mode.
 *
 * This function sets the terminal settings to turn off canonical mode and echo for stdin. It also sets the stdin file
 * descriptor to non-blocking mode. The previous terminal settings and file descriptor flags are saved to be restored
 * later using `restore_terminal_settings()` function.
 *
 * @return void
 */
void do_terminal_setting(void); ///< Function for setting the terminal to non-canonical mode

/**
 * @brief Check if an activity is scheduled for a given datetime
 *
 * This function checks if the activity at the given index i is scheduled to start at the given time t. The check
 * is done by comparing the start time of the activity with the hour and minute fields of the given time struct.
 * If the activity is scheduled for the given time, the function prints a message indicating that it is time for the
 * activity and calls the activity_time() function to display its details.
 *
 * The function uses a static flag to keep track of which activities have been checked at least once during the current
 * minute. This is done to avoid printing the "Time for activity" message multiple times for the same activity during
 * the same minute.
 *
 * @param[in] a Pointer to the activity to check
 * @param[in] i Index of the activity in the schedule
 * @param[in] t Pointer to the time struct to check against
 *
 * @return 1 if the activity is scheduled for the given time, 0 otherwise
 */
int is_scheduled(activity* a, int i, struct tm* t); ///< Function for checking if an activity is scheduled at the current time

/**
 * @brief Checks if an activity is due soon
 *
 * This function checks if the given activity is due to be completed within the next 10 minutes.
 *
 * @param[in] a Pointer to the activity to check
 * @param[in] i Index of the activity in the activities array
 * @param[in] t Pointer to the struct tm representing the current time
 *
 * @return Returns 1 if the activity is due soon, 0 otherwise
 */
int is_due_soon(activity* a, int i, struct tm* t); ///< Function for checking if an activity is due to start in 10 minutes

/**
 * @brief Reads non-blocking input from stdin and parses it for initial time input
 *
 * @param a Pointer to the activity being scheduled
 * @param time_info Pointer to a Time struct containing the current time
 *
 * This function reads non-blocking input from stdin and stores it in a temporary buffer. When a newline character is
 * received, it checks the input using the check_input function. If the input is valid, the parse_time function is used to
 * extract time information from the input and store it in the provided Time struct. If the input is not valid, an error
 * message is printed to stdout. The temporary buffer is then reset for the next input.
 */
void get_non_blocking_inputs(activity* a, Time* time_info); ///< Function for getting user input in a non-blocking way

#endif /* HEADER_HELPER_H */
