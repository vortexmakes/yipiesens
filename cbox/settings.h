/*
 * settings.h
 */
#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "scibrtbl.h"
#include "sleep.h"

/*
 * Version definition 
 */
#define CBSENS_VERSION			"CBSENS_V0.0"


/*
 * Serial Port baud rate
 */
#define BAUD_RATE_DFT			B9600

/*
 * Accelerometer full scale.
 */
#define ACCEL_FULL_SCALE_DFT	FULL_SCALE_8G

/*
 * Thershold for filtering in [ms] of grain noise
 * detections because pails overflowing and gaping.
 */
#define GRNOISE_THRESHOLD		3	

/*
 * Start / Stop Humedity sensor delay in [sec].
 */
#define HUMSENS_ON_DELAY		5
#define HUMSENS_OFF_DELAY		5

/*
 * Move Detection thershold and still delay [msec]
 */
#ifdef _MOVEDET_SIMPLE_AXIS_
#define MOVE_DET_THR			100 
#else
#define MOVE_DET_THR			300
#endif

#define MOVE_DET_DELAY			3000

#endif

