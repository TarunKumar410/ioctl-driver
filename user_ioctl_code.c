#include "header.h"

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int main()
{
        int fd;
        int num,val;
        printf("user space driver ioctl for communicating with ioctl driver\n");
        fd = open("/dev/my_device",O_RDWR);

        if(fd<0){
                printf("unable to open the file\n");
                return 0;
        }

        printf("enter data to send\n");
        scanf("%d",&num);
        printf("writing value to the ioctl driver\n");
        ioctl(fd,WR_DATA,(int32_t *)&num);

        printf("read value from ioctl driver\n");
        ioctl(fd,RD_DATA,(int32_t *)&val);
        printf("val data : %d\n",val);

        printf("closing the driver\n");
        close(fd);
}
