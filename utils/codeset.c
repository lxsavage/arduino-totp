/**
 * Writes in a new private key to the TOTP
 * Written by Logan Savage
 *
 * codeset <device> <base32 key>
 */
#include <stdio.h>
#include <signal.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int serial_fd;

int serial_open(char* port, speed_t baud) {
    struct termios tty;
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("fd");
        return -1;
    }

    int attr = tcgetattr(fd, &tty);
    if (attr < 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    // Set to RAW mode
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;
    if (tcsetattr(fd, TCSANOW, &tty)) {
        perror("tcsetattr RAW");
        close(fd);
        return -1;
    }

    return fd;
}

void handle_interrupt(int sig) {
    if (sig == SIGINT) {
        printf("Interrupt received; closing serial port...");
        tcdrain(serial_fd);
        close(serial_fd);
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s <DEVICE> <BASE32 CODE>\nFind device using `ls /dev/cu.*` or equivalent\n", argv[0]);
        return 1;
    }

    printf("Hold reset button...\n");

    serial_fd = serial_open(argv[1], B9600);
    if (serial_fd <= 0) return 1;

    // Setup interrupt handler to close the serial port
    struct sigaction sa;
    sa.sa_handler = handle_interrupt;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        close(serial_fd);
        return 1;
    }

    // Wait for restart; added extra time to allow for user to react to hold prompt
    sleep(5);
    printf("Release reset button\n");

    char msgbuf[256];
    size_t len = snprintf(msgbuf, 255, "%s\n", argv[2]);

    ssize_t written = write(serial_fd, msgbuf, len);
    if (written < 0) {
        perror("write");
        close(serial_fd);
        return 1;
    }

    tcdrain(serial_fd);
    printf("Sent new 2FA key\n");
    return 0;
}
