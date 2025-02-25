#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class BigInt {
private:
    vector<int> digits;
    int sign;

    vector<int> addVectors(const vector<int>& a, const vector<int>& b) {
        vector<int> result;
        int carry = 0;
        size_t max_size = max(a.size(), b.size());
        for (size_t i = 0; i < max_size || carry; ++i) {
            int digit1 = (i < a.size() ? a[i] : 0);
            int digit2 = (i < b.size() ? b[i] : 0);
            int sum = digit1 + digit2 + carry;
            result.push_back(sum % 10);
            carry = sum / 10;
        }
        return result;
    }

    vector<int> subtractVectors(const vector<int>& a, const vector<int>& b) {
        vector<int> result = a;
        int carry = 0;
        for (size_t i = 0; i < b.size() || carry; ++i) {
            int digit1 = result[i];
            int digit2 = (i < b.size() ? b[i] : 0);
            int diff = digit1 - digit2 - carry;
            if (diff < 0) {
                diff += 10;
                carry = 1;
            } else {
                carry = 0;
            }
            result[i] = diff;
        }

        while (result.size() > 1 && result.back() == 0)
            result.pop_back();

        return result;
    }

    int compareAbs(const BigInt& other) const {
        if (digits.size() != other.digits.size())
            return digits.size() > other.digits.size() ? 1 : -1;

        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i])
                return digits[i] > other.digits[i] ? 1 : -1;
        }
        return 0;
    }

    string toString() const {
        string result;
        if (sign == -1 && !(digits.size() == 1 && digits[0] == 0))
            result.push_back('-');
        for (int i = digits.size() - 1; i >= 0; --i)
            result.push_back(digits[i] + '0');
        return result;
    }

    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0)
            digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0)
            sign = 1;
    }

public:
    BigInt(const string& str) : sign(1), digits() {
        size_t index = 0;

        if (str[0] == '-') {
            sign = -1;
            ++index;
        } else if (str[0] == '+') {
            ++index;
        }

        while (index < str.size() && str[index] == '0') {
            ++index;
        }

        if (index == str.size()) {
            digits.push_back(0);
            sign = 1;
            return;
        }

        for (size_t i = index; i < str.size(); ++i) {
            digits.push_back(str[i] - '0');
        }

        reverse(digits.begin(), digits.end());
    }

    BigInt() : sign(1), digits(1, 0) {}

    BigInt operator+(const BigInt& other) const {
        BigInt result;
        if (sign == other.sign) {
            result.digits = addVectors(digits, other.digits);
            result.sign = sign;
        } else {
            int cmp = compareAbs(other);
            if (cmp == 0) {
                return BigInt("0");
            } else if (cmp > 0) {
                result.digits = subtractVectors(digits, other.digits);
                result.sign = sign;
            } else {
                result.digits = subtractVectors(other.digits, digits);
                result.sign = other.sign;
            }
        }
        result.removeLeadingZeros();
        return result;
    }

    BigInt operator-(const BigInt& other) const {
        return *this + (-other);
    }

    BigInt operator-() const {
        BigInt result(*this);
        if (!(result.digits.size() == 1 && result.digits[0] == 0))
            result.sign = -result.sign;
        return result;
    }

    BigInt operator*(const BigInt& other) const {
        if ((digits.size() == 1 && digits[0] == 0) || (other.digits.size() == 1 && other.digits[0] == 0))
            return BigInt("0");

        vector<int> result(digits.size() + other.digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            for (size_t j = 0; j < other.digits.size(); ++j) {
                result[i + j] += digits[i] * other.digits[j];
            }
        }

        int carry = 0;
        for (size_t i = 0; i < result.size(); ++i) {
            int temp = result[i] + carry;
            result[i] = temp % 10;
            carry = temp / 10;
        }

        while (result.size() > 1 && result.back() == 0)
            result.pop_back();

        BigInt output;
        output.digits = result;
        output.sign = sign * other.sign;
        if (output.digits.size() == 1 && output.digits[0] == 0)
            output.sign = 1;
        return output;
    }

    BigInt operator/(const BigInt& other) const {
        if (compareAbs(other) < 0)
            return BigInt("0");

        BigInt dividendAbs = *this;
        dividendAbs.sign = 1;

        BigInt divisorAbs = other;
        divisorAbs.sign = 1;

        string dividendStr = dividendAbs.toString();
        string quotientStr;
        BigInt remainder("0");

        for (char ch : dividendStr) {
            remainder = remainder * BigInt("10");
            remainder = remainder + BigInt(string(1, ch));

            int count = 0;
            while (remainder.compareAbs(divisorAbs) >= 0) {
                remainder = remainder - divisorAbs;
                ++count;
            }
            quotientStr.push_back('0' + count);
        }

        int pos = quotientStr.find_first_not_of('0');
        if (pos != string::npos)
            quotientStr = quotientStr.substr(pos);
        else
            quotientStr = "0";

        BigInt result(quotientStr);
        result.sign = sign * other.sign;
        if (result.digits.size() == 1 && result.digits[0] == 0)
            result.sign = 1;
        return result;
    }

    BigInt operator%(const BigInt& other) const {
        return *this - (*this / other) * other;
    }

    bool operator==(const BigInt& other) const {
        return toString() == other.toString();
    }

    bool operator<(const BigInt& other) const {
        if (sign != other.sign)
            return sign < other.sign;
        if (digits.size() != other.digits.size())
            return (sign == 1) ? digits.size() < other.digits.size() : digits.size() > other.digits.size();
        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i])
                return (sign == 1) ? digits[i] < other.digits[i] : digits[i] > other.digits[i];
        }
        return false;
    }

    bool operator<=(const BigInt& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const BigInt& other) const {
        return !(*this <= other);
    }

    bool operator>=(const BigInt& other) const {
        return !(*this < other);
    }

    friend ostream& operator<<(ostream& out, const BigInt& num) {
        out << num.toString();
        return out;
    }
};

bool isPrime(const BigInt& n) {
    if (n < BigInt("2"))
        return false;
    BigInt two("2"), one("1"), zero("0");
    for (BigInt d = two; d * d <= n; d = d + one) {
        if (n % d == zero)
            return false;
    }
    return true;
}

vector<BigInt> primeFactorization(const BigInt& n) {
    vector<BigInt> factors;
    BigInt temp = (n < BigInt("0")) ? -n : n;
    BigInt two("2"), one("1");

    for (BigInt i = two; i * i <= temp; i = i + one) {
        if (isPrime(i)) {
            while (temp % i == BigInt("0")) {
                factors.push_back(i);
                temp = temp / i;
            }
        }
    }

    if (temp > one)
        factors.push_back(temp);

    return factors;
}

int main() {
    string a;
    cout << "Enter the number: ";
    cin >> a;

    BigInt num(a);
    vector<BigInt> factors = primeFactorization(num);

    for (size_t i = 0; i < factors.size(); ++i) {
        cout << factors[i];
        if (i < factors.size() - 1)
            cout << " * ";
    }

    cout << endl;
    return 0;
}