#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_LINE_LENGTH 100

//get the sub string if the number has more than 1 digit
int getSubString(char line[], int a, int b)
{
    char subbuff[a + 1];
    memcpy(subbuff, &line[b], a);
    subbuff[a] = '\0';
    int x = atoi(subbuff);

    return x;
}

//read multiple files
int readData(int n, int fd[][2], int argc, char *argv[], int col)
{
    if (n == 0)
        return 0;

    int pid = fork();
    if (pid == 0)
    {
        //make recursion to run thru all the text files
        readData(n - 1, fd, argc, argv, col);
        char line[MAX_LINE_LENGTH] = {0};
        FILE *file = fopen(argv[n], "r");
        //if incorrect file => print: cannot open file
        if (!file)
        {
            printf("cannot open file\n");
            exit(1);
        }

        int i, sum = 0;
        /* Get each line until there are none left */
        while (fgets(line, MAX_LINE_LENGTH, file))
        {
            int count = -1;
            // Iterating each character through for loop.
            for (i = 0; line[i] != '\0'; i++)
            {
                if ((line[i] >= '0') && (line[i] <= '9')) // Checking for numeric characters.
                {
                    count++;
                    if (count / col >= 1)
                        break;
                    else
                    {
                        //negative number handling
                        if (line[i - 1] == '-')
                        {
                            int a = 1;
                            int b = i;
                            while((line[i+1] >= '0') && (line[i+1] <= '9')){
                                i++;
                                a++;
                            }
                            int y = getSubString(line, a, b);
                            printf("Warning - Value -%i found.\n", y);
                            continue;
                        }
                        //positive number handling
                        else
                        {
                            int a = 1;
                            int b = i;
                            while((line[i+1] >= '0') && (line[i+1] <= '9')){
                                i++;
                                a++;
                                }
                            int x = getSubString(line, a, b);
                            sum += x; // Adding numeric characters.
                        }
                    }
                }
            }
        }
        //close file
        fclose(file);

        //Use the pipe to transfer the array
        write(fd[n - 1][1], &sum, sizeof(sum));
        exit(0);
    }
    else
    {
        close(fd[n -1][1]);
        wait(NULL);
    }
    return 0;
}
int main(int argc, char *argv[])
{
    // Return error if incorrect number of arguments. Needs at least one file name, min, and max.
    if (argc < 3)
        return 1;
    char* c = argv[argc - 1];
    int col = atoi(c);    

    //Create variables
    int num_of_files = argc - 2, fd[argc][2];

    //Create pipe for each file
    for (int i = 0; i < num_of_files; i++)
        pipe(fd[i]);

    readData(num_of_files, fd, argc, argv, col);

    //Read from pipe and store data in new_arr
    int sum = 0;
    //add up the sum of all text files
    for (int j = 0; j < num_of_files; j++)
    {
        int sumFromChild;
        read(fd[j][0], &sumFromChild, sizeof(sumFromChild));
        sum += sumFromChild;
    }
    //print out the total sum
    printf("Total Sum : %d\n", sum);

    return 0;
}