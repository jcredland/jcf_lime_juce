
class KeyData
{
public:
	virtual ~KeyData() {}

	/** Generate and return the key. */
	virtual uint8 * getKeyData(int * keyLength) = 0;
	/** Clear the complete key data from memory. */
	virtual void clear() = 0;
};

/**
Creates a 72 byte key by combining local key data with a fixed application key.

This makes it fractionally harder to write a tool that decodes a locally encrypted 
file.
 */
class LocalKeyData
	:
	public KeyData
{
public:
	enum
	{
		APP_KEY_LEN = 24,
		LOCAL_KEY_LEN = 24
	};

	LocalKeyData(
		const uint8 * fixedKeyDataLen24,
		const File & localKeyDataLen32)
		:
		localKeyDataLen32(localKeyDataLen32),
		fixedKeyDataLen24(fixedKeyDataLen24)
	{
		key.reserve(72);
	}

	uint8* getKeyData(int* keyLength) override
	{
		generateKey();

		if (keyLength != nullptr)
			*keyLength = int(key.size());

		return key.data();
	}

	void generateKey()
	{
		key.clear();

		if (!localKeyDataLen32.existsAsFile())
		{
			std::random_device randomDevice;
			std::uniform_int_distribution<int> distribution(0, 255);

			for (int i = 0; i < LOCAL_KEY_LEN; ++i)
				key.push_back(uint8(distribution(randomDevice)));

			localKeyDataLen32.replaceWithData(key.data(), key.size());
		}
		else
		{
			MemoryBlock block;
			localKeyDataLen32.loadFileAsData(block);

			for (int i = 0; i < block.getSize(); ++i)
				key.push_back(*(static_cast<uint8*>(block.getData()) + i));
		}

		for (int i = 0; i < APP_KEY_LEN; ++i)
			key.push_back(fixedKeyDataLen24[i]);
	}

	void clear()
	{
		for (auto & v : key)
			v = 0;

		key.clear();
	}
private:
	std::vector<uint8> key;
	File localKeyDataLen32;
	const uint8* fixedKeyDataLen24;
};

/** 
A SecureCredentials object stores a username and a password.  It will eventually
make use of whatever the most practical secure password storage mechanism is on 
a particular platform.  But right now it'll use a BlowFish encrypted file using 
whatever key is provided for the encryption.
*/

class StoredCredentials
{
public:
	/**
	Creates a StoredCredentials object.  On a platform that doesn't have a secure
	solution for password storage the fileOfLastResort will be used, and will be
	encrypted using the provided key.

	The provided key should be the same each time.  There is no really good way of
	managing the provided encryption key - if you embed it in the app it can be
	reverse engineered and if you store it in a file it can be reverse engineered.

	@param serviceName Some name you can use to later retrieve the correct credentials.
	*/
	StoredCredentials(
		const File & fileOfLastResort,
		const String & serviceName,
		KeyData * keyData, bool deleteKeyDataWhenFinished)
		:
		serviceName(serviceName),
		file(fileOfLastResort), keyData(keyData), deleteKeyData(deleteKeyDataWhenFinished)
	{
		load();
	}

	~StoredCredentials()
	{
		if (deleteKeyData)
			delete keyData;
	}

	struct Credentials
	{
		String username;
		String password;
	};

	void setCredentials(const Credentials & newCredentials)
	{
		credentials = newCredentials;
		save();
	}

	void clear()
	{
		credentials.password.clear();
		credentials.username.clear();
	}
	/**
	We return a pointer to avoid copying our password all over memory...though presumably String
	won't do a copy anyway .. but 
	*/
	Credentials * getCredentials()
	{
		load();
		return &credentials;
	}

private:
	void save() const;

	void load();

	String serviceName;
	File file;

	Credentials credentials;

	KeyData* keyData;

	bool deleteKeyData;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StoredCredentials)
};

