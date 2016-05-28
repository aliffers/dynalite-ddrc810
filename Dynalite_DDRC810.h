#ifndef DYNALITE_DDRC810_H
#define DYNALITE_DDRC810_H

#include <Arduino.h>
#include <I2C_RL8xxM.h>

// Requires Wire library, and Wire must be initialised externally.

class Dynalite_DDRC810 {
	public:
		Dynalite_DDRC810 ();
		~Dynalite_DDRC810 ();
		void begin (byte, byte, void(*)(byte*), byte*, short, byte*, short);
		void process (byte*);
	private:
		byte _id;
		I2C_RL8xxM *_relays;
		byte *_area;
		byte *_channel;
		byte _preset;
		void (*_respond)(byte*);
		void process_logical (byte*);
		void process_physical (byte*);
};

#endif
