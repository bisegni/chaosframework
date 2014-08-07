package it.infn.chaos.mds.event;

class BitData {
	static void Put(byte Data[], final int BitOffset, int NumBits, final int Value) {
		final long valLong = (Value & ((1L << NumBits) - 1L));
		int posByte = BitOffset >> 3;
		int posBit = BitOffset & 7;
		int valByte;
		int ModifyBits;

		long lValue;
		int LeftShift;
		ModifyBits = 8 - posBit;
		if (NumBits < ModifyBits)
			ModifyBits = NumBits;
		LeftShift = (8 - posBit - ModifyBits);
		while (true) {
			valByte = Data[posByte];
			if (ModifyBits == 8) {
				lValue = valLong << (32 - NumBits) >> (24);
				Data[posByte] = (byte) lValue;
			} else {
				lValue = valLong << (32 - NumBits) >> (32 - ModifyBits) << LeftShift;
				Data[posByte] = (byte) ((valByte & ~(((1 << ModifyBits) - 1) << LeftShift)) | lValue);
			}
			NumBits -= ModifyBits;
			if (NumBits == 0)
				break;
			posByte++;
			ModifyBits = 8;
			if (NumBits < ModifyBits) {
				ModifyBits = NumBits;
				LeftShift = (8 - ModifyBits);
			}
		}
	}

	static int GetInt(byte Data[], final int BitOffset, int NumBits) {
		int posByte = BitOffset >> 3;
		int posBit = BitOffset & 7;

		long Value = 0;
		int ModifyBits;
		int valByte;
		int LeftShift;
		ModifyBits = 8 - posBit;
		if (NumBits < ModifyBits)
			ModifyBits = NumBits;
		LeftShift = (8 - posBit - ModifyBits);
		while (true) {
			valByte = Data[posByte] & 0xff;
			if (ModifyBits == 8)
				Value += valByte;
			else
				Value += (valByte & ((1 << ModifyBits) - 1) << LeftShift) >> LeftShift;
			NumBits -= ModifyBits;
			if (NumBits == 0)
				break;
			posByte++;
			ModifyBits = 8;
			if (NumBits < ModifyBits) {
				ModifyBits = NumBits;
				LeftShift = (8 - ModifyBits);
			}
			Value <<= ModifyBits;

		}
		return (int) Value;
	}
}