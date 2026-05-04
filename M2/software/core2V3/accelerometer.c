#include <stddef.h>
#include <math.h>
#include "sys/alt_stdio.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "system.h"
#include "hex.h"
#include "accelerometer.h"

/* shared globals from main.c */
//extern unsigned char hex_buf[6];
#define ACCELEROMETER_SPI_0_BASE 	0x00021098

static int pitch_deg;
static int roll_deg;
static const char *ori;

alt_up_accelerometer_spi_dev *accel;
alt_32 x_axis;
alt_32 y_axis;
alt_32 z_axis;

static int iabs_int(int v)
{
    return (v < 0) ? -v : v;
}

static const char *board_orientation(int x, int y, int z)
{
    int ax = iabs_int(x);
    int ay = iabs_int(y);
    int az = iabs_int(z);

    if (az >= ax && az >= ay)
    {
        return (z >= 0) ? "UP" : "DOWN";
    }
    else if (ax >= ay && ax >= az)
    {
        return (x >= 0) ? "LEFT" : "RIGHT";
    }
    else
    {
        return (y >= 0) ? "FWD" : "BACK";
    }
}

static void get_tilt_angles(int x, int y, int z, int *pitch_out, int *roll_out)
{
    double pitch;
    double roll;

    roll = atan2((double)y, (double)z) * 180.0 / 3.141592653589793;
    pitch = atan2((double)(-x),
                  sqrt((double)y * (double)y + (double)z * (double)z))
            * 180.0 / 3.141592653589793;

    *pitch_out = (int)pitch;
    *roll_out  = (int)roll;
}

void accelerometer_init(void)
{
    accel = alt_up_accelerometer_spi_open_dev(ACCELEROMETER_SPI_0_BASE);
    if (accel == NULL)
    {
        alt_putstr("ACCEL OPEN FAIL\n");
        while (1);
    }

    alt_putstr("ACCEL READY\n");
}

void accelerometer_main(int *pitch,int *roll,int *z)
{
	if ((alt_up_accelerometer_spi_read_x_axis(accel, &x_axis) == 0) &&
        (alt_up_accelerometer_spi_read_y_axis(accel, &y_axis) == 0) &&
        (alt_up_accelerometer_spi_read_z_axis(accel, &z_axis) == 0))
    {
        get_tilt_angles((int)x_axis, (int)y_axis, (int)z_axis, &pitch_deg, &roll_deg);
        ori = board_orientation((int)x_axis, (int)y_axis, (int)z_axis);

        alt_printf("X=%x Y=%x Z=%x\n", x_axis, y_axis, z_axis);
        alt_printf("P=%x R=%x\n", pitch_deg, roll_deg);
        alt_putstr("ORI=");
        alt_putstr(ori);
        alt_putstr("\n");

        hex_show_3_signed_values(pitch_deg, roll_deg, (int)z_axis);

        *pitch=pitch_deg;
        *roll=roll_deg;
        *z=z_axis;
    }
    else
    {
        alt_putstr("READ FAIL\n");
    }

    volatile int i;
    for (i = 0; i < 500; i++);
}
