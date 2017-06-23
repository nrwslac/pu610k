#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<netinet/in.h>
#include<signal.h>
#include<string.h>
#include<fcntl.h>
#include<time.h>
#include<sys/select.h>

char *port = "50000";
char *flog = NULL;

int mode     = 0;
int admin    = 0;
int haserror = 0;
int voltage  = 0;
int curvolt  = 0;
int charging = 0;
int ci       = 0;
int cd       = 0;
int shots    = 0;
int ct       = 20;
int pf       = 7;
time_t  m2_time = 0;
time_t  ci_time = 0;
time_t  ch_time = 0;

pid_t pid[20];
int pidcnt = 0;

void childsig(int sig)
{
    int i;
    pid_t dead;
    while ((dead = waitpid(-1, &i, WNOHANG)) > 0) {
        printf("Process %d died.\n", dead);
        for (i = 0; i < pidcnt; i++) {
            if (pid[i] == dead) {
                pid[i] = pid[pidcnt - 1];
                printf("Reclaimed!\n");
                pidcnt--;
                break;
            }
        }
    }
}


void usr2sig(int sig)
{
    ci_time = time(0);
    shots++;
}

void usr1sig(int sig)
{
    haserror = !haserror;
    if (haserror) {
        mode = 4;
        charging = 0;
    }
}

void simulate(void)
{
    char buf[512];
    int len;
    int lastshot = shots;
    time_t now;

    signal(SIGUSR1, usr1sig);
    signal(SIGUSR2, usr2sig);

    if (flog) {
        int fd;
        sprintf(buf, flog, getpid());
        fd = open(buf, O_WRONLY|O_CREAT|O_APPEND, 0666);
        dup2(fd, 2);
        close(fd);
    }

    while (fgets(buf, 512, stdin) != NULL) {
        if (shots != lastshot) {
            fprintf(stderr, "FIRE!\n");
            lastshot = shots;
            charging = 0;
        }
        len = strlen(buf) - 1;
        if (buf[len] == '\n')
            buf[len--] = 0;
        if (buf[len] == '\r')
            buf[len--] = 0;
        if (buf[0] == 'E' && buf[1] == 'R') { /* Actually, assumes "ER," */
            if (haserror) {
                printf("\r\n:ER,13,008E,External interlock\r\n:");
                fprintf(stderr, "Simulated error\n");
            } else {
                printf("\r\n:ER,OK,000E\r\n:");
                fprintf(stderr, "No error\n");
            }
        } else  if (buf[0] == 'P' && buf[1] == 'W') {
            admin = !strcmp(buf, "PW,LASER");
            fprintf(stderr, "PW admin = %d\n", admin);
            printf("\r\n:PW\r\n:"); 
        } else if (buf[0] == 'M' && buf[1] == 'D') {
            if (buf[2] == ',') {
                int newmode = atoi(buf + 3);
                if (newmode < 0 || newmode > 2 || (newmode == 2 && mode == 0) || (mode == 4 && newmode != 0)) {
                    fprintf(stderr, "Illegal new mode: %s\n", buf);
                    printf("\r\n:MD,!\r\n:");
                } else {
                    mode = haserror ? 4 : newmode;
                    if (mode == 2) {
                        m2_time = time(0);
                        fprintf(stderr, "mode %d @%ld\n", mode, m2_time);
                    } else {
                        fprintf(stderr, "mode %d\n", mode);
                        charging = 0;
                    }
                    printf("\r\n:MD,%d\r\n:", mode);
                }
            } else {
                fprintf(stderr, "mode %d\n", mode);
                printf("\r\n:MD,%d\r\n:", mode);
            }
        } else if (buf[0] == 'C' && buf[1] == 'D') {
            if (buf[2] == ',') {
                int newval = atoi(buf + 3);
                if (newval < 1 || newval > 32767) {
                    fprintf(stderr, "Illegal new charge defer: %s\n", buf);
                    printf("\r\n:CD,!\r\n:");
                } else {
                    cd = newval;
                    printf("\r\n:CD,%d\r\n:", cd);
                    fprintf(stderr, "charge defer %d\n", cd);
                }
            } else {
                printf("\r\n:CD,%d,-1\r\n:", cd);
                fprintf(stderr, "charge defer %d\n", cd);
            }
        } else if (buf[0] == 'C' && buf[1] == 'I') {
            if (buf[2] == ',') {
                int newval = atoi(buf + 3);
                if (newval < 1 || newval > 32767) {
                    fprintf(stderr, "Illegal new charge inhibit: %s\n", buf);
                    printf("\r\n:CI,!\r\n:");
                } else {
                    ci = newval;
                    printf("\r\n:CI,%d\r\n:", ci);
                    fprintf(stderr, "charge inhibit %d\n", ci);
                }
            } else {
                now = time(0);
                len = now - ci_time;
                if (len > ci)
                    len = 0;
                else
                    len = ci - len;
                printf("\r\n:CI,%d,%d\r\n:", ci, len);
                fprintf(stderr, "charge inhibit countdown from %d: %d @%ld\n", ci, len, now);
            }
        } else if (buf[0] == 'C' && buf[1] == 'H') {
            if (buf[2] == ',') {
                now = time(0);
                if (mode != 2 || now - m2_time <= 5 || now - ci_time <= ci || buf[3] != '1') {
                    fprintf(stderr, "Cannot charge yet!\n");
                    printf("\r\n:CH,!\r\n:");
                } else {
                    charging = 1;
                    ch_time = now;
                    printf("\r\n:CH,1\r\n:");
                    fprintf(stderr, "Started charging @%ld\n", now);
                }
            } else {
                now = time(0);
                if (charging && now > ch_time + ct) {
                    printf("\r\n:CH,EOC1\r\n:");
                    fprintf(stderr, "Fully charged @%ld\n", now);
                } else {
                    printf("\r\n:CH,EOC0\r\n:");
                    fprintf(stderr, "Not fully charged.\n");
                }
            }
        } else if (buf[0] == 'V' && buf[1] == 'S') {
            if (buf[2] == ',') {
                voltage = atoi(buf + 3);
                printf("\r\n:VS,%d\r\n:", voltage);
                fprintf(stderr, "Set voltage to %d\n", voltage);
            } else {
                printf("\r\n:VS,%d\r\n:", voltage);
                fprintf(stderr, "Voltage is set to %d\n", voltage);
            }
        } else if (buf[0] == 'H' && buf[1] == 'V') {
            if (mode != 2) {
                printf("\r\n:HV! not flashing.\r\n:");
                fprintf(stderr, "HV not flashing!\n");
            } else if (charging == 1) {
                now = time(0);
                if (now > ch_time + ct) {
                    len = voltage + (random() % 11) - 5;
                } else {
                    len = voltage * (now - ch_time) / ct  + (random() % 11) - 5;
                }
                printf("\r\n:HV,%d\r\n:", len);
                fprintf(stderr, "HV is %d @%ld\n", len, now);
            } else {
                printf("\r\n:HV,%d\r\n:", 0);
                fprintf(stderr, "HV is %d @%ld\n", 0, now);
            }
        } else if (buf[0] == 'P' && buf[1] == 'F') {
            if (buf[2] == ',') {
                int newmode = atoi(buf + 3);
                if (newmode != 3 && newmode != 7) {
                    printf("\r\n:PF,!\r\n:");
                    fprintf(stderr, "Illegal flashing mode: %s\n", buf);
                } else {
                    pf = newmode;
                    printf("\r\n:PF,%d\r\n:", pf);
                    fprintf(stderr, "Flashing mode set to %d\n", pf);
                }
            } else {
                printf("\r\n:PF,%d\r\n:", pf);
                fprintf(stderr, "Flashing mode %d\n", pf);
            }
        } else if (buf[0] == 'R' && buf[1] == 'B') {
            admin = 0;
        } else if (buf[0] == 'V' && buf[1] == 'B') {
            printf("\r\n:%s\r\n:", buf);
        } else if (buf[0] == 'L' && buf[1] == 'E') {
            printf("\r\n:LE,%04x,0,008E,%x,0\r\n:", mode << 8, shots);
            fprintf(stderr, "Status mode %d, shots %d\n", mode, shots);
        } else {
            fprintf(stderr, "Unknown operation: %s\n", buf);
        }
        fflush(stdout);
        fflush(stderr);
    }
    fprintf(stderr, "Exiting!\n");
    exit(0);
}

void server(void)
{
    struct sockaddr_in serv_addr, cli_addr;
    int newfd, fd, portno, len, cnt;
    pid_t newpid;
    fd_set rd_set;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
   
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(port);
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
   
    if (bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    signal(SIGCHLD, childsig);
    signal(SIGUSR2, SIG_IGN);
      
    listen(fd, 5);
    for (;;) {
        FD_ZERO(&rd_set);
        FD_SET(0, &rd_set);
        FD_SET(fd, &rd_set);
        if ((cnt = select(fd+1, &rd_set, NULL, NULL, 0)) < 0) /* Took a signal! */
            continue;
        if (FD_ISSET(0, &rd_set)) {
            char buf[512];
            int i;
            int sig;
            printf("STDIN!\n");
            fgets(buf, 512, stdin);
            sig = (buf[0] == 'e') ? SIGUSR1 : SIGUSR2;
            for (i = 0; i < pidcnt; i++) {
                kill(pid[i], sig);
            }
        }
        if (FD_ISSET(fd, &rd_set)) {
            len = sizeof(cli_addr);
            newfd = accept(fd, (struct sockaddr *)&cli_addr, &len);
            printf("ACCEPT!\n");
            if (newfd < 0) {
                perror("ERROR on accept");
                exit(1);
            }
            if ((newpid = fork()) == 0) {
                dup2(newfd, 0);
                dup2(newfd, 1);
                simulate();
            } else {
                printf("Started pid %d\n", newpid);
                pid[pidcnt++] = newpid;
                close(newfd);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int i;

    for (i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--port")) {
            port = argv[++i];
        } else if (!strcmp(argv[i], "--log")) {
            flog = argv[++i];
        }
    }

    server();
}
