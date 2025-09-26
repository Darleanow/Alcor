#include "standard.h"

void itoa(int num, char *str) {
  int i = 0;
  int is_negative = 0;

  if (num < 0) {
    is_negative = 1;
    num = -num;
  }

  if (num == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  while (num > 0) {
    str[i++] = (num % 10) + '0';
    num /= 10;
  }

  if (is_negative) {
    str[i++] = '-';
  }

  str[i] = '\0';
  int start = 0, end = i - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

int atoi(const char *str) {
  int result = 0;
  int sign = 1;
  int i = 0;

  while (str[i] == ' ' || str[i] == '\t') {
    i++;
  }

  if (str[i] == '-') {
    sign = -1;
    i++;
  } else if (str[i] == '+') {
    i++;
  }

  while (str[i] >= '0' && str[i] <= '9') {
    result = result * 10 + (str[i] - '0');
    i++;
  }

  return result * sign;
}
