#ifndef ALTIMETER_DEFINITIONS_H_
#define ALTIMETER_DEFINITIONS_H_

// Register addresses.
enum
{
	ADD_STATUS,	// Sensor Status - alias for DR_STATUS or F_STATUS

	ADD_OUT_P_MSB, // Bits 12-19 of 20-bit Pressure Sample
	ADD_OUT_P_CSB, // Bits 4-11 of 20-bit Pressure sample
	ADD_OUT_P_LSB, // Bits 0-3 of 20-bit Pressure sample

	ADD_OUT_T_MSB, // Bits 4-11 of 12-bit Temperature sample
	ADD_OUT_T_LSB, // Bits 0-3 of 12-bit Temperature sample

	ADD_DR_STATUS, // Data Ready Status information

	ADD_OUT_P_DELTA_MSB, // Bits 12-19 of 20-bit Pressure change data
	ADD_OUT_P_DELTA_CSB, // Bits 4-11 of 20-bit Pressure change data
	ADD_OUT_P_DELTA_LSB, // Bits 0-3 of 20-bit Pressure change data

	ADD_OUT_T_DELTA_MSB, // Bits 4-11 of 12-bit Temperature change data
	ADD_OUT_T_DELTA_LSB, // Bits 0-3 of 12-bit Temperature change data

	ADD_WHO_AM_I_ALTI, // Device ID number (fixed)

	ADD_F_STATUS, // FIFO Status
	ADD_F_DATA, // FIFO 8-bit data access
	ADD_F_SETUP, // FIFO setup

	ADD_TIME_DLY, // Time since FIFO overflow

	ADD_SYSMOD, // Current system mode

	ADD_INT_SOURCE, // Interrupt status

	ADD_PT_DATA_CFG, // Data event flag configuration

	ADD_BAR_IN_MSB, // Barometric input for Altitude calculation bits 8-15
	ADD_BAR_IN_LSB, // Barometric input for Altitude calculation bits 0-7

	ADD_P_TGT_MSB, // Pressure/Altitude target value bits 8-15
	ADD_P_TGT_LSB, // Pressure/Altitude target value bits 0-7

	ADD_T_TGT, // Temperature target value

	ADD_P_WND_MSB, // Pressure/Altitude window value bits 8-15
	ADD_P_WND_LSB, // Pressure/Altitude window value bits 0-7

	ADD_T_WND, // Temperature window value

	ADD_P_MIN_MSB, // Minimum Pressure/Altitude bits 12-19
	ADD_P_MIN_CSB, // Minimum Pressure/Altitude bits 4-11
	ADD_P_MIN_LSB, // Minimum Pressure/Altitude bits 0-3

	ADD_T_MIN_MSB, // Minimum Temperature bits 8-15
	ADD_T_MIN_LSB, // Minimum Temperature bits 0-7

	ADD_P_MAX_MSB, // Maximum Pressure/Altitude bits 12-19
	ADD_P_MAx_CSB, // Maximum Pressure/Altitude bits 4-11
	ADD_P_MAX_LSB, // Maximum Pressure/Altitude bits 0-3

	ADD_T_MAX_MSB, // Maximum Temperature bits 8-15
	ADD_T_MAX_LSB, // Maximum Temperature bits 0-7

	ADD_CTRL_REG1, // Modes, Oversampling,
	ADD_CTRL_REG2, // Acquisition time step
	ADD_CTRL_REG3, // Interrupt pin configuration
	ADD_CTRL_REG4, // Interrupt enables
	ADD_CTRL_REG5, // Interrupt output pin assignment

	ADD_OFF_P, // Pressure data offset,
	ADD_OFF_T, // Temperature data offset,
	ADD_OFF_H // Altitude data offset,
};


// CTRL_REG1
#define CTRL_REG1_ALT 		BIT(7) // Altitude
#define CTRL_REG1_RAW 		BIT(6) // Raw measurement
#define CTRL_REG1_OS(n) 	((n & 0x7) << 3) // Oversampling
#define CTRL_REG1_RST 		BIT(2) // Reset
#define CTRL_REG1_OST 		BIT(1) // One shot mode
#define CTRL_REG1_SBYB 		BIT(0) // Standby

// CTRL_REG3
#define CTRL_REG3_IPOL1		BIT(5) // INT1 polarity
#define CTRL_REG3_PP_OD1 	BIT(4) // INT1 output type
#define CTRL_REG3_IPOL2 	BIT(1) // INT2 polarity
#define CTRL_REG3_PP_OD2 	BIT(0) // INT2 output type

// CTRL_REG4
#define CTRL_REG4_INT_EN_DRDY 	BIT(7) // Data ready
#define CTRL_REG4_INT_EN_FIFO 	BIT(6) // FIFO
#define CTRL_REG4_INT_EN_PW 	BIT(5) // Pressure window
#define CTRL_REG4_INT_EN_TW 	BIT(4) // Temperature window
#define CTRL_REG4_INT_EN_PTH 	BIT(3) // Pressure threshold
#define CTRL_REG4_INT_EN_TTH 	BIT(2) // Temperature threshold
#define CTRL_REG4_INT_EN_PCHG 	BIT(1) // Pressure change
#define CTRL_REG4_INT_EN_TCHG 	BIT(0) // Temperature change

// CTRL_REG5
// True to map to INT1, false to map to INT2
#define CTRL_REG5_INT_CFG_DRDY 	BIT(7) // Data ready
#define CTRL_REG5_INT_CFG_FIFO 	BIT(6) // FIFO
#define CTRL_REG5_INT_CFG_PW 	BIT(5) // Pressure window
#define CTRL_REG5_INT_CFG_TW 	BIT(4) // Temperature window
#define CTRL_REG5_INT_CFG_PTH 	BIT(3) // Pressure threshold
#define CTRL_REG5_INT_CFG_TTH 	BIT(2) // Temperature threshold
#define CTRL_REG5_INT_CFG_PCHG 	BIT(1) // Pressure change
#define CTRL_REG5_INT_CFG_TCHG 	BIT(0) // Temperature change

// PT_DATA_CGF
#define PT_DATA_CGF_DREM 	BIT(2) // Data ready event mode
#define PT_DATA_CGF_PDEFE 	BIT(1) // Data event on new Pressure/Altitude
#define PT_DATA_CGF_TDEFE 	BIT(0) // Data event on new Temperature

#endif /* ALTIMETER_DEFINITIONS_H_ */
