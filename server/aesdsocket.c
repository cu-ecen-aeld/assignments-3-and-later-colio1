#define GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdbool.h>

#define USER_PORT 9000
#define MAX_CONNECTIONS 1
#define MAX_SIZE 1024
#define WRITE_TO_FILE "/var/tmp/aesdsocketdata"

int sockfd;
int client_sockfd;
int socket_file_fd;
char* buffer_ptr;
bool caught_sigint = false;
bool caught_sigterm = false;
bool is_daemon = false;

void gracefull_exit()
{
    close(sockfd);
    close(client_sockfd);
    close(socket_file_fd);
    remove(WRITE_TO_FILE);
    if (buffer_ptr) {
        free(buffer_ptr);
    }
}

void signal_handler(int signal_number)
{
    syslog(LOG_INFO, "Caught signal, exiting");
    if (signal_number == SIGINT){
        caught_sigint = true;
    }
    if (signal_number == SIGTERM){
        caught_sigterm = true;
    }
    gracefull_exit();
    exit(0);
}

int main(int argc, char* argv[])
{
    ssize_t nbytes_socket;
    ssize_t nbytes_file;
    long int current_buffer_size;
    char* buffer_read_ptr;
    int conn_count;
    bool got_newline;
    struct sigaction act;
	int stdin_fd;
	int stdouterr_fd;
	struct sockaddr_in serv_addr;

    if (argc == 2)
	{
        if (strncmp(argv[1], "-d", 20) == 0)
            is_daemon = true;
    }
    openlog(NULL, 0, LOG_USER);

    memset(&act, 0, sizeof(act));
    act.sa_handler = signal_handler;

    if (sigaction(SIGINT, &act, NULL) == -1){
        syslog(LOG_ERR, "error");
        return -1;
    }
    if (sigaction(SIGTERM, &act, NULL) == -1){
        syslog(LOG_ERR, "error");
        return -1;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        syslog(LOG_ERR, "error");
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	{
        syslog(LOG_ERR, "error");
        return -1;
    }
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(USER_PORT);
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
	{
        syslog(LOG_ERR, "error");
        return -1;
    }
    if (is_daemon)
	{
        pid_t pid = fork();
        if (pid == -1)
		{
            syslog(LOG_ERR, "error");
            close(sockfd);
            return -1;
        }
        if (pid != 0)
            exit(0);

        pid_t sessionid = setsid();
        if (sessionid == -1)
		{
            syslog(LOG_ERR, "error");
            close(sockfd);
            return -1;
        }
        if (chdir("/") == -1)
		{
            syslog(LOG_ERR, "error");
            close(sockfd);
            return -1;
        }

        stdin_fd = open("/dev/null", O_RDONLY);
        stdouterr_fd = open("/dev/null", O_WRONLY);
        dup2(stdin_fd, STDIN_FILENO);
        dup2(stdouterr_fd, STDOUT_FILENO);
        dup2(stdouterr_fd, STDERR_FILENO);
        close(stdin_fd);
        close(stdouterr_fd);
    }

    if (listen(sockfd, MAX_CONNECTIONS) == -1)
	{
        syslog(LOG_ERR, "error");
        close(sockfd);
        return -1;
    }

    socket_file_fd = open(WRITE_TO_FILE, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (socket_file_fd == -1)
	{
        syslog(LOG_ERR, "error");
        return -1;
    }

    conn_count = 0;
    current_buffer_size = 0;
    while (1)
	{
        struct sockaddr_in connect_address;
        socklen_t addrlen = sizeof(connect_address);
        client_sockfd = accept(sockfd, (struct sockaddr*) &connect_address, &addrlen);
        if (client_sockfd == -1){
            syslog(LOG_ERR, "error");
            return -1;
        }

        conn_count++;

        syslog(LOG_INFO, "Accepted connection from %s\n", inet_ntoa(connect_address.sin_addr));
        syslog(LOG_INFO, "[%d] Identifier for accepted socket = %d\n", conn_count, client_sockfd);

        buffer_ptr = malloc(MAX_SIZE);
        if (buffer_ptr == NULL)
		{
            syslog(LOG_ERR, "error");
            gracefull_exit();
            return -1;
        }
        current_buffer_size = MAX_SIZE;
        got_newline = false;
        do{
            buffer_read_ptr = buffer_ptr + current_buffer_size - MAX_SIZE;
            nbytes_socket = recv(client_sockfd, buffer_read_ptr, MAX_SIZE, 0);
            if (nbytes_socket == -1)
			{
                syslog(LOG_ERR, "error");
                gracefull_exit();
                return -1;
            }

            syslog(LOG_INFO, "[%d] Received %li bytes from socket %d\n",conn_count, nbytes_socket, client_sockfd);

            int rc_sync = fsync(socket_file_fd);
            if (rc_sync == -1)
			{
                syslog(LOG_ERR, "error");
                gracefull_exit();
                return -1;
            }
            if (buffer_read_ptr[nbytes_socket-1] == '\n')
                got_newline = true;
            else
			{
                if (nbytes_socket == MAX_SIZE)
				{
                    syslog(LOG_INFO, "[%d] About to reallocate memory! ", conn_count);
                    syslog(LOG_INFO, "Current size = %li, ", current_buffer_size);
                    syslog(LOG_INFO, "New size = %li\n", current_buffer_size+MAX_SIZE);
                    char* bp_temp = realloc(buffer_ptr, current_buffer_size+MAX_SIZE);
                    if (bp_temp == NULL)
					{
                        syslog(LOG_ERR, "error");
                        gracefull_exit();
                        return -1;
                    }
                    buffer_ptr = bp_temp;
                    current_buffer_size += MAX_SIZE;
                }
            }
        } while(got_newline == false);

        syslog(LOG_INFO, "[%d] Found end of data symbol, closing connection.\n", conn_count);

        nbytes_file = write(socket_file_fd, buffer_ptr,
		nbytes_socket + current_buffer_size - MAX_SIZE);
        if (nbytes_file == -1)
		{
            syslog(LOG_ERR, "error");
            gracefull_exit();
            return -1;
        }
        lseek(socket_file_fd, 0, SEEK_SET); // Go to beginning of file
        do
		{
            nbytes_file = read(socket_file_fd, 
                buffer_ptr, current_buffer_size); // Read in up to current_buffer_size bytes
            if (nbytes_file == -1)
			{
                syslog(LOG_ERR, "Couldn't read from file %s\n", WRITE_TO_FILE);
                gracefull_exit();
                return -1;
            }
            syslog(LOG_INFO, "[%d] Got %li bytes from file\n", conn_count, nbytes_file);
            send(client_sockfd, buffer_ptr,
                nbytes_file, 0); // Send back nbytes_file to socket
        } while (nbytes_file > 0);
        close(client_sockfd);
        syslog(LOG_INFO, "Closed connection from %s\n", inet_ntoa(connect_address.sin_addr));
        free(buffer_ptr);
        buffer_ptr = NULL;
    }
    gracefull_exit();
    return 0;
}