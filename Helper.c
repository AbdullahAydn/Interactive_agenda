/**
 * @file Helper.c
 * @brief This file contains helper functions for the activity tracker program.
 *
 * This file contains functions that are used to help with the implementation of the activity tracker program.
 */

#include "Helper.h"

#define CLEAR_TERMINAL_DELAY    2   // Delay for clearing terminal screen (in seconds)
#define INPUT_DELAY             3   // Delay for user input (in seconds)

#define NONBLOCK_MODE           1   // Flag for non-blocking input mode
#define BLOCK_MODE              0   // Flag for blocking input mode


struct termios old_terminal_settings, new_terminal_settings;  // Structs for old and new terminal settings
int old_file_desc_flag, new_file_desc_flag;    // Flags for old and new file descriptor
Time time_info; // Struct for storing time information
double total_time = 0.0; // Total time spent on activities


/**
 * @brief Delays program execution for the given number of seconds.
 *
 * This function delays program execution for the given number of seconds by using a busy loop.
 *
 * @param[in] second The number of seconds to delay program execution.
 */
static void delay_time(int second) {
    // Converting time into microseconds
    int mikro_seconds = 1000000 * second;

    // Storing start time
    clock_t start_time = clock();

    // Looping until required time is not achieved
    while (clock() < start_time + mikro_seconds)
        ;
}


/**
 * @brief Clears the terminal screen.
 *
 * This function clears the terminal screen by printing an escape sequence to the console.
 */
static void clear_terminal(void) {
    delay_time(CLEAR_TERMINAL_DELAY);
    printf("\033[2J");  // Escape sequence to clear terminal
    printf("\033[%d;%dH\n", 0, 0);  // Set cursor position to top-left corner
}


/**
 * @brief Sets the input mode for the terminal.
 *
 * This function sets the input mode for the terminal to either non-blocking or blocking mode.
 *
 * @param[in] mode The input mode to set for the terminal.
 */
static void input_mode(int mode) {
    if (mode) {
        tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal_settings);
        fcntl(STDIN_FILENO, F_SETFL, new_file_desc_flag | O_NONBLOCK);
    }
    else {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_settings);
        fcntl(STDIN_FILENO, F_SETFL, old_file_desc_flag);
    }
}



/**
 * @brief Function to check if the current time is within an activity's scheduled time
 *
 * This function takes an activity and a pointer to a tm structure representing the current time.
 * It returns true if the current time is within the activity's scheduled time, otherwise it returns false.
 *
 * @param[in] a Pointer to the activity to check
 * @param[in] t Pointer to a tm structure representing the current time
 *
 * @return Returns true if the current time is within the activity's scheduled time, false otherwise
 */
static int is_activity_time(activity* a, struct tm* t) {
    // Check if current hour is within the activity's start and end hour
    if (a->start_time.hour < t->tm_hour && a->end_time.hour > t->tm_hour) {
        return true;
    }
    // Check if current hour is the same as the activity's start hour, and if current minute is within the activity's start and end minute
    else if (a->start_time.hour == t->tm_hour) {
        if (a->start_time.minute <= t->tm_min && a->end_time.minute > t->tm_min) {
            return true;
        }
        // Check if the activity ends after the current time, but the start minute has already passed
        if (a->end_time.hour > t->tm_hour && a->start_time.minute <= t->tm_min) {
            return true;
        }
    }
    // Check if current hour is the same as the activity's end hour, and if current minute is before the activity's end minute
    else if (a->end_time.hour == t->tm_hour) {
        if (a->end_time.minute > t->tm_min) {
            return true;
        }
    }
    // Current time is not within activity time
    return false;
}



/**
 * @brief Checks if the input string represents a valid time format (HH:MM)
 *
 * This function takes a string input and checks if it represents a valid time format in the
 * form of "HH:MM", where HH is the hour (00-23) and MM is the minute (00-59).
 *
 * @param[in] input Pointer to the string to check for valid time format
 *
 * @return Returns 1 if the input represents a valid time format, otherwise returns 0
 */
static int check_input(char* input) {
    int hour, minute;

    // Check if input is "now"
    if (strcmp(input, "now") == 0) {
        return 1;
    }

    // Check if input is not in the format of "HH:MM"
    if (strlen(input) != 5 || input[2] != ':') {
        return 0;
    }

    // Try to parse the input into hour and minute integers
    if (sscanf(input, "%d:%d", &hour, &minute) != 2) {
        return 0;
    }

    // Check if hour and minute integers are within valid range
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }

    // Input represents a valid time format
    return 1;
}



/**
 * @brief Function to prompt the user if they are doing the given activity now
 *
 * This function takes a pointer to an activity and prompts the user if they are currently
 * doing the activity. If the activity is already marked as done, it will not prompt the user
 * and will display a message saying that the activity is already done.
 *
 * @param[in,out] a Pointer to the activity to check if it is being done now
 *
 * @return Returns 0 if the activity is marked as done or the user confirms they are doing it now,
 * otherwise returns 1
 */
static int activity_time(activity* a) {
    if (!a->done) {
        input_mode(BLOCK_MODE);
        // Wait for user input
        char user_input[MAX_LENGTH];
        delay_time(INPUT_DELAY);
        do {
            printf("Are you doing %s now? (yes/no)\t", a->name);
            scanf("%s", user_input);
        } while ((!!strcmp(user_input, "yes") + !!strcmp(user_input, "no")) != 1);
        input_mode(NONBLOCK_MODE);
        // Set activity as done if user confirms
        if (strcmp(user_input, "yes") == 0) {
            a->done = 1;
            printf("%s marked as done.\n", a->name);
            clear_terminal();
            return 0;
        }
        clear_terminal();
    }
    else {
        printf("Chill, you've already done: %s\n", a->name);
        clear_terminal();
    }
    return 1;
}



/**
 * @brief Parse user input and check if there is an activity scheduled for the given time
 *
 * This function takes an activity array and a Time struct as input, along with a user input string
 * representing a specific time. It checks if any activities are scheduled for the given time and
 * prompts the user to mark the activity as done if there is one. If there are no activities
 * scheduled, it prints a message indicating so.
 *
 * @param[in,out] a Activity array
 * @param[in,out] time_info Time struct containing current time information
 * @param[in] input User input representing a specific time
 */
static void parse_time(activity* a, Time* time_info, char* input) {
    static struct tm* t_local;
    int activity_status = 1;

    // Get local time
    t_local = localtime(&time_info->current_time);

    // If input is not "now", parse hour and minute from input string
    if (strcmp(input, "now")) {
        sscanf(input, "%d:%d", &t_local->tm_hour, &t_local->tm_min);
    }

    // Loop through activity array and check if there is an activity scheduled for the given time
    for (int8_t i = MAX_ACTIVITIES - 1; i >= 0; i--) {
        if (is_activity_time(&a[i], t_local)) {
            printf("Time for %s\n", a[i].name);
            activity_time(&a[i]);
            activity_status = 0;
        }
    }

    // If no activity is scheduled for the given time, print a message indicating so
    if (activity_status) {
        printf("There is no activity to do.\n");
    }
    clear_terminal();
}




/**
 * @brief Gets user input for speed factor
 *
 * This function prompts the user to input a speed factor between 1 and 30,
 * which determines how many times faster the program should run. It repeatedly
 * asks for input until a valid speed factor is entered.
 *
 * @param[in,out] speed_factor Pointer to integer variable to store speed factor
 */
void get_speed_factor(int* speed_factor) {
    char input[20] = { '\0' };

    // Repeat prompt until valid input is entered
    do {
        printf("How many times would you like to speed it up? (1...30)\t");
        scanf("%s", input);
        *speed_factor = atoi(input);
    } while (!(*speed_factor > 0 && *speed_factor <= 30));

    clear_terminal();
}



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
struct tm* get_time(Time* time_info) {
    // Flag to get the current time only once
    static int get_current_time_flag = 0;

    // Get the current time only once
    if (!get_current_time_flag) {
        time(&time_info->current_time);
        get_current_time_flag = 1;
    }

    // Adjust the current time by the total time elapsed
    if (total_time >= 1) {
        time_info->current_time += total_time;
        total_time = 0.0;
    }

    // Get the local time
    time_info->local_time = localtime(&time_info->current_time);

    // Return a pointer to the struct tm containing the local time
    return time_info->local_time;
}




/**
 * @brief Sets terminal settings and stdin file descriptor for non-blocking mode.
 *
 * This function sets the terminal settings to turn off canonical mode and echo for stdin. It also sets the stdin file
 * descriptor to non-blocking mode. The previous terminal settings and file descriptor flags are saved to be restored
 * later using `restore_terminal_settings()` function.
 *
 * @return void
 */
void do_terminal_setting(void) {
    // get the terminal settings
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    new_terminal_settings = old_terminal_settings;
    new_terminal_settings.c_lflag &= ~(ICANON); // turn off canonical mode and echo
    //new_term.c_lflag &= ~(ICANON | ECHO); // turn off canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal_settings);

    // set non-blocking mode for stdin
    old_file_desc_flag = fcntl(STDIN_FILENO, F_GETFL, 0);
    new_file_desc_flag = old_file_desc_flag;
    fcntl(STDIN_FILENO, F_SETFL, new_file_desc_flag | O_NONBLOCK);
}





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
int is_scheduled(activity* a, int i, struct tm* t) {
    static int previous_min;
    static int check_flags = 0;
    int return_val = 0;

    if (!previous_min)
        previous_min = t->tm_min;

    if (!((check_flags >> i) & 1U) && a->start_time.hour == t->tm_hour && a->start_time.minute == t->tm_min) {
        printf("Time for %s\n", a->name);
        activity_time(a);
        return_val = 1;
    }
    else {
        return_val = 0;
    }
    check_flags |= 1UL << i;
    if (t->tm_min != previous_min) {
        check_flags = 0;
        previous_min = t->tm_min;
    }
    return return_val;
}


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
int is_due_soon(activity* a, int i, struct tm* t) {
    static int previous_min;
    static int check_flags = 0;
    int return_val;

    if (!previous_min)
        previous_min = t->tm_min;

    if (!((check_flags >> i) & 1U) && is_activity_time(a, t) && ((a->end_time.hour == t->tm_hour && a->end_time.minute - \
        t->tm_min == 10) || (a->end_time.hour == (t->tm_hour + 1) && (a->end_time.minute + 60) - t->tm_min == 10))) {
        printf("Don't forget to do %s in 10 minutes!\n", a->name);
        activity_time(a);
        return_val = 1;
    }
    else {
        return_val = 0;
    }

    check_flags |= 1UL << i;
    if (t->tm_min != previous_min) {
        check_flags = 0;
        previous_min = t->tm_min;
    }
    return return_val;
}


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
void get_non_blocking_inputs(activity* a, Time* time_info) {
    static int idx = 0;
    static char temp_buf[MAX_LENGTH] = { '\0' };
    char buf[MAX_LENGTH] = { '\0' };
    int n;

    n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) {
        buf[n] = '\0'; // add null terminator to the end of the input
        strcpy(&temp_buf[idx], buf);
        idx += n;
        if (buf[n - 1] == '\n') {
            temp_buf[idx - 1] = '\0';
            if (check_input(temp_buf)) {
                //Parse initial time input
                parse_time(a, time_info, temp_buf);
            }
            else {
                printf("Please enter a time (\"now\" or \"HH:MM\")\n");
            }
            memset(temp_buf, '\0', idx);
            idx = 0;
        }
    }
}
