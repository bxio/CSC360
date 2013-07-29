#define __timer_t_defined
#include <stdio.h>
#include <errno.h>

#include "jcommon.h"

/*=====================
 * UVIC ROSE Addition
 *=====================
 */

int32 position = 0;
int32 disk[DISK_BLOCK_COUNT];

void DiskDelay(int32 timeout)
{
	UpdateVMTimer();
	long long vm_timeout = vm_time_ms + (long long)timeout;
	while (vm_time_ms < vm_timeout) UpdateVMTimer();
}

void DiskOpen(int32 startPosition)
{
	position = startPosition;
	int i;
	// Zero everything
	for (i = 0; i < DISK_BLOCK_COUNT; i++)
	{
		disk[i] = 0;
	}
}

void DiskClose()
{
	int i;
	// Print out debugging info
	for (i = 0; i < DISK_BLOCK_COUNT; i++)
	{
		printf("[%d] = %d\n", i, disk[i]);
	}
}

int32 DiskBlocks()
{
	return DISK_BLOCK_COUNT;
}

int32 DiskPosition()
{
	return position;
}

int32 DiskSeek(int32 block)
{
	DiskDelay(10 * abs(position - block));
	return position = block;
}

int32 DiskRead()
{
	DiskDelay(1);
	return disk[position];
}

void DiskWrite(int32 value)
{
	DiskDelay(1);
	disk[position] = value;
}

void DiskSave(int id)
{
	char buffer[300];
	if (sprintf(buffer, "vdisk_%03d", id) < 0)
	{
		fprintf(stderr, "Could not open file w/ id: [%d].\n", id);
		return;
	}
	FILE* fd = fopen(buffer, "w");
	if (fd != NULL)
	{
		int i;
		for (i = 0; i < DISK_BLOCK_COUNT; i++)
		{
			fprintf(fd, "[%d] - %d\n", i, disk[i]);
			if (ferror(fd))
			{
				fprintf(stderr, "Error writing to file.\n");
				fclose(fd);
				return;
			}
		}
		
		fclose(fd);
	}
	else
	{
		fprintf(stderr, "Error opening to disk for saving [%d].\n", errno);
	}
}

void DiskLoad(int id)
{
	char buffer[300];
	if (sprintf(buffer, "vdisk_%03d", id) < 0)
	{
		fprintf(stderr, "Could not open file w/ id: [%d].\n", id);
		return;
	}
	FILE* fd = fopen(buffer, "r");
	if (fd != NULL)
	{
		int i;
		int index = -1;
		int value = -1;
		for (i = 0; i < DISK_BLOCK_COUNT; i++)
		{
			fscanf(fd, "[%d] - %d\n", &index, &value);
			if (ferror(fd))
			{
				fprintf(stderr, "Error reading file.\n");
				fclose(fd);
				return;
			}
			disk[index] = value;
		}
		
		fclose(fd);
	}
	else
	{
		fprintf(stderr, "Error opening to disk for saving [%d].\n", errno);
	}
}
