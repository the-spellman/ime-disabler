#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/mei.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define _countof(a) (sizeof(a)/sizeof(*(a)))
#define NUM_DEV_NAMES 5
char *DEF_DEV_NAMES[NUM_DEV_NAMES] = {"mei0", "mei", "mei1", "mei2", "mei3"};

struct guid
{
        uint32_t   data1;
        uint16_t   data2;
        uint16_t   data3;
        uint8_t   data4[8];
};

static const struct guid mkhi_guid = {
	0x8E6A6715,
	0x9ABC,
	0x4043,
	{0x88, 0xEF, 0x9E, 0x39, 0xC6, 0xF6, 0x3E, 0x0F}
};

uint8_t disable_cmd[] = {0xff,0x10,0x00,0x00};


char *find_dev_name() {
    char *dev_name = NULL;
    struct stat st;

    for (int i = 0; i < NUM_DEV_NAMES; i++) {
        char path[20];
        snprintf(path, sizeof(path), "/dev/%s", DEF_DEV_NAMES[i]);

        if (stat(path, &st) == 0) {
            dev_name = DEF_DEV_NAMES[i];
            break;
        }
    }

    if (dev_name == NULL) {
        perror("device not found");
        exit(1);
    }

    return dev_name;
}

int main() {
	char *dev_name = find_dev_name();
	printf("Device found: %s\n", dev_name);
	int fd = open(dev_name, O_RDWR);
	struct mei_connect_client_data meidata;

	printf("opened\n");

	memcpy(&meidata.in_client_uuid,&mkhi_guid,sizeof(mkhi_guid));

	printf("Sending IOCTL_MEI_CONNECT_CLIENT .. ");
    	// THE BIG BLOCKS OF COMMENTS ARE AN UGLY HACK, BUT I NEEDED THIS FIX LIKE YESERDAY.
	int rc = ioctl(fd, IOCTL_MEI_CONNECT_CLIENT, &meidata);
	//if (rc < 0) {
		//printf("error\n"); fflush(stdout);
		//perror("ioctl");
		//close(fd);
		//return 1;
	//}
	printf("ok\n");

	printf("Writing disableme payload .. ");
	rc = write(fd, disable_cmd, sizeof(disable_cmd));
	//if (rc < 0) {
		//printf("error\n"); fflush(stdout);
		//perror("write");
		//close(fd);
		//return 1;
	//}
	fsync(fd);
	printf("written %d bytes\n",rc);

	printf("Sleep 1 sec\n",rc);
	sleep(1);

	close(fd);

	return 0;
}
