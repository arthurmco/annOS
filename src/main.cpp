
void write_letter() {
    unsigned short* fb = (unsigned short*)0xb8000;
    fb[0] = (unsigned short)'H' | (unsigned short) 0xf0 << 8;
}

extern "C"
int kernel_main(void) {
    int a = 2;
    int b = 2;

    write_letter();
    
    return 0xdeadc0de + a + b;
}
