#include <Stream.h>
#include "TMC2208Stepper.h"
#include "TMC2208Stepper_MACROS.h"

//TMC2208Stepper::TMC2208Stepper(HardwareSerial& SR) : TMC_SERIAL(SR) {}
TMC2208Stepper::TMC2208Stepper(Stream * SR) {
	TMC_SERIAL = SR;
}

/*	
	Requested current = mA = I_rms/1000
	Equation for current:
	I_rms = (CS+1)/32 * V_fs/(R_sense+0.02ohm) * 1/sqrt(2)
	Solve for CS ->
	CS = 32*sqrt(2)*I_rms*(R_sense+0.02)/V_fs - 1
	
	Example:
	vsense = 0b0 -> V_fs = 0.325V
	mA = 1640mA = I_rms/1000 = 1.64A
	R_sense = 0.10 Ohm
	->
	CS = 32*sqrt(2)*1.64*(0.10+0.02)/0.325 - 1 = 26.4
	CS = 26
*/
void TMC2208Stepper::setCurrent(uint16_t mA, float Rsense, float multiplier) {
	uint8_t CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.325 - 1;
	
	// If Current Scale is too low, turn on high sensitivity R_sense and calculate again
	if (CS < 16) {
		vsense(true);
		CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.180 - 1;
	} else if(vsense()) { // If CS >= 16, turn off high_sense_r if it's currently ON
		vsense(false);
	}
	irun(CS);
	ihold(CS*multiplier);
	mA_val = mA;
}

uint8_t TMC2208Stepper::calcCRC(uint8_t datagram[], uint8_t len) {
	uint8_t crc = 0;
	for (uint8_t i = 0; i < len; i++) {
		uint8_t currentByte = datagram[i];
		for (uint8_t j = 0; j < 8; j++) {
			if ((crc >> 7) ^ (currentByte & 0x01)) {
				crc = (crc << 1) ^ 0x07;
			} else {
				crc = (crc << 1);
			}
			crc &= 0xff;
			currentByte = currentByte >> 1;
		} 
	}
	return crc;
}


void TMC2208Stepper::sendDatagram(uint8_t addr, uint32_t regVal, uint8_t len) {
	uint8_t datagram[] = {SYNC, SLAVE_ADDR, addr, (uint8_t)(regVal>>24), (uint8_t)(regVal>>16), (uint8_t)(regVal>>8), (uint8_t)(regVal>>0), 0x00};

	datagram[len] = calcCRC(datagram, len);

	for(int i=0; i<=len; i++){
		bytesWritten += TMC_SERIAL->write(datagram[i]);
	}
}

bool TMC2208Stepper::sendDatagram(uint8_t addr, uint32_t *data, uint8_t len) {
	uint8_t datagram[] = {SYNC, SLAVE_ADDR, addr, 0x00};
	datagram[len] = calcCRC(datagram, len);

	while (TMC_SERIAL->available() > 0) TMC_SERIAL->read(); // Flush

	for(int i=0; i<=len; i++) bytesWritten += TMC_SERIAL->write(datagram[i]);
	

	TMC_SERIAL->flush(); // Wait for TX to finish
	for(int byte=0; byte<4; byte++) TMC_SERIAL->read(); // Flush bytes written
	delay(1);

	uint64_t out = 0x00000000UL;
	while(TMC_SERIAL->available() > 0) {
		uint8_t res = TMC_SERIAL->read();
		out <<= 8;
		out |= res&0xFF;
	}
	uint8_t out_datagram[] = {(uint8_t)(out>>56), (uint8_t)(out>>48), (uint8_t)(out>>40), (uint8_t)(out>>32), (uint8_t)(out>>24), (uint8_t)(out>>16), (uint8_t)(out>>8), (uint8_t)(out>>0)};
	if (calcCRC(out_datagram, 7) == (uint8_t)(out&0xFF)) {
		*data = out>>8;
		return 0;
	} else {
		return 1;
	}
}

// GSTAT
bool TMC2208Stepper::GSTAT(uint32_t *data) { READ_REG(GSTAT); }
void TMC2208Stepper::GSTAT(uint32_t input) {
	GSTAT_sr = input;
	UPDATE_REG(GSTAT);
}

void TMC2208Stepper::reset(bool B)	{ MOD_REG(GSTAT, RESET); 	}
void TMC2208Stepper::drv_err(bool B){ MOD_REG(GSTAT, DRV_ERR); 	}
void TMC2208Stepper::uv_cp(bool B)	{ MOD_REG(GSTAT, UV_CP); 	}
bool TMC2208Stepper::reset()		{ GET_BYTE(GSTAT, RESET);	}
bool TMC2208Stepper::drv_err()		{ GET_BYTE(GSTAT, DRV_ERR);	}
bool TMC2208Stepper::uv_cp()		{ GET_BYTE(GSTAT, UV_CP);	}

// IFCNT
bool TMC2208Stepper::IFCNT(uint32_t *data) {
	bool b = sendDatagram(READ|REG_IFCNT, data);
	return b;
}

// SLAVECONF
void TMC2208Stepper::SLAVECONF(uint32_t input) {
	SLAVECONF_sr = input&SLAVECONF_bm;
	UPDATE_REG(SLAVECONF);
}
bool TMC2208Stepper::SLAVECONF(uint32_t *data) {
	data = &SLAVECONF_sr;
	return 1;
}
void TMC2208Stepper::senddelay(uint8_t B) 	{ MOD_REG(SLAVECONF, SENDDELAY);	}
uint8_t TMC2208Stepper::senddelay() 		{ GET_BYTE(SLAVECONF, SENDDELAY); 	}

// OTP_PROG
void TMC2208Stepper::OTP_PROG(uint32_t input) {
	OTP_PROG_sr = input;
	UPDATE_REG(OTP_PROG);
}

// IOIN
bool TMC2208Stepper::IOIN(uint32_t *data) {
	bool b = sendDatagram(READ|REG_IOIN, data);
	return b;
}
bool TMC2208Stepper::enn()			{ GET_BYTE_R(IOIN, ENN);		}
bool TMC2208Stepper::ms1()			{ GET_BYTE_R(IOIN, MS1);		}
bool TMC2208Stepper::ms2()			{ GET_BYTE_R(IOIN, MS2);		}
bool TMC2208Stepper::diag()			{ GET_BYTE_R(IOIN, DIAG);		}
bool TMC2208Stepper::pdn_uart()		{ GET_BYTE_R(IOIN, PDN_UART);	}
bool TMC2208Stepper::step()			{ GET_BYTE_R(IOIN, STEP);		}
bool TMC2208Stepper::sel_a()		{ GET_BYTE_R(IOIN, SEL_A);		}
bool TMC2208Stepper::dir()			{ GET_BYTE_R(IOIN, DIR);		}
uint8_t TMC2208Stepper::version() 	{ GET_BYTE_R(IOIN, VERSION);	}

// FACTORY_CONF
bool TMC2208Stepper::FACTORY_CONF(uint32_t *data) { READ_REG(FACTORY_CONF); }
void TMC2208Stepper::FACTORY_CONF(uint32_t input) {
	FACTORY_CONF_sr = input;
	UPDATE_REG(FACTORY_CONF);
}
void TMC2208Stepper::fclktrim(uint8_t B){ MOD_REG(FACTORY_CONF, FCLKTRIM);	}
void TMC2208Stepper::ottrim(uint8_t B)	{ MOD_REG(FACTORY_CONF, OTTRIM);	}
uint8_t TMC2208Stepper::fclktrim()		{ GET_BYTE(FACTORY_CONF, FCLKTRIM);	}
uint8_t TMC2208Stepper::ottrim()		{ GET_BYTE(FACTORY_CONF, OTTRIM);	}

// IHOLD_IRUN
void TMC2208Stepper::IHOLD_IRUN(uint32_t input) {
	IHOLD_IRUN_sr = input;
	UPDATE_REG(IHOLD_IRUN);
}
bool TMC2208Stepper::IHOLD_IRUN(uint32_t *data) {
	data = &IHOLD_IRUN_sr;
	return 1;
}
void TMC2208Stepper::ihold(uint8_t B) 	{ MOD_REG(IHOLD_IRUN, IHOLD);	}
void TMC2208Stepper::irun(uint8_t B)  	{ MOD_REG(IHOLD_IRUN, IRUN); 	}
uint8_t TMC2208Stepper::ihold() 		{ GET_BYTE(IHOLD_IRUN, IHOLD);	}
uint8_t TMC2208Stepper::irun()  		{ GET_BYTE(IHOLD_IRUN, IRUN); 	}

// TPOWERDOWN
void TMC2208Stepper::TPOWERDOWN(uint32_t input) {
	TPOWERDOWN_sr = input;
	UPDATE_REG(TPOWERDOWN);
}
bool TMC2208Stepper::TPOWERDOWN(uint32_t *data) {
	data = &TPOWERDOWN_sr;
	return 1;
}

// TSTEP
bool TMC2208Stepper::TSTEP(uint32_t *data) {
	bool b = sendDatagram(READ|REG_TSTEP, data);
	return b;
}

// TPWMTHRS
void TMC2208Stepper::TPWMTHRS(uint32_t input) {
	TPWMTHRS_sr = input;
	UPDATE_REG(TPWMTHRS);
}
bool TMC2208Stepper::TPWMTHRS(uint32_t *data) {
	data = &TPWMTHRS_sr;
	return 1;
}

// VACTUAL
void TMC2208Stepper::VACTUAL(uint32_t input) {
	VACTUAL_sr = input;
	UPDATE_REG(VACTUAL);
}
bool TMC2208Stepper::VACTUAL(uint32_t *data) {
	data = &VACTUAL_sr;
	return 1;
}

// MSCNT
bool TMC2208Stepper::MSCNT(uint32_t *data) {
	bool b = sendDatagram(READ|REG_MSCNT, data);
	return b;
}

// MSCURACT
bool TMC2208Stepper::MSCURACT(uint32_t *data) {
	bool b = sendDatagram(READ|REG_MSCURACT, data);
	return b;
}
uint16_t TMC2208Stepper::cur_a() { GET_BYTE_R(MSCURACT, CUR_A);	}
uint16_t TMC2208Stepper::cur_b() { GET_BYTE_R(MSCURACT, CUR_B);	}

// MSCNT
bool TMC2208Stepper::PWM_SCALE(uint32_t *data) {
	bool b = sendDatagram(READ|REG_PWM_SCALE, data);
	return b;
}
uint8_t TMC2208Stepper::pwm_scale_sum() { GET_BYTE_R(PWM_SCALE, PWM_SCALE_SUM); }
int16_t TMC2208Stepper::pwm_scale_auto() {
	// Not two's complement? 9nth bit determines sign
	uint32_t d;
	PWM_SCALE(&d);
	int16_t response = (d>>PWM_SCALE_AUTO_bp)&0xFF;
	if (((d&PWM_SCALE_AUTO_bm) >> 24) & 0x1) return -response;
	else return response;
}