#include <stdio.h>
#include <stdlib.h>

#include <spnav.h>

int main(void)
{
	spnav_event sev;

	if(spnav_open()!=-1) {
	  spnav_close();
	}

	return 0;
}

