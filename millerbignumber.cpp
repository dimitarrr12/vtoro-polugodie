#include <bits/stdc++.h>
#include <random>
using namespace std;

class BigNumber {
private:
    vector<int> digits;
    int sign;  // 1 for positive, -1 for negative

    vector<int> addVectors(const vector<int>& a, const vector<int>& b) {
        vector<int> result;
        int carry = 0;
        int size = max(a.size(), b.size());
        for (int i = 0; i < size || carry; i++) {
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
        int borrow = 0;
        for (int i = 0; i < b.size() || borrow; i++) {
            int digit1 = result[i];
            int digit2 = (i < b.size() ? b[i] : 0);
            int diff = digit1 - digit2 - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result[i] = diff;
        }
        while (result.size() > 1 && result.back() == 0) result.pop_back();
        return result;
    }

    int compareAbs(const BigNumber& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() > other.digits.size() ? 1 : -1;
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return digits[i] > other.digits[i] ? 1 : -1;
            }
        }
        return 0;
    }

    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0) sign = 1;
    }

public:
    BigNumber(const string& str) : sign(1), digits() {
        int index = 0;
        if (str[0] == '-') {
            sign = -1;
            index++;
        } else if (str[0] == '+') {
            sign = 1;
            index++;
        }
        while (index < str.size() && str[index] == '0') index++;
        if (index == str.size()) {
            digits.push_back(0);
            sign = 1;
            return;
        }
        for (int i = index; i < str.size(); i++) {
            digits.push_back(str[i] - '0');
        }
        reverse(digits.begin(), digits.end());
    }

    BigNumber() : sign(1), digits(1, 0) {}

    string toString() const {
        string str;
        if (sign == -1 && !(digits.size() == 1 && digits[0] == 0)) {
            str.push_back('-');
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            str.push_back(digits[i] + '0');
        }
        return str;
    }

    BigNumber operator+(const BigNumber& other) const {
        BigNumber result;
        if (sign == other.sign) {
            result.digits = addVectors(digits, other.digits);
            result.sign = sign;
        } else {
            int cmp = compareAbs(other);
            if (cmp == 0) return BigNumber("0");
            else if (cmp > 0) {
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

    BigNumber operator-(const BigNumber& other) const {
        return *this + (-other);
    }

    BigNumber operator-() const {
        BigNumber result(*this);
        if (!(result.digits.size() == 1 && result.digits[0] == 0)) result.sign = -result.sign;
        return result;
    }

    BigNumber operator*(const BigNumber& other) const {
        BigNumber result;
        if ((digits.size() == 1 && digits[0] == 0) || (other.digits.size() == 1 && other.digits[0] == 0)) return BigNumber("0");

        vector<int> res(digits.size() + other.digits.size(), 0);
        for (int i = 0; i < digits.size(); i++) {
            for (int j = 0; j < other.digits.size(); j++) {
                res[i + j] += digits[i] * other.digits[j];
            }
        }

        int carry = 0;
        for (int i = 0; i < res.size(); i++) {
            int temp = res[i] + carry;
            res[i] = temp % 10;
            carry = temp / 10;
        }

        while (res.size() > 1 && res.back() == 0) res.pop_back();

        result.digits = res;
        result.sign = sign * other.sign;
        if (result.digits.size() == 1 && result.digits[0] == 0) result.sign = 1;
        return result;
    }

    BigNumber operator/(const BigNumber& other) const {
        if (compareAbs(other) < 0) return BigNumber("0");

        BigNumber dividendAbs = *this;
        dividendAbs.sign = 1;
        BigNumber divisorAbs = other;
        divisorAbs.sign = 1;

        string dividendStr = dividendAbs.toString();
        string quotientStr = "";
        BigNumber remainder("0");

        for (char ch : dividendStr) {
            remainder = remainder * BigNumber("10");
            remainder = remainder + BigNumber(string(1, ch));

            int count = 0;
            while (remainder.compareAbs(divisorAbs) >= 0) {
                remainder = remainder - divisorAbs;
                count++;
            }
            quotientStr.push_back('0' + count);
        }

        int pos = quotientStr.find_first_not_of('0');
        if (pos != string::npos) quotientStr = quotientStr.substr(pos);
        else quotientStr = "0";

        BigNumber result(quotientStr);
        result.sign = sign * other.sign;
        if (result.digits.size() == 1 && result.digits[0] == 0) result.sign = 1;
        return result;
    }

    BigNumber operator%(const BigNumber& other) const {
        return *this - ((*this) / other) * other;
    }

    bool operator==(const BigNumber& other) const {
        return toString() == other.toString();
    }

    bool operator<(const BigNumber& other) const {
        if (sign != other.sign) return sign < other.sign;
        if (digits.size() != other.digits.size()) {
            return (sign == 1) ? digits.size() < other.digits.size() : digits.size() > other.digits.size();
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return (sign == 1) ? digits[i] < other.digits[i] : digits[i] > other.digits[i];
            }
        }
        return false;
    }

    bool operator<=(const BigNumber& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const BigNumber& other) const {
        return !(*this <= other);
    }

    bool operator>=(const BigNumber& other) const {
        return !(*this < other);
    }

    friend ostream& operator<<(ostream& out, const BigNumber& num) {
        out << num.toString();
        return out;
    }
};

BigNumber expmod(BigNumber base, BigNumber exponent, BigNumber mod) {
    BigNumber result("1");
    BigNumber one("1"), zero("0");

    for (BigNumber i = zero; i < exponent; i = i + one) {
        result = (result * base) % mod;
    }

    return result;
}

BigNumber generateRandomBigNumber(BigNumber numDigits) {
    if (numDigits <= BigNumber("0")) return BigNumber("0");

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 9);
    uniform_int_distribution<int> distRest(0, 9);

    string randStr;
    randStr += to_string(dist(gen));

    for (BigNumber i = BigNumber("1"); i < numDigits; i = i + BigNumber("1")) {
        randStr += to_string(distRest(gen));
    }

    return BigNumber(randStr);
}

bool MillerRabin(BigNumber n, BigNumber d) {
    BigNumber two("2"), one("1"), zero("0"), four("4");
    BigNumber a = generateRandomBigNumber(n) % (n - four) + two;

    BigNumber x = expmod(a, d, n);
    if (x == one || x == n - one) return true;

    while (d < (n - one)) {
        x = (x * x) % n;
        d = d * two;

        if (x == one) return false;
        if (x == (n - one)) return true;
    }

    return false;
}

bool isPrime(BigNumber n, int k) {
    BigNumber two("2"), one("1"), zero("0"), four("4"), three("3");
    if (n <= one || n == four) return false;
    if (n <= three) return true;

    BigNumber d = n - one;
    while (d % two == zero) d = d / two;

    for (int i = 0; i < k; i++) {
        if (!MillerRabin(n, d)) return false;
    }

    return true;
}

int main() {
    int k = 4;
    int primeCount = 0;
    string input;
    
    cout << "Enter large number N:" << endl;
    cin >> input;
    BigNumber n(input);
    
    for (BigNumber i = BigNumber("1"); i < n; i = i + BigNumber("1")) {
        if (isPrime(n, k)) {
            primeCount++;
            cout << n << " ";
        }
    }
    cout << "Total primes: " << primeCount << endl;

    return 0;
}