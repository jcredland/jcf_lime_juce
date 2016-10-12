#pragma once

/** A class for holding a pitch. */
class Pitch
{
public:
	Pitch() : freq(440.0) {}

	void setFrequency(double f) { freq = f; }

	double getFrequency() const { return freq; }

	/** Returns the midi note, including any fractional part */
	double getMidiNote() const;

	/** Returns the nearest midi note, rounded to an integer */
	int getNearestMidiNote() const;

	/** Returns any difference between the nearest midi note and the actual midi note
	in cents.  That is 0 is 'in tune' */

	int getErrorInCents() const;

	/** Constructs a string to represent the midinote */
	static String getMidiNoteAsString(int midinote);

	int getOctaveNumber() const;

	/** Sets the note frequency from a string like Cb0 or F#-1. If the
	octave isn't entered then it uses the same octave as the current note.
	Returns false if the note isn't recognised.  It will muddle through
	if the octave is specified wrong - selecting something close
	to the current pitch.
	*/
	bool setFrequencyFromNoteName(const String& note);

private:
	static constexpr int offsetMiddleC = 2; // set to 1 for C4 = midi note 60
	double freq;
};


