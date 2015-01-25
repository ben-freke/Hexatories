#ifndef __LOG_H
#define __LOG_H
	
	/*
		Restarts the log - clears the file and prints time and date at the top.
	*/
	void restart_log();
	/*
		Log a message in the same style as printf.
		usage:
		log("Number: %d\n", number);
	*/
	void log(const char* message, ...);
#endif