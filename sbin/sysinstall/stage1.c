/*
 * Copyright (c) 1994, Paul Richards.
 *
 * All rights reserved.
 *
 * This software may be used, modified, copied, distributed, and
 * sold, in both source and binary form provided that the above
 * copyright and these terms are retained, verbatim, as the first
 * lines of this file.  Under no circumstances is the author
 * responsible for the proper functioning of this software, nor does
 * the author assume any responsibility for damages incurred with
 * its use.
 */

#define DKTYPENAMES
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fstab.h>

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/devconf.h>
#include <ufs/ffs/fs.h>
#include <machine/console.h>

#include <dialog.h>

#include "disk.h"
#include "sysinstall.h"

unsigned char **options;
unsigned char *scratch;
unsigned char *errmsg;
unsigned char *bootblocks;

int dialog_active = 0;

/* Forward decls */
void exit_sysinstall(void);
void exit_prompt(void);
extern char *part_type(int);

char selection[30];

char *device_names[] = {"wd", "sd", "cd", "mcd", 0};
struct devconf *device_list[MAX_NO_DEVICES];
struct disk disk_list[MAX_NO_DEVICES];

int no_devices;
int no_disks;

int
alloc_memory()
{
    int i;

    scratch = (char *) calloc(SCRATCHSIZE, sizeof(char));
    if (!scratch)
	return(-1);

    errmsg = (char *) calloc(ERRMSGSIZE, sizeof(char));
    if (!errmsg)
	return(-1);
    
    options = (unsigned char **)calloc(MAX_NO_DISKS, sizeof(char *));
    if (!options)
	return(-1);
    for (i = 0; i < MAX_NO_DISKS; i++) {
	options[i] = (char *)calloc(100, sizeof(char));
	if (!options[i])
	    return(-1);
    }

    return(0);
}

void
free_memory()
{
	int i;

	free(scratch);
	free(errmsg);

	for (i = 0; i < MAX_NO_DEVICES; i++)
		free(device_list[i]);
}

int
query_devices()
{
	int i, j;
	int size, osize;
	int ndevs;
	int mib[8];
	struct devconf *dev;

	mib[0] = CTL_HW;
	mib[1] = HW_DEVCONF;
	mib[2] = DEVCONF_NUMBER;

	size = sizeof ndevs;
	if (sysctl(mib, 3, &ndevs, &size, 0, 0) < 0) {
		sprintf(errmsg, "Couldn't get device info from kernel\n");
		return(-1);
	}

	dev = 0; osize = 0;

	for (i=0; i <= ndevs; i++) {
		mib[2] = i;
		if (sysctl(mib, 3, 0, &size, 0, 0) < 0)
			/* Probably deleted device */
			continue;
		if (size > osize) {
			dev = realloc(dev, size);
			if (!dev) {
				sprintf(errmsg,
				        "Couldn't allocate memory for device information\n");
				return(-1);
			}
		}

		osize = size;

		if (sysctl(mib, 3, dev, &size, 0, 0) < 0) {
			sprintf(errmsg, "Error getting information on device %d\n", i);
			return(-1);
		}

		for (j=0; device_names[j]; j++)
			if (!strcmp(device_names[j], dev->dc_name)) {
				device_list[no_devices++] = dev;
				dev = 0; osize = 0;
				break;
			}
	}
	return (0);
}

int
configure_disks()
{
	char *disk_names[] = {"wd", "sd", 0};
	char diskname[20];
	int i, j;
	int fd;
	int valid=0;
	int choice;

	/* Search for possible installation targets */
	no_disks = 0;
	for (i=0; i < no_devices; i++)
		for (j=0; disk_names[j]; j++)
			if (!strcmp(disk_names[j], device_list[i]->dc_name)) {
				/* Try getting drive info */
				sprintf(diskname, "/dev/r%s%dd",
						  device_list[i]->dc_name,
						  device_list[i]->dc_unit);
				if ((fd = open(diskname, O_RDONLY)) == -1)
					continue;
				if (ioctl(fd, DIOCGDINFO, &disk_list[no_disks].lbl) == -1) {
					close(fd);
					continue;
				}
				disk_list[no_disks++].devconf = device_list[i];
				close(fd);
			}

	do {
		if (no_disks == 1)
			sprintf(scratch,"There is %d disk available for installation: ",
					  no_disks);
		else
			sprintf(scratch,"There are %d disks available for installation: ",
					  no_disks);

		for (i = 0; i < no_disks; i++) {
			sprintf(options[(i*2)], "%d",i+1);
			sprintf(options[(i*2)+1], "%s%d:  %s (%dMb)",
					  disk_list[i].devconf->dc_name,
					  disk_list[i].devconf->dc_unit,
					  disk_list[i].lbl.d_typename,
					  disk_size(&disk_list[i].lbl));
		}

		sprintf(options[no_disks*2], "%d", no_disks+1);
		sprintf(options[(no_disks*2)+1], "    Done");

		dialog_clear_norefresh();
		if (dialog_menu("FreeBSD Installation", scratch, -1, -1,
							 min(5, no_disks+1),  no_disks+1, options, selection)) {
			dialog_clear_norefresh();
			sprintf(scratch,"\nYou selected cancel.\n");
			AskAbort(scratch);
			valid = 0;
			continue;
		}
		choice = atoi(selection) - 1;
		if (choice == no_disks)
			valid = 1;
		else {
			if (edit_mbr(choice) == -1) {
				sprintf(scratch, "\nThe following error occured while\nediting the master boot record.\n%s", errmsg);
				AskAbort(scratch);
				valid = 0;
				continue;
			};
			if (edit_disklabel(choice) == -1) {
				sprintf(scratch, "\nThe following error occured while\nediting the disklabel.\n%s", errmsg);
				AskAbort(scratch);
				valid = 0;
				continue;
			}
			disk_list[choice].selected = 1;
		}
	} while (!valid);
}

int
stage1()
{
    int i, j;

	query_devices();
	configure_disks();
	/* List filesystems */
	for (i=0; i < MAX_NO_DEVICES; i++) {
		if (!disk_list[i].selected)
			continue;
		for (j=0; j < MAXPARTITIONS; j++) {
			if ((j == OURPART) || (j == RAWPART))
				continue;
			if (!disk_list[i].lbl.d_partitions[j].p_size)
				continue;
			mounts[no_mounts++] = &disk_list[i].mounts[j];
		}
	}
	return 0;
}
