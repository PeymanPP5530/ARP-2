#include "./../include/processA_utilities.h"
#include <bmpfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>





/* the size (in bytes) of shared memory object */
const int SIZE = 1600*600*sizeof(int);

  bmpfile_t *bmp;
  
  rgb_pixel_t pixel = {80, 0, 0, 0};

  	/* name of the shared memory object */
	const char* name = "OS";

	// /* strings written to shared memory */
	// const char* message_0 = "Hello";
	// const char* message_1 = "World!";

	/* shared memory file descriptor */
	int shm_fd;

	/* pointer to shared memory object */
	int* ptr;

bool print_flag = false;



  int width = 1600;
  int height = 600;
  int depth = 4;
  void bitmap_creat(int circle_x, int circle_y);
  void update_shared_memory( bmpfile_t *bmp);

int main(int argc, char *argv[])
{

	/* create the shared memory object */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory object */
	ftruncate(shm_fd, SIZE);

	/* memory map the shared memory object */
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);


    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();
    bitmap_creat(circle.x, circle.y);

    // Infinite loop
    while (TRUE)
    {
        // Get input in non-blocking mode
        int cmd = getch();

        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }

        // Else, if user presses print button...
        else if(cmd == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(check_button_pressed(print_btn, &event)) {
                    //bitmap_creat(circle.x, circle.y);
                    print_flag = true;
                    bitmap_creat(circle.x, circle.y);
                    mvprintw(LINES - 1, 1, "Print button pressed");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {
            move_circle(cmd);
            draw_circle();
           bitmap_creat(circle.x, circle.y);
        }
    }
    
    endwin();
    return 0;
}


void update_shared_memory( bmpfile_t *bmp)
{

    for(int x = 0; x < 1600; x++)
    {
        for(int y = 0; y < 600; y++)
        {
         rgb_pixel_t *pixel=bmp_get_pixel(bmp ,x, y);
         ptr[x*599+y]= pixel->blue;

        }
    }

// rgb_pixel_t *pixel=bmp_get_pixel(bmp ,20, 20);
//  mvprintw(LINES - 1, 1, "blue=%d , red=%d , green=%d, alpha=%d",pixel->blue,pixel->red,pixel->green,pixel->alpha);

// sleep(5);
}


void bitmap_creat(int circle_x, int circle_y)
{
    bmp = bmp_create(width, height, depth);
    int radius =20;
    rgb_pixel_t pixel = {80, 0, 0, 0};
     for(int x = -radius; x <= radius; x++) 
     {
      for(int y = -radius; y <= radius; y++) 
      {
       // If distance is smaller, point is within the circle
         if(sqrt(x*x + y*y) < radius) {
          /*
          * Color the pixel at the specified (x,y) position
          * with the given pixel values
          */
          bmp_set_pixel(bmp, (circle_x)*(width/(COLS - BTN_SIZE_X -2)) + x, (circle_y)*(height/LINES) + y, pixel);
         }
       }
      }
if(print_flag == true)
 {
     bmp_save(bmp,"testpp" );
  print_flag = false;
 }
  update_shared_memory( bmp);
  bmp_destroy(bmp);
}