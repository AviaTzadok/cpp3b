#include "NumberWithUnits.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
constexpr double EPS=0.0001;

using namespace std;

namespace ariel{
    static map<string, map<string, double> > map;

NumberWithUnits::NumberWithUnits(double value, const string& unit){
         if(map.find(unit) == map.end()){
            throw invalid_argument{"Unit doesn't exist in the units table!"};
        }
        this->value = value;
        this->unit = unit;
}

double convert(double value, const string& from, const string& to){
        if(from == to) {return value;}
        if(map[from][to] == 0){
           throw invalid_argument{"Units do not match - ["+from+"] cannot be converted to ["+to+"]"};
          }
        return value * map[from][to];
}

int compare(const NumberWithUnits& num1, const NumberWithUnits& num2){
        double converted = convert(num2.value, num2.unit, num1.unit);
        double diff = num1.value - converted;
        int result = 0;
        if (!(abs(diff) < EPS))
        {
            if (diff > EPS)
            {
                result = 1;
            }
            else
            {
                result = -1;
            }
        }
        return result;
    
}

void NumberWithUnits::read_units(ifstream& infile){
        string eq;
        string unit1;
        string unit2;
        double num1 = 0;
        double num2 = 0;
        while(infile >> num1 >> unit1 >> eq >> num2 >> unit2) {
            map[unit1][unit2] = num2;
            map[unit2][unit1] = 1/num2;


            for(auto &pair : map[unit2]) {
                double w = map[unit1][unit2] * pair.second;
                map[unit1][pair.first] = w;
                map[pair.first][unit1] = 1/w;
            }

            for(auto &pair : map[unit1]) {
                double w = map[unit2][unit1] * pair.second;
                map[unit2][pair.first] = w;
                map[pair.first][unit2] = 1/w;
            }
        }
}
        NumberWithUnits NumberWithUnits::operator+(const NumberWithUnits& c) const{
             double converted = convert(c.value, c.unit, this->unit);
            NumberWithUnits ans(this->value+converted, this->unit);
            return ans;
        }
        NumberWithUnits& NumberWithUnits::operator+=(const NumberWithUnits& c){
            double converted = convert(c.value, c.unit, this->unit);
            this->value+=converted;
            return *this;
        }
        NumberWithUnits NumberWithUnits::operator+() const{
            return NumberWithUnits{this->value, this->unit};
        }
        NumberWithUnits NumberWithUnits::operator-(const NumberWithUnits& c)const{
            double converted = convert(c.value, c.unit, this->unit);
            NumberWithUnits ans(this->value-converted, this->unit);
            return ans;
        }
        NumberWithUnits& NumberWithUnits::operator-=(const NumberWithUnits& c){
            double converted = convert(c.value, c.unit, this->unit);
            this->value-=converted;
            return *this;
        }
        NumberWithUnits NumberWithUnits::operator-() const{
            return NumberWithUnits(-this->value, this->unit);
        }
        NumberWithUnits NumberWithUnits::operator++(int c){
            NumberWithUnits temp =  NumberWithUnits(this->value, this->unit);
            this->value++;
            return temp;
        }
        NumberWithUnits NumberWithUnits::operator--(int c){
            NumberWithUnits temp =  NumberWithUnits(this->value, this->unit);
            this->value--;
            return temp;
        }
        NumberWithUnits& NumberWithUnits::operator++(){
             this->value+=1;
             return *this;
        }
        NumberWithUnits& NumberWithUnits::operator--(){
             this->value-=1;
             return *this;
        }

        
    bool operator>(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) > 0;
    }
    bool operator>=(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) >= 0;
    }
    bool operator<(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) < 0;
    }
    bool operator<=(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) <= 0;
    }
    bool operator==(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) == 0;
    }
    bool operator!=(const NumberWithUnits& c1, const NumberWithUnits& c2){
        return compare(c1, c2) != 0;
    }



    NumberWithUnits operator*(const NumberWithUnits& c2, double num) {
        return NumberWithUnits(c2.value*num, c2.unit);
    }
    NumberWithUnits operator*(double c1,const NumberWithUnits& n) {
        return NumberWithUnits(n.value*c1, n.unit);
    }    
    ostream& operator<< (std::ostream& output, const NumberWithUnits & c){
                        return (output<<c.value<<"["<<c.unit<<"]");
    }

    static istream& getAndCheckNextCharIs(istream& input, char expectedChar) {
    char actualChar=0;
    input >> actualChar;
    if (!input) {return input;}

    if (actualChar!=expectedChar) {
        // failbit is for format error
        input.setstate(ios::failbit);
    }
    return input;
    }
    istream& operator>>(istream& input, NumberWithUnits& c){
    ios::pos_type startPosition = input.tellg();
        double num=0;
        string str;
    if ( (!(input >> num))|| (!getAndCheckNextCharIs(input,'['))  ||(!(input >> str)) ) {

        // rewind on error
        auto errorState = input.rdstate(); // remember error state
        input.clear(); // clear error so seekg will work
        input.seekg(startPosition); // rewind
        input.clear(errorState); // set back the error flag
    } else {
        if(str[str.length()-1]==']'){
            str=str.substr(0,str.length()-1);
        }else{
             getAndCheckNextCharIs(input,']');
        }
        if(map.find(str) == map.end()){
            throw invalid_argument{"Unit doesn't exist in the units table!"};
        }
        c.unit=str;
        c.value=num;
       
    }

    return input;
    }
}