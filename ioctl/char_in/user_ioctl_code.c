#include "header.h"

#define WR_DATA _IOW('a','a',char*)
#define RD_DATA _IOR('a','b',char*)

int main()
{
        int fd;
        char num[100],val[100];
        printf("user space driver ioctl for communicating with ioctl driver\n");
        fd = open("/dev/my_device",O_RDWR);

        if(fd<0){
                printf("unable to open the file\n");
                return 0;
        }

        printf("enter data to send\n");
        scanf("%[^\t\n]s",num);
        printf("writing value to the ioctl driver\n");
        ioctl(fd,WR_DATA,(char *)&num);

        printf("read value from ioctl driver\n");
        ioctl(fd,RD_DATA,(char *)&val);
        printf("val data : %s\n",val);

        printf("closing the driver\n");
        close(fd);
}
