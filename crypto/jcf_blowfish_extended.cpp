
BlowfishExtended::BlowfishExtended(const void* keyData, int keyBytes): blowFish(keyData, keyBytes)
{
}

MemoryBlock BlowfishExtended::encrypt(const ValueTree& data) const
{
	MemoryBlock memory;

	{
		MemoryOutputStream outStream(memory, false);
		data.writeToStream(outStream);
	}

	encryptMemoryBlock(memory);
	return memory;
}

MemoryBlock BlowfishExtended::encrypt(const String& string) const
{
	MemoryBlock memory;
	appendStringToMemoryBlock(string, memory);
	encryptMemoryBlock(memory);
	return memory;
}

Result BlowfishExtended::decrypt(MemoryBlock& source, ValueTree& resultingTree) const
{
	auto result = decryptMemoryBlock(source);

	if (!result)
		return result;

	resultingTree = ValueTree::readFromData(source.getData(), source.getSize());

	return Result::ok();
}

Result BlowfishExtended::decrypt(MemoryBlock& source, String& resultingString) const
{
	auto result = decryptMemoryBlock(source);

	if (!result)
		return result;

	auto data = static_cast<const char *>(source.getData());

	resultingString = String::fromUTF8(data, source.getSize());

	return Result::ok();
}

void BlowfishExtended::appendStringToMemoryBlock(const String& string, MemoryBlock& memoryBlock)
{
	auto num = string.getNumBytesAsUTF8();
	memoryBlock.append(string.toRawUTF8(), num);
}

void BlowfishExtended::encryptMemoryBlock(MemoryBlock& memoryBlock) const
{
	addPaddingPKCS5(memoryBlock);

	auto numBytes = memoryBlock.getSize();
	auto data = memoryBlock.getData();

	jassert(numBytes % 8 == 0); // should be true: see addPaddingPKCS5

	for (auto i = 0; i < numBytes; i += 8)
		blowFish.encrypt(
			*(static_cast<uint32*>(data) + i / sizeof(uint32)),
			*(static_cast<uint32*>(data) + i / sizeof(uint32) + 1)
		);
}

Result BlowfishExtended::decryptMemoryBlock(MemoryBlock& memoryBlock) const
{
	auto numBytes = memoryBlock.getSize();
	auto data = memoryBlock.getData();

	if (numBytes % 8 != 0)
	{
		/* We expect that well formed encrypted data will be aligned to 8 bytes. */
		jassertfalse;
		return Result::fail("Invalid input data");
	}

	for (auto i = 0; i < numBytes; i += 8)
		blowFish.decrypt(
			*(static_cast<uint32*>(data) + i / sizeof(uint32)),
			*(static_cast<uint32*>(data) + i / sizeof(uint32) + 1)
		);

	auto paddingRemovalResult = removePaddingPKCS5(memoryBlock);

	if (!paddingRemovalResult)
		return Result::fail("Corrupt data or key");

	return Result::ok();
}

void BlowfishExtended::addPaddingPKCS5(MemoryBlock& memoryBlock)
{
	auto numBytes =  8 - int(memoryBlock.getSize() % 8);

	if (numBytes == 0)
		numBytes = 8;

	std::vector<uint8> padding(numBytes, numBytes);
	memoryBlock.append(padding.data(), padding.size());
}

bool BlowfishExtended::removePaddingPKCS5(MemoryBlock& memoryBlock)
{
	if (memoryBlock.getSize() == 0)
		return false;

	auto numBytes = *(static_cast<uint8*>(memoryBlock.getData()) + memoryBlock.getSize() - 1);

	if (numBytes > memoryBlock.getSize())
		return false;

	for (size_t i = memoryBlock.getSize() - numBytes; i < memoryBlock.getSize(); ++i)
		if (*(static_cast<uint8*>(memoryBlock.getData()) + i) != numBytes)
			return false;

	memoryBlock.removeSection(memoryBlock.getSize() - numBytes, numBytes);

	return true;
}

class BlowFishExtendedTests
	:
	public UnitTest
{
public:
	BlowFishExtendedTests()
		:
		UnitTest("BlowFishExtended")
	{}

	void runTest() override
	{
		beginTest("BlowFishExtendedTests");
		uint8 key[32] =
		{
			1, 2, 3, 4, 1, 2, 3, 4,
			1, 2, 3, 4, 1, 2, 3, 4,
			1, 2, 3, 4, 1, 2, 3, 4,
			1, 2, 3, 4, 1, 2, 3, 4,
		};

		BlowfishExtended blowFish(key, 32);

		StringArray strings;
		strings.add("1234567890");
		strings.add("123456789");
		strings.add("12345678");
		strings.add("1234567");
		strings.add("123456");
		strings.add("12345");
		strings.add("1234");
		strings.add("123");
		strings.add("12");
		strings.add("1");
		strings.add(String());

		for (auto s :strings)
		{
			auto memory = blowFish.encrypt(s);
			String decryptedString;
			auto result = blowFish.decrypt(memory, decryptedString);
			expect(result);
			expect(decryptedString == s);
		}

		{
			auto m1 = blowFish.encrypt(ValueTree("tree"));
			ValueTree r1;
			auto result = blowFish.decrypt(m1, r1);
			expect(result);
			expect(r1.getType() == Identifier("tree"));
		}
		
	}
};

static BlowFishExtendedTests blowfish_extended_tests;