#pragma once

/** A class for holding a pitch. */
class Pitch
{
public:
	Pitch() : freq(440.0) {}

	void setFrequency(double f);

	double getFrequency() { return freq; }

	/** Returns the midi note, including any fractional part */
	double getMidiNote();
	/** Returns the nearest midi note, rounded to an integer */
	int getNearestMidiNote();
	/** Returns any difference between the nearest midi note and the actual midi note
	in cents.  That is 0 is 'in tune' */
	int getErrorInCents();
	/** Constructs a string to represent the midinote */
	static String getMidiNoteAsString(int midinote);
	int getOctaveNumber();
	/** Sets the note frequency from a string like Cb0 or F#-1. If the
	octave isn't entered then it uses the same octave as the current note.
	Returns false if the note isn't recognised.  It will muddle through
	if the octave is specified wrong - selecting something close
	to the current pitch.
	*/
	bool setFrequencyFromNoteName(const String& note);

private:
	double freq;
};


