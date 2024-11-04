
#pragma once
#include <juce_core/juce_core.h>

namespace jcf
{
class KeyData
{
public:
    virtual ~KeyData() = default;

    /** Generate and return the key. */
    virtual juce::uint8* getKeyData (int* keyLength) = 0;

    /** Clear the complete key data from memory. */
    virtual void clear() = 0;
};

/**
 * Creates a 72 byte key by combining local key data with a fixed application key. This
 * makes it fractionally harder to write a tool that decodes a locally encrypted file.
 */
class LocalKeyData : public KeyData
{
public:
    enum
    {
        APP_KEY_LEN = 24,
        LOCAL_KEY_LEN = 24
    };

    LocalKeyData (const juce::uint8* fixedKeyDataLen24, const juce::File& localKeyDataLen32);

    juce::uint8* getKeyData (int* keyLength) override;

    void generateKey();

    void clear() override;

private:
    std::vector<juce::uint8> key;
    juce::File localKeyDataLen32;
    const juce::uint8* fixedKeyDataLen24;
};

/**
 * A SecureCredentials object stores a username and a password.  It will eventually make
 * use of whatever the most practical secure password storage mechanism is on a
 * particular platform.  But right now it'll use a BlowFish encrypted file using whatever
 * key is provided for the encryption.
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
    StoredCredentials (const juce::File& fileOfLastResort, const juce::String& serviceName, KeyData* keyData, bool deleteKeyDataWhenFinished);

    ~StoredCredentials();

    struct Credentials
    {
        juce::String username{};
        juce::String password{};
    };

    void setCredentials (const Credentials& newCredentials);

    void clear();

    /**
     * We return a pointer to avoid copying our password all over memory...though
     * presumably String won't do a copy anyway .. but
     */
    Credentials* getCredentials();

private:
	void save() const;
	void load();

	juce::String serviceName;
	juce::File file;

	Credentials credentials;

	KeyData* keyData;

	bool deleteKeyData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StoredCredentials)
};


} // namespace jcf