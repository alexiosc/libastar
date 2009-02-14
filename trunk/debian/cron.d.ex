#
# Regular cron jobs for the libastar package
#
0 4	* * *	root	[ -x /usr/bin/libastar_maintenance ] && /usr/bin/libastar_maintenance
