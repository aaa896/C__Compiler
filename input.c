
int main(void) {

    int z = 1;
    for (int x = 10; x < 32; ++x) {
        for (int y =0 ; y < 3; ++y) {
            x += y;
        }
        

        int y = ~-7;
        x *=  y;
        x += 98;
        z = x;
    }

    return z;

}
