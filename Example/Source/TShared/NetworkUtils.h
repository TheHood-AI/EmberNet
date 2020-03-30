#pragma once

constexpr float BytesToMB(const unsigned int aBytes)
{
	return static_cast<float>(aBytes) / 1000.f;
}

constexpr unsigned int MBToBytes(const unsigned int aMegaBytes)
{
	return aMegaBytes * 1000 * 1000;
}

constexpr unsigned int GBToMB(const unsigned int aGigaBytes)
{
	return aGigaBytes * 1000;
}

constexpr unsigned int GBToBytes(const unsigned int aGigaBytes)
{
	return MBToBytes(GBToMB(aGigaBytes));
}

constexpr unsigned int globalSocketbufferSize = MBToBytes(500);
