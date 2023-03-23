# Grandma's Agenda

This program is an interactive agenda designed to help individuals with Alzheimer's keep track of their daily activities. It displays the scheduled activity at an exact time and prompts the user to mark it as "done" or "undone". The program also alerts the user 10 minutes before a scheduled activity ends (if it's still "undone").

## Installation

Clone the repository using git clone https://github.com/AbdullahAydn/Interactive_agenda.git
Compile the program using a C compiler, such as 
```bash
make
```
Run the program by executing the generated executable file:

```bash
./grandmas-agenda
```

## Usage

Start the program by running the executable file.
The program will prompt you for a time
```bash
HH:MM
```
or the keyword "now".
```bash
now
```
Enter the time or keyword to display the scheduled activity for that time.
If the activity is "undone", the program will ask if you are currently doing it. Answer
```bash
yes
``` 
to mark it as "done" or 
```bash
no
``` 
to leave it as "undone".
The program will also alert you when a scheduled activity is starting or about to end (if it's still "undone").

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[Apache](http://www.apache.org/licenses/)
