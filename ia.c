#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void handle_signal(int sig)
{
    write(STDOUT_FILENO,"\nInterrupt detected! Ctrl + C pressed. \n",40);
    exit(EXIT_SUCCESS);
}

int main()
{
    int fd1[2], fd2[2];
    pid_t pid;
    char message[100];
    char buffer[100];
    char input[80];
    char choice[10];
    ssize_t bytes;
    int option;

    signal(SIGINT, handle_signal);

	//create pipes for sending announcement
    pipe(fd1);
    pipe(fd2);

	// create first child process for students
    pid = fork();

    if (pid == 0)
    {

	signal(SIGINT, SIG_IGN);

        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

		//read announcements from the student pipe
        while ((bytes = read(fd1[0], buffer, 100)) > 0)
	{
		buffer[bytes] = '\0';
        	printf("\nStudent [PID: %d] Information: %s\n", getpid(), buffer);
		fflush(stdout);
	}

        close(fd1[0]);
        exit(EXIT_SUCCESS);
    }

	 //create second child process for teachers
    pid = fork();

    if (pid == 0)
    {

	signal(SIGINT, SIG_IGN);

        close(fd2[1]);
        close(fd1[0]);
        close(fd1[1]);

		//read announcement from the teacher pipe
        while ((bytes = read(fd2[0], buffer, 100)) > 0)
	{
		buffer[bytes] = '\0';
		printf("\nTeachers [PID: %d] Notification: %s\n", getpid(), buffer);
		fflush(stdout);
	}

        close(fd2[0]);
        exit(EXIT_SUCCESS);
    }

    close(fd1[0]);
    close(fd2[0]);

	//keep displaying admin menu until user exit the system
    while(1)
    {
	printf("\n====SCHOOL ADMIN SYSTEM====\n");
	printf("1. Send Announcement\n");
	printf("2. Exit\n");
	printf("Enter your choice: \n");
	fflush(stdout);

	//read the menu choice from the user
	if (fgets(choice, sizeof(choice), stdin) == NULL)
	break;

	option = atoi(choice);

	//exit the system if option 2 is selected
	if (option == 2)
	{
		printf("\nExiting School Admin System. \n");
		break;
	}

	//display an error for an invalid menu choice
	if (option != 1)
	{
		printf("Invalid choice.\n");
		continue;
	}

	//ask admin to enter an announcement
	printf("School Admin: Enter an announcement: ");
	fflush(stdout);

	//read the announcement entered by the admin
 	if (fgets(input, sizeof(input), stdin) == NULL)
	break;

	snprintf(message, sizeof(message), "%s", input);

	write(fd1[1], message, sizeof(message));
 	write(fd2[1], message, sizeof(message));

	usleep(100000);
    }

    close(fd1[1]);
    close(fd2[1]);

	//wait for both child processes to finish
    wait(NULL);
    wait(NULL);

    return 0;
}



