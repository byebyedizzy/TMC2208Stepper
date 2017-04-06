#ifndef TMC2208Stepper_h
#define TMC2208Stepper_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#endif

#include "source/TMC2208Stepper_REGDEFS.h"


class TMC2208Stepper {
	public:
		//TMC2208Stepper(HardwareSerial& serial);
		TMC2208Stepper(Stream * serial);
		void setCurrent(uint16_t mA, float Rsense, float multiplier);
		// RW: GCONF
		void GCONF(uint32_t input);
		void I_scale_analog(bool B);
		void internal_Rsense(bool B);
		void en_spreadCycle(bool B);
		void shaft(bool B);
		void index_otpw(bool B);
		void index_step(bool B);
		void pdn_disable(bool B);
		void mstep_reg_select(bool B);
		void multistep_filt(bool B);
		bool GCONF(uint32_t *data);
		bool I_scale_analog();
		bool internal_Rsense();
		bool en_spreadCycle();
		bool shaft();
		bool index_otpw();
		bool index_step();
		bool pdn_disable();
		bool mstep_reg_select();
		bool multistep_filt();
		// R+WC: GSTAT
		void GSTAT(uint32_t input);
		void reset(bool B);
		void drv_err(bool B);
		void uv_cp(bool B);
		bool GSTAT(uint32_t *data);
		bool reset();
		bool drv_err();
		bool uv_cp();
		// R: IFCNT
		bool IFCNT(uint32_t *data);
		// W: SLAVECONF
		void SLAVECONF(uint32_t input);
		bool SLAVECONF(uint32_t *data);
		void senddelay(uint8_t B);
		uint8_t senddelay();
		// W: OTP_PROG
		void OTP_PROG(uint32_t input);
		// R: OTP_READ
		// R: IOIN
		bool IOIN(uint32_t *data);
		bool enn();
		bool ms1();
		bool ms2();
		bool diag();
		bool pdn_uart();
		bool step();
		bool sel_a();
		bool dir();
		uint8_t version();
		// RW: FACTORY_CONF
		void FACTORY_CONF(uint32_t input);
		bool FACTORY_CONF(uint32_t *data);
		void fclktrim(uint8_t B);
		void ottrim(uint8_t B);
		uint8_t fclktrim();
		uint8_t ottrim();
		// W: IHOLD_IRUN
		void IHOLD_IRUN(uint32_t input);
		bool IHOLD_IRUN(uint32_t *data);
		void ihold(uint8_t B);
		void irun(uint8_t B);
		uint8_t ihold();
		uint8_t irun();
		// W: TPOWERDOWN
		void TPOWERDOWN(uint32_t input);
		bool TPOWERDOWN(uint32_t *data);
		// R: TSTEP
		bool TSTEP(uint32_t *data);
		// W: TPWMTHRS
		void TPWMTHRS(uint32_t input);
		bool TPWMTHRS(uint32_t *data);
		// W: VACTUAL
		void VACTUAL(uint32_t input);
		bool VACTUAL(uint32_t *data);
		// R: MSCNT
		bool MSCNT(uint32_t *data);
		// R: MSCURACT
		bool MSCURACT(uint32_t *data);
		uint16_t cur_a();
		uint16_t cur_b();
		// RW: CHOPCONF
		void CHOPCONF(uint32_t input);
		void toff(uint8_t B);
		void hstrt(uint8_t B);
		void hend(uint8_t B);
		void tbl(uint8_t B);
		void vsense(bool B);
		void mres(uint8_t B);
		void intpol(bool B);
		void dedge(bool B);
		void diss2g(bool B);
		void diss2vs(bool B);
		bool CHOPCONF(uint32_t *data);
		uint8_t toff();
		uint8_t hstrt();
		uint8_t hend();
		uint8_t tbl();
		bool vsense();
		uint8_t mres();
		bool intpol();
		bool dedge();
		bool diss2g();
		bool diss2vs();
		// R: DRV_STATUS
		bool DRV_STATUS(uint32_t *data);
		bool otpw();
		bool ot();
		bool s2ga();
		bool s2gb();
		bool s2vsa();
		bool s2vsb();
		bool ola();
		bool olb();
		bool t120();
		bool t143();
		bool t150();
		bool t157();
		bool stealth();
		bool stst();
		// RW: PWMCONF
		void PWMCONF(uint32_t input);
		void pwm_ofs(uint8_t B);
		void pwm_grad(uint8_t B);
		void pwm_freq(uint8_t B);
		void pwm_autoscale(bool B);
		void pwm_autograd(bool B);
		void freewheel(uint8_t B);
		void pwm_reg(uint8_t B);
		void pwm_lim(uint8_t B);
		bool PWMCONF(uint32_t *data);
		uint8_t pwm_ofs();
		uint8_t pwm_grad();
		uint8_t pwm_freq();
		bool pwm_autoscale();
		bool pwm_autograd();
		uint8_t freewheel();
		uint8_t pwm_reg();
		uint8_t pwm_lim();
		// R: PWM_SCALE
		bool PWM_SCALE(uint32_t *data);
		uint8_t pwm_scale_sum();
		int16_t pwm_scale_auto();

		uint16_t bytesWritten = 0;
	private:
		Stream * TMC_SERIAL;
		void sendDatagram(uint8_t addr, uint32_t regVal, uint8_t len=7);
		bool sendDatagram(uint8_t addr, uint32_t *data, uint8_t len=3);
		uint8_t calcCRC(uint8_t datagram[], uint8_t len);
		// Shadow registers
		uint32_t 	GCONF_sr = 			0x00000000UL,
					GSTAT_sr = 			0x00000000UL,
					SLAVECONF_sr = 		0x00000000UL,
					OTP_PROG_sr = 		0x00000000UL,
					FACTORY_CONF_sr = 	0x00000000UL,
					IHOLD_IRUN_sr = 	0x00000000UL,
					TPOWERDOWN_sr = 	0x00000000UL,
					TPWMTHRS_sr = 		0x00000000UL,
					VACTUAL_sr = 		0x00000000UL,
					CHOPCONF_sr = 		0x00000000UL,
					PWMCONF_sr = 		0x00000000UL,
					tmp_sr = 			0x00000000UL;

		uint32_t mA_val = 0;
};

#endif