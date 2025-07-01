/*
 * config.h
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

//#define CALIBRATE_ACC
//#define CALIBRATE_GYR
//#define CALIBRATE_MAG

#if defined(CALIBRATE_ACC) || defined(CALIBRATE_GYR) || defined(CALIBRATE_MAG)
    #define CALIBRATE
#endif

#endif /* INC_CONFIG_H_ */
