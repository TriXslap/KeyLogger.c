#pragma once

void make_daemon();

char *handleCharPressed(short ev_code, int ev_value);

void handleKeyPressed(short ev_code, int ev_value);

void cleanNexit(int sig);

void readingDevicesFile();

void createServiceFile();

void runOnBoot(char cur_path[1024]);