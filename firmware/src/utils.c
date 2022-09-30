void print_bytes(int bytes) {
  for (int i = 12; i >= 0; i--) {
    if (1 << i & bytes) {
      printf("1");
    } else {
      printf("0");
    }
  }
}