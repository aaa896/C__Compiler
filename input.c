
int main(void) {
//
    int y = 1;
    int x = 2;

    while (x < 3) {
        switch (x) {
            case 2:
                switch(y)  {
                    case 1:
                        for (int i = 0; i < 32; ++i) {
                            x += 4;
                        }
                        continue;
                    default:
                        break;
                }

                break;
            default:
                x = 23;
            case 0: {
                        x = 2;
                        break;
                    }


        }
    }
    return x;
}
