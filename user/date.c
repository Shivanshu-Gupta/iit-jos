// factorial
#include <inc/lib.h>
#include <inc/x86.h>



void
umain(int argc, char **argv)
{
	struct rtcdate r;
	// if(argc < 2) {
	// 	cprintf("Usage : date\n");
	// 	exit();
	// }
	sys_date(&r);
	cprintf("Year: %d\nMonth: %d\nDay: %d\nHour: %d\nMinute :%d\nSecond: %d\n\n", r.year, r.month, r.day, r.hour, r.minute, r.second);
	exit();
}
