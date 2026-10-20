
int fun(int a, int b, int c, int d, int e, int f, int g, int h) ;



int main(void) {
  int x = 1;
  x = fun(x, 2,3,4,5,6,7,8);
  return x;
}

int fun(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + h;
}

