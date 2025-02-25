#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class BigInt {
private:
    vector<int> digits;
    int sign; // 1 for positive, -1 for negative

    // Helper function to add two vectors
    vector<int> addVectors(const vector<int>& a, const vector<int>& b) const {
        vector<int> result;
        int carry = 0, n = max(a.size(), b.size());
        for (int i = 0; i < n || carry; i++) {
            int digit1 = (i < a.size() ? a[i] : 0);
            int digit2 = (i < b.size() ? b[i] : 0);
            int sum = digit1 + digit2 + carry;
            result.push_back(sum % 10);
            carry = sum / 10;
        }
        return result;
    }

    // Helper function to subtract two vectors (assumes a >= b)
    vector<int> subtractVectors(const vector<int>& a, const vector<int>& b) const {
        vector<int> result = a;
        int carry = 0;
        for (int i = 0; i < b.size() || carry; i++) {
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

    // Helper function to remove leading zeros
    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0)
            digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0)
            sign = 1;
    }

    // Helper function to convert the number to a string
    string toString() const {
        string str;
        if (sign == -1 && !(digits.size() == 1 && digits[0] == 0))
            str.push_back('-');
        for (int i = digits.size() - 1; i >= 0; i--)
            str.push_back(digits[i] + '0');
        return str;
    }

public:
    // Constructor to create a BigInt from a string
    BigInt(const string& str) : sign(1), digits() {
        int index = 0;
        if (str[0] == '-') {
            sign = -1;
            index++;
        } else if (str[0] == '+') {
            sign = 1;
            index++;
        }

        while (index < str.size() && str[index] == '0')
            index++;

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

    // Default constructor
    BigInt() : sign(1), digits(1, 0) {}

    // Add two BigInt objects
    BigInt operator+(const BigInt& other) const {
        BigInt result;
        if (this->sign == other.sign) {
            result.digits = addVectors(this->digits, other.digits);
            result.sign = this->sign;
        } else {
            int cmp = compareAbs(other);
            if (cmp == 0) {
                return BigInt("0");
            } else if (cmp > 0) {
                result.digits = subtractVectors(this->digits, other.digits);
                result.sign = this->sign;
            } else {
                result.digits = subtractVectors(other.digits, this->digits);
                result.sign = other.sign;
            }
        }
        result.removeLeadingZeros();
        return result;
    }

    // Subtract two BigInt objects
    BigInt operator-(const BigInt& other) const {
        return *this + (-other);
    }

    // Negate a BigInt
    BigInt operator-() const {
        BigInt result(*this);
        if (!(result.digits.size() == 1 && result.digits[0] == 0))
            result.sign = -result.sign;
        return result;
    }

    // Multiply two BigInt objects
    BigInt operator*(const BigInt& other) const {
        BigInt result;
        if ((digits.size() == 1 && digits[0] == 0) || (other.digits.size() == 1 && other.digits[0] == 0))
            return BigInt("0");

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

        while (res.size() > 1 && res.back() == 0)
            res.pop_back();

        result.digits = res;
        result.sign = this->sign * other.sign;
        if (result.digits.size() == 1 && result.digits[0] == 0)
            result.sign = 1;
        return result;
    }

    // Divide two BigInt objects
    BigInt operator/(const BigInt& other) const {
        if (this->compareAbs(other) < 0)
            return BigInt("0");

        BigInt dividendAbs = *this;
        dividendAbs.sign = 1;
        BigInt divisorAbs = other;
        divisorAbs.sign = 1;

        string dividendStr = dividendAbs.toString();
        string quotientStr = "";
        BigInt remainder("0");

        for (char ch : dividendStr) {
            remainder = remainder * BigInt("10");
            remainder = remainder + BigInt(string(1, ch));

            int count = 0;
            while (remainder.compareAbs(divisorAbs) >= 0) {
                remainder = remainder - divisorAbs;
                count++;
            }
            quotientStr.push_back('0' + count);
        }

        int pos = quotientStr.find_first_not_of('0');
        if (pos != string::npos)
            quotientStr = quotientStr.substr(pos);
        else
            quotientStr = "0";

        BigInt result(quotientStr);
        result.sign = this->sign * other.sign;
        if (result.digits.size() == 1 && result.digits[0] == 0)
            result.sign = 1;
        return result;
    }

    // Modulo operation
    BigInt operator%(const BigInt& other) const {
        return *this - ((*this) / other) * other;
    }

    // Equality comparison
    bool operator==(const BigInt& other) const {
        return this->toString() == other.toString();
    }

    // Less than comparison
    bool operator<(const BigInt& other) const {
        if (this->sign != other.sign)
            return this->sign < other.sign;
        if (digits.size() != other.digits.size())
            return (this->sign == 1) ? (digits.size() < other.digits.size()) : (digits.size() > other.digits.size());
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i])
                return (this->sign == 1) ? (digits[i] < other.digits[i]) : (digits[i] > other.digits[i]);
        }
        return false;
    }

    // Less than or equal comparison
    bool operator<=(const BigInt& other) const {
        return (*this < other) || (*this == other);
    }

    // Greater than comparison
    bool operator>(const BigInt& other) const {
        return !(*this <= other);
    }

    // Greater than or equal comparison
    bool operator>=(const BigInt& other) const {
        return !(*this < other);
    }

    // Output the BigInt as a string
    friend ostream& operator<<(ostream& out, const BigInt& num) {
        out << num.toString();
        return out;
    }
};

int main() {
    string a, b;
    cout << "Enter the first number: ";
    cin >> a;
    cout << "Enter the second number: ";
    cin >> b;

    BigInt num1(a), num2(b);
    BigInt sum = num1 + num2;
    BigInt difference = num1 - num2;
    BigInt product = num1 * num2;
    BigInt division = num1 / num2;

    cout << "Sum: " << sum << endl;
    cout << "Difference: " << difference << endl;
    cout << "Product: " << product << endl;
    cout << "Division: " << division << endl;

    return 0;
}