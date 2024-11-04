#include "jcf_secure_credentials.h"
namespace jcf
{
using namespace juce;
void StoredCredentials::save() const
{
    int keyLen;
    auto data = keyData->getKeyData (&keyLen);
    // ScopedPointer<BlowfishExtended> blowFish = new BlowfishExtended(data, keyLen);

    const std::unique_ptr<BlowfishExtended> blowFish = std::make_unique<BlowfishExtended> (data, keyLen);

    ValueTree t{ "credentials" };
    t.setProperty ("username", credentials.username, nullptr);
    t.setProperty ("password", credentials.password, nullptr);
    auto memory = blowFish->encrypt (t);
    // MemoryBlock memory;
    keyData->clear();
    file.replaceWithData (memory.getData(), memory.getSize());
}

void StoredCredentials::load()
{
    int keyLen;
    auto data = keyData->getKeyData (&keyLen);
    // ScopedPointer<BlowfishExtended> blowFish = new BlowfishExtended(data, keyLen);

    const std::unique_ptr<BlowfishExtended> blowFish = std::make_unique<BlowfishExtended> (data, keyLen);

    if (! file.existsAsFile())
        return;

    MemoryBlock memory;
    file.loadFileAsData (memory);

    ValueTree t;

    auto result = blowFish->decrypt (memory, t);

    keyData->clear();

    jassert (result);

    if (result)
    {
        credentials.username = t["username"];
        credentials.password = t["password"];
    }
}

LocalKeyData::LocalKeyData (const uint8* fixedKeyDataLen24, const File& localKeyDataLen32)
    : localKeyDataLen32 (localKeyDataLen32), fixedKeyDataLen24 (fixedKeyDataLen24)
{
    key.reserve (72);
}
uint8* LocalKeyData::getKeyData (int* keyLength)
{
    generateKey();

    if (keyLength != nullptr)
        *keyLength = int (key.size());

    return key.data();
}
void LocalKeyData::clear()
{
    for (auto& v : key)
        v = 0;

    key.clear();
}
StoredCredentials::StoredCredentials (const File& fileOfLastResort, const String& serviceName, KeyData* keyData, bool deleteKeyDataWhenFinished)
    : serviceName (serviceName), file (fileOfLastResort), keyData (keyData), deleteKeyData (deleteKeyDataWhenFinished)
{
    load();
}
StoredCredentials::~StoredCredentials()
{
    if (deleteKeyData)
        delete keyData;
}
void StoredCredentials::setCredentials (const Credentials& newCredentials)
{
    credentials = newCredentials;
    save();
}
void StoredCredentials::clear()
{
    credentials.password.clear();
    credentials.username.clear();
}
StoredCredentials::Credentials* StoredCredentials::getCredentials()
{
    load();
    return &credentials;
}
void LocalKeyData::generateKey()
{
    key.clear();

    if (! localKeyDataLen32.existsAsFile())
    {
        std::random_device randomDevice;
        std::uniform_int_distribution<int> distribution (0, 255);

        for (int i = 0; i < LOCAL_KEY_LEN; ++i)
            key.push_back (uint8 (distribution (randomDevice)));

        localKeyDataLen32.replaceWithData (key.data(), key.size());
    }
    else
    {
        MemoryBlock block;
        localKeyDataLen32.loadFileAsData (block);

        for (size_t i = 0; i < block.getSize(); ++i)
            key.push_back (*(static_cast<uint8*> (block.getData()) + i));
    }

    for (int i = 0; i < APP_KEY_LEN; ++i)
        key.push_back (fixedKeyDataLen24[i]);
}

// void StoredCredentials::setPassword(const String& newPassword)
//{
//	DATA_BLOB DataIn;
//	DATA_BLOB DataOut;
//
//	BYTE *pbDataInput = (BYTE *)"Hello world of data protection.";
//	DWORD cbDataInput = strlen((char *)pbDataInput) + 1;
//
//	DataIn.pbData = pbDataInput;
//	DataIn.cbData = cbDataInput;
//
//	//  Begin protect phase. Note that the encryption key is created
//	//  by the function and is not passed.
//
//	if (CryptProtectData(
//		&DataIn,
//		L"This is the description string.", // A description string
//											// to be included with the
//											// encrypted data.
//		NULL,                               // Optional entropy not used.
//		NULL,                               // Reserved.
//		NULL,                               // Pass NULL for the
//											// prompt structure.
//		0,
//		&DataOut))
//	{
//		printf("The encryption phase worked.\n");
//	}
//	else
//	{
//		jassertfalse;
//	}
//
//}

class StoredCredentialsUnitTest : public UnitTest
{
public:
    StoredCredentialsUnitTest() : UnitTest ("StoredCredentialsUnitTest") {}

    void runTest() override
    {
        beginTest ("Stored Credentials");

        std::vector<uint8> appKey = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3,
        };

        TemporaryFile tempFile;

        LocalKeyData keyData (appKey.data(), tempFile.getFile());

        TemporaryFile tempCredentialStore;

        StoredCredentials storedCredentials (tempCredentialStore.getFile(), "Temp", &keyData, false);

        storedCredentials.setCredentials ({ "user", "pass123" });

        storedCredentials.clear();

        auto credentialPointer = storedCredentials.getCredentials();

        expect (credentialPointer->password == "pass123");
        expect (credentialPointer->username == "user");
    }
};

static StoredCredentialsUnitTest stored_credentials_unit_test;
} // namespace jcf