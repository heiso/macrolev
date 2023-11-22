#include <stdint.h>
#include <stdio.h>

static int read = 0;
static int write = 0;
#define REPORT_SIZE 6
static int queue[REPORT_SIZE];

void queue_report(int value) {
  queue[write] = value;

  write = (write + 1) % REPORT_SIZE;

  if (write == read) {
    read = (read + 1) % REPORT_SIZE;
  }

  printf("added %d, read %d, write %d\n", value, read, write);
}

int has_reports() {
  printf("has_report %d\n", read != write);
  return read != write;
}

int consume_report() {
  int rtn = read;

  read = (read + 1) % REPORT_SIZE;

  printf("consumed index %d, value %d, read %d, write %d\n", rtn, queue[rtn],
         read, write);
  return rtn;
}

int main(void) {
  queue_report(10);
  queue_report(20);
  queue_report(30);
  queue_report(40);
  queue_report(50);
  queue_report(60);
  queue_report(70);

  while (has_reports()) {
    int index = consume_report();
  }
}