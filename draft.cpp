#pragma GCC optimize(2)

#include <iostream>
// #include <vector>
#include <chrono>

constexpr uint64_t uint64_max = 0xFFFFFFFFFFFFFFFFull;

struct UInt128
{
  uint64_t high;
  uint64_t low;
};

void print(const UInt128 &x)
{
  if (x.high == 0)
    std::cout << x.low << std::endl;
  else
    std::cout << std::hex << x.high << "," << x.low << std::dec << std::endl;
}

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

/**
 * @return `1` if x > y, `0` if x == y, `-1` if x < y
 */
int compare(const UInt128 &x, const UInt128 &y)
{
  if (x.high > y.high)
    return 1;
  else if (x.high < y.high)
    return -1;
  else if (x.low > y.low)
    return 1;
  else if (x.low < y.low)
    return -1;
  else
    return 0;
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

// void add_2_assignment(UInt128 &x)
// {
//   x.low += 2;
//   if (x.low < 2)
//     x.high++;
// }

bool is_divisible(const UInt128 &dividend, int dividend_bit_count, uint64_t divisor, int divisor_bit_count, UInt128 &result)
{
  result.high = 0;
  result.low = 0;

  UInt128 dividend_copy(dividend);
  UInt128 divisor_copy{0, divisor};
  // const int dividend_bit_count = get_bit_count(dividend),
  //           divisor_bit_count = get_bit_count(divisor);
  // const int initial_offset = dividend_bit_count - divisor_bit_count;
  // int offset = initial_offset;
  int offset = dividend_bit_count - divisor_bit_count;
  left_shift_assignment(divisor_copy, offset);
  while (offset + divisor_bit_count >= 64)
  {
    switch (compare(dividend_copy, divisor_copy))
    {
    case 1:
      subtraction_assignment(dividend_copy, divisor_copy);
      if (offset >= 64)
      {
        result.high |= 0x1ull << (offset - 64);
      }
      else
      {
        result.low |= 0x1ull << offset;
      }
      break;
    case 0:
      result.low |= 0x1ull;
      return true;
    case -1:
      break;
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
    result.low |= dividend_copy.low / divisor;
    return true;
  }
}

/**
 * @attention `x` must be odd
 */
int interger_factorization(const UInt128 &x, uint64_t result[], UInt128 &last_result)
{
  int result_count = 0;
  uint64_t divisor = 3;
  UInt128 current_x(x);
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

// int main()
// {
//   int x;
//   std::cin >> x;

//   UInt128 a, b, c, last_result;
//   uint64_t result[100];

//   auto start = std::chrono::high_resolution_clock::now();

//   get_mersenne_number(x, a);
//   const int result_count = interger_factorization(a, result, last_result);

//   auto end = std::chrono::high_resolution_clock::now();

//   std::cout << "--" << std::endl;
//   for (int i = 0; i < result_count; i++)
//   {
//     std::cout << result[i] << std::endl;
//   }
//   if (last_result.high != 0 || last_result.low != 1)
//   {
//     print(last_result);
//   }

//   std::chrono::duration<double, std::ratio<1, 1000>> diff = end - start;
//   std::cout << "-- " << diff.count() << "ms" << std::endl;
//   return 0;
// }

int main()
{
  int n;
  std::cin >> n;

  UInt128 x, last_result;
  uint64_t result[100];
  for (int i = 0; i < n; i++)
  {
    int p;
    std::cin >> p;
    get_mersenne_number(p, x);
    const int result_count = interger_factorization(x, result, last_result);
    for (int i = 0; i < result_count; i++)
    {
      std::cout << result[i] << " ";
    }
    if (last_result.high != 0 || last_result.low != 1)
    {
      print(last_result);
    }
  }
}

// int main()
// {
//   UInt128 x, last_result;
//   uint64_t result[100];
//   for (int i = 23; i < 71; i++)
//   {
//     std::cout << i << ": ";
//     get_mersenne_number(i, x);
//     const int result_count = interger_factorization(x, result, last_result);
//     for (int i = 0; i < result_count; i++)
//     {
//       std::cout << result[i] << " ";
//     }
//     if (last_result.high != 0 || last_result.low != 1)
//     {
//       print(last_result);
//     }
//   }
// }