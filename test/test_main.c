#include <stdio.h>
#define MIN(a,b) ((a)<(b)?(a):(b))

int compare_16(char * buffer_1,char * buffer_2,int length)
{
    int i;
    for (i=0;i<length;i++)
    {
        if ((int)buffer_1[i]!=(int)buffer_2[i])
        {
            printf("no equal");
            return 1;
        }
    }
    return 0;
}

int main() {
    int num = 0x34ff;
    char *p = (char *)&num;
    char temp;
    char plus_hex[] = {0x00,0xff,0x07};
    char sub_hex[] = {0x00,0xff,0x15};
    temp = p[0];
    p[0] = p[1];
    p[1] = temp;
    num  = num >> 5;
    int i = 30;
    int length = MIN(sizeof(plus_hex), sizeof(sub_hex));
    compare_16(plus_hex, sub_hex,length);
    printf("plus_hex is %2x",(int)plus_hex[1]);
    // 打印每个字节的值
    printf("p[0] = 0x%02x (十进制值是 %d)\n", p[0], p[0] & 0xFF);
    printf("p[1] = 0x%02x (十进制值是 %d)\n", p[1] & 0xFF, p[1] & 0xFF);
    printf("num is %02x\n",num);
    if (*(p+1) & 0x04)
        printf("negative\n");
    return 0;
    
}
