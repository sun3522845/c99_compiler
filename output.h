#ifndef _OUTPUT_H
#define _OUTPUT_H

extern void output_open(char *source_file_name);
extern void output_close();
extern void output(char *format, ...);

#endif
