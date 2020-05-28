#include "accel.h"
#include "movdet.h"
#include "derivative.h"
#include "iic.h"
#include "ioports.h"
#include "sleep.h"
#include "reset.h"
#include "formats.h"
#include "system.h"
#include "mma845x.h"
#include "watchdog.h"
#include "settings.h"
#include "intr.h"


#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

byte SlaveAddressIIC;     // accelerometer slave I2C address
byte value[6];             // working value result scratchpad

static uchar accel_present = 0;

#pragma DATA_SEG DEFAULT


void
init_accelerometer( void )
{
	byte dev_id;
	/*
	**  Verify IIC communications with the accelerometer
	*/
	SlaveAddressIIC = MMA845x_IIC_ADDRESS;
	if( SA0_PIN == 1 )
		SlaveAddressIIC += 2;

	/*
	**  delay for about 10ms
	*/
	sleep( 10/SLEEP_BASE );

	/*
	**  Identify the accelerometer
	*/
	dev_id = IIC_RegRead(SlaveAddressIIC, WHO_AM_I_REG);
	if( dev_id != MMA8451Q_ID )
	{
		accel_present = 0;
		return;
	}

	accel_present = 1;
	/*
	**  MMA8451 recognized
	*/
	MMA845x_Init( ACCEL_FULL_SCALE_DFT );
	MMA845x_Active();
}

void
dump_accel_regs( void )
{
	uchar i;

	if( !accel_present )
		return;

	for( i = 0; i < 0x40; ++i )
		dprintf(( 0, "add%02x:reg%02x:%x\n", SlaveAddressIIC, i, IIC_RegRead(SlaveAddressIIC, i )));
}


static tword x,y,z;
static unsigned short xf=0,yf=0,zf=0;

void
accel_lpfilter( tword in, unsigned short *out )
{
	unsigned short temp;

	if( in.Byte.hi > 0x1F )
	{
		temp = (~in.Word + 1) & 0x3FFF;
		if( temp > ACCEL1G_RES )
			temp = ACCEL1G_RES;

		temp += ACCEL1G_RES;	// sumo 1G para angulos negativos

	}
	else
	{
		temp = ((in.Word /*>> 2*/) & 0x3FFF);
		if( temp > ACCEL1G_RES )
			temp = ACCEL1G_RES;

		temp = ACCEL1G_RES - temp;
	}

	temp = (temp>>3)&0x7FF;
#define __FILTER__
#ifdef __FILTER__
	*out += (( temp - *out )  >> 3 );   /* Low-Pass-Filter */
	*out &= 0x7FF;
#else
	*out = temp;
#endif
}

void
proc_accel( void )
{
	byte RegisterFlag;   // temporary accelerometer register variable

	if( !accel_present )
		return;

	RegisterFlag = IIC_RegRead(SlaveAddressIIC, STATUS_00_REG);
	if( (RegisterFlag & ZYXDR_MASK) == 0 )
		return;

	IIC_RegReadN(SlaveAddressIIC, OUT_X_MSB_REG, 6, &value[0]);

	x.Byte.hi = value[0];
	x.Byte.lo = value[1];
	x.Word >>= 2;

	y.Byte.hi = value[2];
	y.Byte.lo = value[3];
	y.Word >>= 2;

	z.Byte.hi = value[4];
	z.Byte.lo = value[5];
	z.Word >>= 2;

	accel_lpfilter( x, &xf );
	accel_lpfilter( y, &yf );
	accel_lpfilter( z, &zf );

#ifdef _MOVEDET_SIMPLE_AXIS_
	proc_accel_movdet( x.Word );
#else
	proc_accel_movdet( x.Word, y.Word, z.Word );
#endif
}

void
get_accel_data( ACCEL_T *p )
{
	if( !accel_present )
	{
		p->x = 0xEEEE;
		p->y = 0xEEEE;
		p->z = 0xEEEE;
		return;
	}
	p->x = xf;
	p->y = yf;
	p->z = zf;
}

void
show_accel_data( void )
{
	static unsigned short sxf,syf,szf;

	enter_critical();
	sxf = xf;
	syf = yf;
	szf = zf;
	exit_critical();

	mprintf(0,"x:%u y:%u z:%u\r\n",sxf,syf,szf);
}

void
accel_calibrate( void )
{
	static tword x_value;	// 16-bit X accelerometer value
	static tword y_value;	// 16-bit Y accelerometer value
	static tword z_value;	// 16-bit Z accelerometer value
	static signed int X_cal=0;
	static signed int Y_cal=0;
	static signed int Z_cal=0;

	if( !accel_present )
		return;
	/*
	**  Verify IIC communications with the accelerometer
	*/
	SlaveAddressIIC = MMA845x_IIC_ADDRESS;
	if (SA0_PIN == 1)
		SlaveAddressIIC += 2;

	MMA845x_Standby();
	/*
	** Configure sensor for:
	** - Sleep Mode Poll Rate of 50Hz (20ms)
	** - Low Noise Bit Set
	** - System Output Data Rate of 1.56Hz (640ms)
	** - Full Scale of +/-2g
	** - High Resolution Mode MODS1 bit set
	*/
	IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, ASLP_RATE_20MS+LNOISE_MASK+DATA_RATE_640MS);
	IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, FULL_SCALE_2G);
	IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, MODS1_MASK);
	/*
	** Set the Calibration variables to zero
	*/
	/*
	** Clear the Calibration Register
	*/
	IIC_RegWrite(SlaveAddressIIC, OFF_X_REG, 0x00);
	IIC_RegWrite(SlaveAddressIIC, OFF_Y_REG, 0x00);
	IIC_RegWrite(SlaveAddressIIC, OFF_Z_REG, 0x00);
	/*
	** Go into the Active Mode
	*/
	MMA845x_Active();
	/*
	** Delay time to change from Standby to Active before taking a reading
	** at 1.56Hz (2/ODR + 1ms delay timing)
	**/
	sleep(1300/SLEEP_BASE);
	IIC_RegReadN(SlaveAddressIIC, OUT_X_MSB_REG, 6, &value[0]);

	x_value.Byte.hi = value[0];
	x_value.Byte.lo = value[1];
	y_value.Byte.hi = value[2];
	y_value.Byte.lo = value[3];
	z_value.Byte.hi = value[4];
	z_value.Byte.lo = value[5];

	dprintf((0,"x:%04x\t y:%04x\t z:%04x\n",x_value.Word,y_value.Word,z_value.Word));

	/**
	** Calculations for the 14-bit MMA8451Q device calibration
	*/

	if( x_value.Byte.hi > 0x7F )
	{
		x_value.Word= (~x_value.Word +1)>>2;
		X_cal= X_cal + x_value.Word/8;
	}
	else
	{
		X_cal= X_cal + ((~x_value.Word+1)>>2)/8;
	}
	if( y_value.Byte.hi > 0x7F )
	{
		y_value.Word= (~y_value.Word +1)>>2;
		Y_cal= Y_cal + y_value.Word/8;
	}
	else
	{
		Y_cal= Y_cal + ((~y_value.Word+1)>>2)/8;
	}
	if( z_value.Byte.hi > 0x7F )
	{
		z_value.Word= (~z_value.Word +1)>>2;
		Z_cal+=(4096 + z_value.Word)/8;
	}
	else
	{
		Z_cal+=(int)(4096- (z_value.Word>>2))/8;
		if( Z_cal < 0 )
		{
			Z_cal+=256;
		}
	}

	dprintf((0,"Calibration values x:%05u\t y:%05u\t z:%05u\n",X_cal,Y_cal,Z_cal));

	/*
	** Go to Standby mode and write the Calibration values into the registers
	*/
	MMA845x_Standby();
	IIC_RegWrite( SlaveAddressIIC, OFF_X_REG, (byte)X_cal );
	IIC_RegWrite( SlaveAddressIIC, OFF_Y_REG, (byte)Y_cal );
	IIC_RegWrite( SlaveAddressIIC, OFF_Z_REG, (byte)Z_cal );
	MMA845x_Active();
}
