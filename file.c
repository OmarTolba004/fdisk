#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include "file.h"

int main(int argc, char **argv)
{
	/* buffer for read and write opration*/
	uint8_t buf[512];
	/* Pointer of type Mbr*/
	Mbr *mymbr = (Mbr *)buf;

	/* Checking if number of arguments are correct*/
	if (argc != 2)
	{
		printf("Provide a hard disk file\n");
		return -2;
	}

	/* Openning the hard drive file*/
	int fd = open(argv[1], O_RDONLY);

	if (fd < 0)
	{ /* Couldn't open the file */
		printf("Could not open file: %s\n", argv[1]);
		return -1;
	}

	/* Reading first 512 bytes of hard disk*/
	int n = read(fd, buf, 512);

	if (n != 512)
	{ /* Reading operation failed*/
		printf("Could not read 512 byte\n");
		return -3;
	}

	/* Checking magical number for Mbr*/
	if (mymbr->magic != 0xAA55)
	{
		printf("Not an MBR(magic=0x%X)\n", mymbr->magic);
		return -4;
	}

	/* Printing mapped read data*/
	printf("Device     Boot    Start      Sectors  Id \n");
	for (int i = 0; i < 4; i++)
	{
		if (mymbr->partition_entries[i].start_sector == 0)
			continue;

		printf("%s%d %-5d %u %u %x\n",
			   argv[1],
			   i + 1,
			   (mymbr->partition_entries[i].status == 0x80) ? 1 : 0,
			   mymbr->partition_entries[i].start_sector,
			   mymbr->partition_entries[i].num_sectors,
			   mymbr->partition_entries[i].type);
	}

	for (int i = 0; i < 4; i++)
	{
		uint8_t buf[512];

		if (mymbr->partition_entries[i].type == 0x5)
		{
			/* Buffer to stroe EBR*/
			off_t extended_partition_start_sector = mymbr->partition_entries[i].start_sector;

			/* First Ebr of extended hardDisk*/
			Mbr *Ebr = (Mbr *)buf;

			/* seek opened file to the location of first Ebr of the first logical partition*/
			off_t offset = lseek(fd, (off_t)extended_partition_start_sector * 512, SEEK_SET);

			/* Reading 512 bytes of first Ebr*/
			int n = read(fd, buf, 512);

			if (n != 512)
			{ /* Reading operation failed*/
				printf("Could not read 512 byte\n");
				return ER_OPEN_FILE;
			}

			if (Ebr->magic != 0xaa55)
			{
				printf("Could reach the magic number of Ebr \n");
			}

			Mbr *current_Ebr = Ebr;

			while (current_Ebr->partition_entries[1].start_sector != 0)
			{

				/* Printing data of this Ebr*/

				printf("%s %-5d %u %u %x\n",
					   argv[1],
					   (current_Ebr->partition_entries[0].status == 0x80) ? 1 : 0,
					   current_Ebr->partition_entries[0].start_sector,
					   current_Ebr->partition_entries[0].num_sectors,
					   current_Ebr->partition_entries[0].type);

				/* Calculating the offset for the next Ebr*/
				off_t nextEbr_offset = (off_t)(extended_partition_start_sector + current_Ebr->partition_entries[1].start_sector) * 512;

				printf("extended partition start sector %d\n", extended_partition_start_sector);
				printf("second entry start sectro %d\n", current_Ebr->partition_entries[1].start_sector);
				printf("NextEbr sector is %ld\n", (off_t)(extended_partition_start_sector + current_Ebr->partition_entries[1].start_sector));
				/* seek opened file to the location of first Ebr of the first logical partition*/
				lseek(fd, (off_t)nextEbr_offset * 512, SEEK_SET);

				/* Reading 512 bytes of first Ebr*/
				n = read(fd, buf, 512);

				if (n != 512)
				{ /* Reading operation failed*/
					printf("Could not read 512 byte of next ebr\n");
					return ER_OPEN_FILE;
				}
				current_Ebr = (Mbr *)buf;
			}

			printf("%s %-5d %u %u %x\n",
				   argv[1],
				   (current_Ebr->partition_entries[0].status == 0x80) ? 1 : 0,
				   current_Ebr->partition_entries[0].start_sector,
				   current_Ebr->partition_entries[0].num_sectors,
				   current_Ebr->partition_entries[0].type);
		}
	}

	return 0;
}
