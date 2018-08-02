
//-----------------------------------------------------------------------------
// Utilities functions
// Gustavo Rodriguez (agnasg)
// license GNU Lesser General Public License v2.1
// Permissions
//	Commercial use
//	Modification
//	Distribution
//	Private use
// Limitations
//	Liability
//	Warranty
//-----------------------------------------------------------------------------

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex>		// using std::regex_search
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "Globals.h"

// Multipropose functions that I have in every project.
// Disclaimer: There are some silly and hardcore functions here (Fasten your seat belt)

// random
int				getRand(int x, int y);
std::string		strim(const std::string str);

// Make this string lowercase and return the result
std::string stolower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);

	return str;
}

// Return a string representation of the given integer
std::string itos (int num)
{
	static std::stringstream converter;
	
	converter.str( std::string() );
	converter.clear();
	converter << num;
	return converter.str();
}

// Return a string representation of the given integer (unsigned int num)
std::string itos (unsigned int num)
{
	static std::stringstream converter;
	
	converter.str( std::string() );
	converter.clear();
	converter << num;
	return converter.str();
}

// Return a string representation of the given float
std::string ftos (float f)
{
	std::stringstream converter;
	converter << f;
	return converter.str();
}


// find substring (case insensitive)
// find str2 in str1
bool findi(const std::string& str1, const std::string& str2)
{
	if ("Floumoy" == str1) {
		int s2 = 9;
		s2++;
	}
	u32 size = str2.size();
	return (strnicmp(str1.c_str(), str2.c_str(), size) == 0);
}

// return true if the string is a valid number, false otherwise
bool isNumber (std::string s)
{
	std::string::const_iterator it;

	for (it = s.begin(); it != s.end() && std::isdigit(*it); ++it);
    
	return !s.empty() && it == s.end();
}

// 05-02-17
// separate a text in lines using '\n' as separator
// return how many lines found
// Note: this insert the '\n' around the string's width
// 22-03-17: ignore tags like #xxxxxx the line color.

u32 nroff0(std::string text, std::vector<std::string>& lines, u32 width)
{
	
	std::stringstream ss(text);
	std::string to;
	u32 lineno = 0;

	std::size_t found = text.find("engage");
	if (found!=std::string::npos) {
		int aa = 1;
		aa++;
	}

	u32 size = 0;
	std::string line;
	bool first = true;
	while(std::getline(ss, to, ' ')) {
		if (to.find("#") != 0) {			// this string has some '#' we need to investigate
			const std::string s = to;		// std::regex_search require const argument
			std::regex rgx("^#(\\w+):(.*)"); // match[1]=ff0101, match[2]=msg after the ':'
			std::smatch match;
			std::string s0 = s; // back to a no const string. wtf
			if (std::regex_search(s.begin(), s.end(), match, rgx)) { // this is to be sure we have #xxxxxx: pattern
				std::size_t begin = s0.find("#");
				s0.erase(begin, 8);	// strlen ("#xxxxxx:") == 8
			}
			size += s0.size ();	// we use the string without the "#xxxxxx:" to calculate the size ()
		} else {
			size += to.size ();
		}
		
		if (size < width) {
			if (first) {
				first = false;	
			} else {
				line += " ";
				size++;
			}
			line += to;
			continue;
		}
		// strim (line) remove \n\r\t
		lines.push_back (strim (line));
		line.clear ();
		line += to;
		size = to.size (); // <-
		lineno++;
	}
	if (line.size ()) {
		// strim (line) remove \n\r\t
		lines.push_back (strim (line));
		lineno++;
	}
	return lineno;
}

// separate a text in lines using '\n' as separator
// return how many lines found
u32 nroff(std::string text, std::vector<std::string>& lines, u32 width)
{
	std::stringstream ss(text);
	std::string line;
	// split line by '\n'
	u32 lineno = 0;
	while (std::getline(ss, line, '\n')){
		lineno += nroff0(line, lines, width);
	}

	return lineno;
}

// trim the string

std::string  strim(const std::string str)
{
	static std::string s;
	s = str;
	size_t startpos = s.find_first_not_of(" \n\r\t");
	if (std::string::npos != startpos)
	{
		s = s.substr(startpos);
	}

	s.erase(s.find_last_not_of(" \n\r\t") + 1);

	// remove any extra internal '\n'
	s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());

	return s;
}

// this handle simple json input like "{"arg1": "val1", "arg2": "val2"}"
void json_decode(std::string json, std::map<std::string, std::string>& result)
{
	int parent = 0;
	int elem = 0;
	int iskey = 1;
	std::string key, value;

	// Iterate over sequence elements and 
	for (std::string::iterator itr = json.begin(); itr != json.end(); itr++) {
		switch (*itr) {
		case '{':
			parent++;
			break;
		case '}':
			parent--;
			break;
		case '"':
			if (elem) {
				elem--;
				// falling down	
			} else {
				elem++;
				break;
			}
			// falling down
		case ':':
			iskey = 0;
			break;
		case ',':
			result[key] = value;
			//g_write_debug ("[%s] => [%s]", key.c_str (), value.c_str ());
			iskey = 1;
			key.erase(key.begin(), key.end());
			value.erase(value.begin(), value.end());
			break;
		case ' ': case '\t':
			if (!elem) { // if not in an element skip it
				break;
			}
			// falling down
		default:
			if (iskey) {
				key += *itr;
			} else {
				value += *itr;
			}
			break;
		}
	}
	result[key] = value;

}

// convert first letter to upper case
// like the php function
std::string ucword(std::string str)
{
	std::string result = str;
	result[0] = toupper(result[0]);

	return result;
}

// return a random code using letters and numebers
std::string makeRandomCode (u32 size)
{
	std::string salt = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string code;
	u32 dash = getRand (1, salt.size ()-1);
	for (u32 i = 0; i < size; i++) {
		if (i == dash) {
			code += "-";
			continue;
		}
		u32 r = getRand (0, salt.size ());
		code += salt[r];
	}

	return code;
}

std::string makeRandomShipName (u32 size)
{
	std::string lowercaseletters = "abcdefghijklmnopqrstuvwxyz";
	std::string uppercaseletters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string digits = "0123456789";

	std::string name;
	for (u32 i = 0; i < 4; i++) {
		u32 r = getRand (0, uppercaseletters.size ()-1);
		name += uppercaseletters[r];
	}
	name += "-";
	for (u32 i = 0; i < 2; i++) {
		u32 r = getRand (0, digits.size ()-1);
		name += itos (digits[r]-'0');
	}
	return name;
}

// like php str_replace
std::string& str_replace(const std::string &search, const std::string &replace, std::string &subject)
{
    std::string buffer;
 
    int sealeng = search.length();
    int strleng = subject.length();
 
    if (sealeng==0)
        return subject;//no change
 
    for(int i=0, j=0; i<strleng; j=0 ) {
        while (i+j<strleng && j<sealeng && subject[i+j]==search[j])
            j++;
        if (j==sealeng) {//found 'search'
            buffer.append(replace);
            i+=sealeng;
        } else {
            buffer.append( &subject[i++], 1);
        }
    }
    subject = buffer;
    return subject;
}

// Return the string representation of a number
std::string numberToStringHex (DWORD n)
{
    std::stringstream stream;
    stream << std::hex << n;
    std::string result(stream.str());
    return result;
}

// Return the number given the string representation
int stringHexToNumber (std::string value)
{
    int x;
    std::stringstream ss;
    ss << std::hex << value;
    ss >> x;

    return x;
}

// this convert std::string with UTF-8 to encoding Windows1252
// that is the encoding used by VS and DX.
// seems this is compatible with (ISO8859 ISO 8859) not sure
std::string UTF8toWindows1252(const char * in)
{
    std::string out;
    if (in == NULL)
        return out;

    unsigned int codepoint;
	// the spanish conversion table, but you can add more here if you like.
	// use https://www.utf8-chartable.de/unicode-utf8-table.pl
	std::string conv =  "áéíóúñÁÉÍÓÚÑ¿¡·°»¢";
    while (*in != 0)
    {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch == 0xc3) {
            ++in;
			ch = static_cast<unsigned char>(*in);
			if (ch == 0xa1) {
				out.append(1, static_cast<char>(conv[0])); // á
			} else if (ch == 0xa9) {
				out.append(1, static_cast<char>(conv[1])); // é
			} else if (ch == 0xad) {
				out.append(1, static_cast<char>(conv[2])); // í
			} else if (ch == 0xb3) {
				out.append(1, static_cast<char>(conv[3])); // ó
			} else if (ch == 0xba) {
				out.append(1, static_cast<char>(conv[4])); // ú
			} else if (ch == 0xb1) {
				out.append(1, static_cast<char>(conv[5])); // ñ
			} else if (ch == 0x81) {
				out.append(1, static_cast<char>(conv[6])); // Á
			} else if (ch == 0x89) {
				out.append(1, static_cast<char>(conv[7])); // É
			} else if (ch == 0x8d) {
				out.append(1, static_cast<char>(conv[8])); // Í
			} else if (ch == 0x93) {
				out.append(1, static_cast<char>(conv[9])); // Ó
			} else if (ch == 0x9a) {
				out.append(1, static_cast<char>(conv[10])); // Ú
			} else if (ch == 0x91) {
				out.append(1, static_cast<char>(conv[11])); // Ñ
			} else {
				out.append(1, static_cast<char>(conv[17])); // ¢ not recognized
			}
			++in;
		} else if (ch == 0xc2) { // c2 bf
			++in;
			ch = static_cast<unsigned char>(*in);
			if (ch == 0xbf) {
				out.append(1, static_cast<char>(conv[12])); // ¿
			} else if (ch == 0xa1) {
				out.append(1, static_cast<char>(conv[13])); // ¡
			} else if (ch == 0xb7) {
				out.append(1, static_cast<char>(conv[14])); // ·
			} else if (ch == 0xb0) {
				out.append(1, static_cast<char>(conv[15])); // ° DEGREE SIGN
			} else if (ch == 0xbb) {
				out.append(1, static_cast<char>(conv[16])); // »
			} else {
				out.append(1, static_cast<char>(conv[17])); // ¢ not recognized
			} 
			++in;
		} else {
			out.append(1, static_cast<char>(ch));
			++in;
		}
    }
    return out;
}
