#include "RSA.h"

long long stepen(long long b, long long k, long long m) {
	if (k == 0) {
		return 1;
	}
	if (k == 1) {
		return b;
	}
	if (k % 2 == 0) {
		long long kostyl = stepen(b, k / 2, m) % m;
		return (kostyl * kostyl) % m;
	}
	else {
		return (stepen(b, (k - 1), m) % m * b % m) % m;
	}
}

bool isPrime(long long prime)
{
	long long i;

	for (i = 2; i*i <= prime; i++)
	{
		if (prime % i == 0)
		{
			return false;
		}
	}

	return true;
}

long long calculateE(long long t)
{
	long long e;

	for (e = 2; e < t; e++)
	{
		if (greatestCommonDivisor(e, t) == 1)
		{
			return e;
		}
	}

	return -1;
}

long long greatestCommonDivisor(long long e, long long t)
{
	while (e > 0)
	{
		long long myTemp;

		myTemp = e;
		e = t % e;
		t = myTemp;
	}

	return t;
}

long long calculateD(long long e, long long t)
{
	long long d;
	long long k = 1;

	while (1)
	{
		k = k + t;

		if (k % e == 0)
		{
			d = (k / e);
			return d;
		}
	}

}


long long encrypt(long long i, long long e, long long n)
{
	long long current, result;

	current = i - 97;

	result = stepen(current, e, n);

	return result;
}

long long decrypt(long long i, long long d, long long n)
{
	unsigned long long result;

	result = stepen(i, d, n);

	return result + 97;
}

long long genPrime() {
	while (true) {
		long long from;
		BCryptGenRandom(NULL, (PUCHAR)&from, sizeof(long long), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		from = from % (long long) 1e12 + 1e11;
		for (long long i = from; i <= 1e12; i++) {
			if (isPrime(i)) {
				return i;
			}
		}
	}
}