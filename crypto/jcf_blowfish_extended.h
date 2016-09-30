
/**
Provides an easier to use interface for JUCE Blowfish.

Allows for encrypting ValueTrees, MemoryBlocks and Strings. Almost anything can 
be stored in one of these.

Uses PKCS 5 padding.

SECURITY WARNINGS  

These functions make no attempt to stop filling your heap with copies of the 
decrypted data.  And are not designed to resist trivial memory dumping attack.  
Nor, does the JUCE BlowFish implementation may any attempt to clean up the key 
from memory.

In some circumstances random length padding may be a better choice, where 
information could be gleaned from the length of the output ciphertext.
*/
class BlowfishExtended
{
public:
	BlowfishExtended(const void* keyData, int keyBytes);

	/** Encrypt a ValueTree with blowfish */
	MemoryBlock encrypt(const ValueTree& data) const;

	/** Encrypt a string with blowfish */
	MemoryBlock encrypt(const String& string) const;

	/** Note that this will modify the original memory block in the process of decrypting. */
	Result decrypt(MemoryBlock& source, ValueTree& resultingTree) const;

	/** Decrypts a memory block to a string.  */
	Result decrypt(MemoryBlock& source, String& resultingString) const;

	/** 
	 Base function used by encrypt.  Can be used to encrypt a MemoryBlock if you 
	 happen to need to for some reason. 
	*/
	void encryptMemoryBlock(MemoryBlock& memoryBlock) const;

	Result decryptMemoryBlock(MemoryBlock& memoryBlock) const;

	/**
	Add PCKS5 padding to make data of arbitrary length up to an 8 byte boundary.

	Required prior to performing a blowfish encryption.

	See specification: https://tools.ietf.org/html/rfc2898#section-6.1.1 
	*/
	static void addPaddingPKCS5(MemoryBlock& memoryBlock);

	/**
	Removes PCKS5 padding from a memory block.

	Returns false if the data is invalid, too short or appears not to be PKCS 5 padding.
	*/
	static bool removePaddingPKCS5(MemoryBlock& memoryBlock);

	/** Utility function to add a string to a memory block */
	static void appendStringToMemoryBlock(const String& string, MemoryBlock& memoryBlock);
private:
	BlowFish blowFish;
};


