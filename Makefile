CC=gcc
SRCS=capture_video.c powerdown_camcorder.c download_memory.c toggle_camera_lcd_screen.c delete_file.c upload_file.c download_file.c close_camcorder.c download_all_movies.c download_last_movie.c format_camcorder.c messagebox.c open_camcorder.c ops-linux.c readwrite.c unlock_camcorder.c send_monitor_command.c update_directory_listing.c
OBJS=capture_video.o powerdown_camcorder.o download_memory.o toggle_camera_lcd_screen.o delete_file.o upload_file.o download_file.o close_camcorder.o download_all_movies.o download_last_movie.o format_camcorder.o messagebox.o open_camcorder.o ops-linux.o readwrite.o unlock_camcorder.o send_monitor_command.o update_directory_listing.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wcast-align -Wredundant-decls
LDFLAGS=`pkg-config --libs gtk+-2.0` -lgthread-2.0 -W -Wall -Wl,-warn-common
all: ops-linux

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

ops-linux: $(OBJS)
	$(CC) -lusb $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm ops-linux *~ *.o -f
