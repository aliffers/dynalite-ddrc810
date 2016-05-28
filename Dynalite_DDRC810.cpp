#include <Wire.h>
#include <I2C_RL8xxM.h>

#include "Dynalite_DDRC810.h"

Dynalite_DDRC810::Dynalite_DDRC810 () {
	_id = 0;
	_relays = NULL;
	_area = NULL;
	_channel = NULL;
}

Dynalite_DDRC810::~Dynalite_DDRC810 () {
	if (_relays != NULL) delete _relays;
	if (_area != NULL) delete _area;
	if (_channel != NULL) delete _channel;
}

void Dynalite_DDRC810::begin (byte id, byte addr, void(*respond)(byte*), byte *area, short areas, byte *channel, short channels) {
	_id = id;
	_relays = new I2C_RL8xxM (addr);
	_respond = respond;
	_area = new byte[areas];
	memcpy (_area, area, areas);
	_channel = new byte[channels];
	memcpy (_channel, channel, channels);
	_preset = B11111111;
}

void Dynalite_DDRC810::process (byte *opcode) {
	switch (opcode[0]) {
		case 0x1C: process_logical (opcode); break;
		case 0x5C: process_physical (opcode); break;
	}
}

void Dynalite_DDRC810::process_physical (byte *opcode) {
	if (opcode[2] == _id && opcode[1] == 0x50 && opcode[3] == 0x92)
		_relays->Switch (opcode[4], opcode[5] != 0);
}

void Dynalite_DDRC810::process_logical (byte *opcode) {
	byte areamask = _area[opcode[1]];
	if (areamask > 0) {
		byte mask = areamask;
		switch (opcode[3]) {
			case 0x00: case 0x01: case 0x02: case 0x0A: case 0x0C: case 0x0D: _relays->OnMask (areamask); break; // select current preset
			case 0x03: case 0x04: _relays->OffMask (areamask); break; // 0x03 = preset 4 (off), 0x04 = set to off
			case 0x05: break; // decrement dimmer level
			case 0x06: break; // increment dimmer level
			case 0x08: break; // leave program mode
			case 0x0F: break; // reset preset
			case 0x15: break; // lock control panels
			case 0x17: break; // panic
			case 0x18: break; // unpanic
			case 0x20: break; // set area links
			case 0x21: break; // clear area links
			case 0x48: break; // preference
			case 0x60: break; // report channel level
			case 0x61: // request channel level
				   mask = areamask & _channel[opcode[2]];
				   if (mask > 0) {
					   byte level = ((mask & ~(_relays->ReadValue ())) > 0) ? 0x01 : 0xFF;
					   byte response[] = { 0x1C, opcode[1], opcode[2], 0x60, level, level, 0xFF, 0x00 };
					   _respond (response);
				   }
				   break;
			case 0x62: break; // report preset
			case 0x63: break; // request preset
			case 0x64: break; // preset offset
			case 0x66: break; // current preset
			case 0x67: break; // restore preset
			case 0x70: break; // toggle channel state
			case 0x71: case 0x72: case 0x73: // start fading to a level, 3 codes for different time frames, treat as equal with relays and ignore time
				   mask = areamask & _channel[opcode[2]];
				   if (mask > 0) {
					if (opcode[4] == 0xFF) _relays->OffMask (mask);
					else _relays->OnMask (mask);
				   }
				   break;
			case 0x76: break; // stop fading
			case 0x79: break; // start fading to a level
			case 0x7A: break; // stop fading to a level
			case 0x7D: break; // program toggle preset
		}
	}
}
