#pragma once
#include <math.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <ctime>
#include <windows.h>

bool isPrime(long long prime);
long long calculateE(long long t);
long long greatestCommonDivisor(long long e, long long t);
long long calculateD(long long e, long long t);
long long encrypt(long long i, long long e, long long n);
long long decrypt(long long i, long long d, long long n);
long long genPrime();