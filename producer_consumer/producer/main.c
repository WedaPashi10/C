#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

float get_alpha(void)
{
    float a = 100.20;
    return a;
}

float get_Pi(void)
{
    float var = 3.14159;
    float a = get_alpha() + var;
    return a;
}

int main(void)
{
    float fl_var = 10.20;
    uint8_t u8ch = 'A';

    printf("%u, %Xh, %c\n", u8ch, u8ch, u8ch);
    u8ch = u8ch;
    
    fprintf(stdout, "In %s file, at %d line", __FILE__, __LINE__);
    //fl_var = get_Pi();
    printf("\n%.4f", fl_var);
    fprintf(stdout, "\n\n");
    return EXIT_SUCCESS;
}
