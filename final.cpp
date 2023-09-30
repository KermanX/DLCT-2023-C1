#pragma GCC optimize(2)

#include <iostream>

constexpr uint64_t uint64_max = ~0ull;

struct UInt128
{
  uint64_t high;
  uint64_t low;
};

/**
 * the same as log2
 */
int get_bit_count(uint64_t x)
{
  int result = 0;
  if (x & 0xffffffff00000000ull)
  {
    result += 32;
    x >>= 32;
  }
  if (x & 0xffff0000ull)
  {
    result += 16;
    x >>= 16;
  }
  if (x & 0xff00ull)
  {
    result += 8;
    x >>= 8;
  }
  if (x & 0xf0ull)
  {
    result += 4;
    x >>= 4;
  }
  if (x & 0xcull)
  {
    result += 2;
    x >>= 2;
  }
  if (x & 0x2ull)
  {
    result += 1;
    x >>= 1;
  }
  return result + x;
};

int get_bit_count(const UInt128 &x)
{
  if (x.high == 0)
    return get_bit_count(x.low);
  else
    return get_bit_count(x.high) + 64;
}

void left_shift_assignment(UInt128 &x, int shift)
{
  if (shift == 0)
    return;
  if (shift >= 64)
  {
    x.high = x.low;
    x.low = 0;
    shift -= 64;
  }
  x.high <<= shift;
  x.high |= x.low >> (64 - shift);
  x.low <<= shift;
}

void right_shift_1_assignment(UInt128 &x)
{
  x.low >>= 1;
  x.low |= x.high << (64 - 1);
  x.high >>= 1;
}

void subtraction_assignment(UInt128 &x, const UInt128 &y)
{
  x.high -= y.high;
  if (x.low < y.low)
  {
    x.high--;
    // 2^64 + a - b = b + ~b + 1 + a - b = a + ~b + 1
    x.low = x.low + (~y.low) + 1;
  }
  else
  {
    x.low -= y.low;
  }
}

bool is_divisible(const UInt128 &dividend, int dividend_bit_count, uint64_t divisor, int divisor_bit_count, UInt128 &quotient)
{
  quotient.high = 0;
  quotient.low = 0;

  UInt128 dividend_copy(dividend);
  UInt128 divisor_copy{0, divisor};

  int offset = dividend_bit_count - divisor_bit_count;
  left_shift_assignment(divisor_copy, offset);

  while (offset + divisor_bit_count >= 64)
  {
    if (dividend_copy.high == divisor_copy.high && dividend_copy.low == divisor_copy.low)
    { // dividend_copy == divisor_copy
      quotient.low |= 0x1ull;
      return true;
    }
    else if (dividend_copy.high > divisor_copy.high || (dividend_copy.high == divisor_copy.high && dividend_copy.low > divisor_copy.low))
    { // dividend_copy > divisor_copy
      subtraction_assignment(dividend_copy, divisor_copy);
      if (offset >= 64)
      {
        quotient.high |= 0x1ull << (offset - 64);
      }
      else
      {
        quotient.low |= 0x1ull << offset;
      }
    }
    right_shift_1_assignment(divisor_copy);
    offset--;
  }
  if (dividend_copy.low % divisor != 0)
  {
    return false;
  }
  else
  {
    quotient.low |= dividend_copy.low / divisor;
    return true;
  }
}

/**
 * @attention `x` must be odd
 */
int interger_factorization(const UInt128 &x, uint64_t result[], UInt128 &last_result)
{
  uint64_t divisor = 3;
  UInt128 current_x(x);
  int result_count = 0;
  int dividend_bit_count, divisor_bit_count;
  while ((dividend_bit_count = get_bit_count(current_x)) + 1 >= 2 * (divisor_bit_count = get_bit_count(divisor)))
  {
    UInt128 quotient;
    if (is_divisible(current_x, dividend_bit_count, divisor, divisor_bit_count, quotient))
    {
      result[result_count] = divisor;
      result_count++;
      current_x = quotient;
    }
    else
    {
      divisor += 2;
    }
  }
  last_result = current_x;
  return result_count;
}

/*
 * set `result` to `2^p - 1`
 */
void get_mersenne_number(int p, UInt128 &result)
{
  if (p <= 64)
  {
    result.high = 0;
    result.low = uint64_max >> (64 - p);
  }
  else
  {
    result.high = uint64_max >> (128 - p);
    result.low = uint64_max;
  }
}

int main()
{
  for (int i = 23; i < 71; i++)
  {
    UInt128 x, last_result;
    uint64_t result[100];

    get_mersenne_number(i, x);
    const int result_count = interger_factorization(x, result, last_result);

    std::cout << i << ": ";
    for (int i = 0; i < result_count; i++)
    {
      std::cout << result[i] << " ";
    }
    if (last_result.high != 0 || last_result.low != 1)
    {
      // in fact, last_result.high must be 0
      std::cout << last_result.low;
    }
    std::cout << std::endl;
  }
}
