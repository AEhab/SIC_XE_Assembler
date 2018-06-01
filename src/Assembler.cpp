#include<bits/stdc++.h>
using namespace std;

map<char, char> reg; // regname, no
map<string, pair<int, int> > Dir;
map<string, pair<int, string> > Instruction; //intrnam, format , opcode
map<string, string> Symbol_Table; // symbol , location
map<string, pair<string, int> > EXTSYMT;
vector<string> EXTREF;
vector<string> EXTDEF;
map<int, pair<string, pair<string, string> > > line; // id , lcount, inst,operants
map<int, pair<string, pair<string, pair<string, pair<int, int> > > > > Literal_Table; // counter of literal , lcount , value, address, length, pool
vector<pair<string, pair<string, string> > > Modification; // address, 05/06 , +/-
map<int, string> ObjCode;
int id;
int Literal_counter = 0;
int literal_pool = 1, lastliter_count = 0;
bool LTFlag = false;
bool n, i, b, p, x, e;
int pro_cou = 0;
int last_prog_len = 0;
string Disp_OR_Address, INSTR;
string BASE;

string FromHexToBin(string s, int from, int WalkForward) {
	stringstream ss;
	ss << hex << s;
	unsigned n;
	ss >> n;
	bitset<32> b(n);
	s = b.to_string();
	return s.substr(from, WalkForward);
}

string FromDecToBin(int x) {
	string binary = std::bitset<8>(x).to_string(); //to binary
	return binary;
}

string FromBinToHex(string s) {
	bitset<32> set(s);
	stringstream res;
	res << hex << uppercase << set.to_ulong();
	return res.str();
}

int FromHexToDec(string s) {
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << s;
	ss >> x;
	// output it as a signed type
	return static_cast<int>(x);
}

string FromDecToHex(int x) {
	std::stringstream ss;
	ss << std::hex << x; // int decimal_value
	string ans, res(ss.str());
	transform(res.begin(), res.end(), res.begin(), ::toupper);
	for (int i = 0; i < 6 - (int) res.size(); i++)
		ans += '0';
	ans += res;
	return ans;
}

string FromDecToHex_nozero(int x) {
	std::stringstream ss;
	ss << std::hex << x; // int decimal_value
	string res(ss.str());
	transform(res.begin(), res.end(), res.begin(), ::toupper);
	return res;
}

string FromDecToString(int x) {
	std::stringstream ss;
	ss << std::dec << x; // int decimal_value
	string res(ss.str());
	return res;
}

string TwosComplement(string s) {
	int sz = s.size();
	int i = sz - 1;
	bool First1 = false;
	while (i >= 0) {
		if (First1) {
			if (s[i] == '0')
				s[i] = '1';
			else
				s[i] = '0';
		}
		if (s[i] == '1')
			First1 = true;
		i--;
	}
	return s;
}

void reading_Reg() {

//Open File To fill map of registers within it's numbers
	ifstream Register("Reg.txt");
	char RegName, RegNo;
	if (Register.is_open()) {
		while (Register >> RegName >> RegNo)
			reg[RegName] = RegNo;
		Register.close();
	} else
		cout << "Error file is closed (not open)" << endl;
}

void reading_directives() {
	//Open File To fill map of Directives with it's reserving value
	ifstream Directive("Directives.txt");
	string Dir_name;
	int Dir_ID;
	int Dir_value;
	if (Directive.is_open()) {
		while (Directive >> Dir_name >> Dir_ID >> Dir_value)
			Dir[Dir_name] = make_pair(Dir_ID, Dir_value);
		Directive.close();
	} else
		cout << "Error file is closed (not open)" << endl;
}

void reading_InstructionSet() {
	//Open File To fill in map of instruction with opcode & it's format
	ifstream Instr("InstructionSet.txt");
	string InstrName;
	int format;
	string OPCODEInHexa;
	if (Instr.is_open()) {
		while (Instr >> InstrName >> format >> OPCODEInHexa) {
			Instruction[InstrName].first = format;
			Instruction[InstrName].second = OPCODEInHexa;
			//  cout<<InstrName<<" "<<Instruction[InstrName].first <<" "<<Instruction[InstrName].second<<endl;
		}
		Instr.close();
	} else
		cout << "Error file is closed (not open)" << endl;
}

int FormatNo(string s) {
	if (Instruction[s].first == 1)
		return 1;
	else if (Instruction[s].first == 2)
		return 2;
	else if (Instruction[s].first == 3)
		return 3;
	else if (Instruction[s].first == 4)
		return 4;

	return 0;
}

void Dir_Handle(stringstream &ssin, string str) {
	if (Dir[str].first == 1) {
		int value;
		ssin >> value;
		line[id].second.second = FromDecToString(value);
		line[id + 1].first = FromDecToHex_nozero(
				value * Dir[str].second + FromHexToDec(line[id].first));
	} else if (Dir[str].first == 2) {
		string data = "";
		if (str == "BYTE") {
			ssin >> data;
			if (data[0] == 'C')
				line[id + 1].first = FromDecToHex_nozero(
						data.size() - 3 + FromHexToDec(line[id].first)); // the size with out C'..'
			else if (data[0] == 'X')
				line[id + 1].first = FromDecToHex_nozero(
						((data.size() - 3) / 2) + FromHexToDec(line[id].first)); // the size with out X'..' also every 2 hex in 1 byte
			else {
				cout << "error in BYTE definition" << endl;
				exit(1);
			}
		} else if (str == "WORD") {
			ssin >> data;
			int arraylen = 1;
			for (int i = 0; i < (int) data.size(); i++) {
				if (data[i] == ',')
					arraylen++;
			}
			line[id + 1].first = FromDecToHex_nozero(
					3 * arraylen + FromHexToDec(line[id].first));
			if (isdigit(data[i]) == false) {
				Modification.push_back(
						make_pair(
								FromDecToHex_nozero(
										FromHexToDec(line[id].first)),
								make_pair("06", data)));
			}
		} else {
			cout << "error\n";
		}
		line[id].second.second = data;
	} else {
		cout << "error in DIRECTIVE definition" << endl;
		exit(1);
	}
}

string convertToASCII(string letter) {
	string coo;
	for (int i = 0; i < (int) letter.length(); i++) {
		char x = letter.at(i);
		coo = coo + FromDecToHex_nozero(x);
	}
	return coo;
}

void saving_literal(string str) {

	int flag = 1, flg = 1;
	LTFlag = true;
	for (int i = 0; i < Literal_counter; i++) {
		if (str[1] == 'X')
			if (FromHexToDec(Literal_Table[i].second.first)
					== FromHexToDec(str.substr(3, str.length() - 4)))
				flag = 0;
		if (str[1] == 'C')
			if (FromHexToDec(Literal_Table[i].second.first)
					== FromHexToDec(
							convertToASCII(str.substr(3, str.length() - 4))))
				flg = 0;
	}
	if (str[1] == 'X' && flag == 1) {
		Literal_Table[Literal_counter].first = str; //literal
		Literal_Table[Literal_counter].second.first = str.substr(3,
				str.length() - 4); // hex
		Literal_Table[Literal_counter].second.second.first = '0'; // Address
		Literal_Table[Literal_counter].second.second.second.first = ceil(
				(str.length() - 4) / 2.0); // length
		Literal_Table[Literal_counter].second.second.second.second =
				literal_pool; //pool
		Literal_counter++;
	} else if (str[1] == 'C' && flg == 1) {
		string cpy = str.substr(3, str.length() - 4);
		Literal_Table[Literal_counter].first = str; //literal
		Literal_Table[Literal_counter].second.first = convertToASCII(cpy); // char
		Literal_Table[Literal_counter].second.second.first = '0'; // Address
		Literal_Table[Literal_counter].second.second.second.first =
				(str.length() - 4); // length
		Literal_Table[Literal_counter].second.second.second.second =
				literal_pool; //pool
		Literal_counter++;
	}
}

void NegativeDisp(int disp) {
	string str = FromDecToHex_nozero(abs(disp));
	int k = str.size();
	str = FromHexToBin(str, 32 - k * 4 - 1, k * 4);
	Disp_OR_Address = 'F' + FromBinToHex(TwosComplement(str));
}

void n_iFlg(int z) {
	string str = line[z].second.second;
	int sz = str.size();
	if (str[0] == '@') {
		i = 0;
		n = 1;
	} else if (str[0] == '#') {
		i = 1;
		n = 0;
		b = 0;
		p = 0;
		if (isdigit(str[1])) {
			str = str.substr(1, sz - 1);
			int n = atoi(str.c_str());
			Disp_OR_Address = FromDecToHex_nozero(n);
		}
	} else {
		i = 1;
		n = 1;
	}
}

void Xflg(int z) {
	string str = line[z].second.second;
	int n = str.size();
	if (str[n - 2] == ',' && str[n - 1] == 'X')
		x = 1;
}

void b_OR_p(int z, int Fno) {
	int disp;
	string str = line[z].second.second;
	int sz = str.size();

	if (str[0] == '#' && !isdigit(str[1]))
		str = str.substr(1, sz - 1);

	if (str[0] >= 'A' && str[0] <= 'Z') {
		if (str[sz - 2] == ',' && str[sz - 1] == 'X'
				&& Symbol_Table.find(str.substr(0, sz - 2))
						!= Symbol_Table.end()) {
			if (Fno == 4)
				Disp_OR_Address = Symbol_Table[str.substr(0, sz - 2)];
			else
				disp = FromHexToDec(Symbol_Table[str.substr(0, sz - 2)])
						- FromHexToDec(line[z + 1].first);
		} else if (Symbol_Table.find(str) != Symbol_Table.end()) {
			if (Fno == 4)
				Disp_OR_Address = Symbol_Table[str];
			else
				disp = FromHexToDec(Symbol_Table[str])
						- FromHexToDec(line[z + 1].first);
		} else if (str[0] == '@'
				&& Symbol_Table.find(str.substr(1, sz - 1))
						!= Symbol_Table.end()) {
			if (Fno == 4)
				Disp_OR_Address = Symbol_Table[str.substr(1, sz - 1)];
			else
				disp = FromHexToDec(Symbol_Table[str.substr(1, sz - 1)])
						- FromHexToDec(line[z + 1].first);
		}
	}
	//cout<<FromHexToDec(Symbol_Table[str.substr(0,sz-3)])<<" "<<FromHexToDec(line[z].first)<<endl;
	//cout<<disp<<endl;

//if it's format 4 b&p equal zero 3altooool ??
	if (Fno != 4) {
		if (disp <= 2047 && disp >= -2048) {
			p = 1, b = 0;
			if (disp < 0)
				NegativeDisp(disp);
			else
				Disp_OR_Address = FromDecToHex_nozero(disp);
		} else {
			p = 0, b = 1;
			if (BASE == "") {
				cout << "Error Base needed" << endl;
				exit(1);
			}
			if (str[0] >= 'A' && str[0] <= 'Z') {
				if (str[sz - 2] == ',' && str[sz - 1] == 'X'
						&& Symbol_Table.find(str.substr(0, sz - 2))
								!= Symbol_Table.end())
					disp = FromHexToDec(Symbol_Table[str.substr(0, sz - 2)])
							- FromHexToDec(Symbol_Table[BASE]);
				else if (Symbol_Table.find(str) != Symbol_Table.end())
					disp = FromHexToDec(Symbol_Table[str])
							- FromHexToDec(Symbol_Table[BASE]);
			}
			if (disp < 0) {
				cout << "Error Check your Format 3 in line " << z << endl;
				exit(1);
			} else {
				Disp_OR_Address = FromDecToHex_nozero(disp);
				//If disp greater than 12 bit ....
			}
		}
	}
	// cout<<Disp_OR_Address<<endl;
}

string AddingFlagsToString(bool z, string bin) {
	if (z)
		bin += "1";
	else
		bin += "0";
	return bin;
}

void CreateObjCode(int z, int Fno) {

	string str = "", bin, INSTR = line[z].second.first;
	bin = FromHexToBin(Instruction[INSTR].second, 32 - 8, 6);

	bin = AddingFlagsToString(n, bin);
	bin = AddingFlagsToString(i, bin);
	bin = AddingFlagsToString(x, bin);
	bin = AddingFlagsToString(b, bin);
	bin = AddingFlagsToString(p, bin);
	bin = AddingFlagsToString(e, bin);
//cout<<bin<<endl;
	int sz = Disp_OR_Address.size();
	if (Fno == 3)
		sz = 3 - sz;
	if (Fno == 4)
		sz = 5 - sz;
	for (int k = 0; k < sz; k++)
		str += "0";

	string leadingZero = "", str2 = FromBinToHex(bin);
	sz = str2.size();
	sz = 3 - sz;
	for (int k = 0; k < sz; k++)
		leadingZero += "0";

	//cout<<leadingZero<<FromBinToHex(bin)<<" "<<str2<<" "<<str<<" "<<Disp_OR_Address<<endl;
	ObjCode[z] = leadingZero + str2 + str + Disp_OR_Address;
}

void SetFlags() {

	for (int l = 0; l < id; l++) {

		INSTR = line[l].second.first;
		string str = line[l].second.second;
		int sz = str.size();
		int Fnumber;
		if (Instruction.find(INSTR) != Instruction.end()) {
			Fnumber = FormatNo(line[l].second.first);
			if (Fnumber == 1) {
				ObjCode[l] = Instruction[INSTR].second;
			} else if (Fnumber == 2) {
				if (sz > 1)
					ObjCode[l] += Instruction[INSTR].second + reg[str[0]]
							+ reg[str[2]];
				else
					ObjCode[l] += Instruction[INSTR].second + reg[str[0]] + '0';
			} else if (INSTR == "RSUB") {
				ObjCode[l] = "4F0000";
			} else if (INSTR == "+RSUB") {
				ObjCode[l] = "4F000000";
			} else if (Fnumber == 3) {
				e = 0;
				n_iFlg(l);
				Xflg(l);
				b_OR_p(l, Fnumber);
				CreateObjCode(l, Fnumber);
			} else if (Fnumber == 4) {
				e = 1;
				n_iFlg(l);
				Xflg(l);
				b_OR_p(l, Fnumber);
				b = 0;
				p = 0;
				CreateObjCode(l, Fnumber);
			}
		} else {

			if (INSTR == "BYTE" || INSTR == "WORD") {
				// cout<<INSTR<<" "<<str<<" "<<sz<<endl;
				if (str[0] == 'C')
					ObjCode[l] = convertToASCII(str.substr(2, sz - 3));
				else if (str[0] == 'X')
					ObjCode[l] = str.substr(2, sz - 3);
				else {
					string s = "";
					for (int y = 0; y < sz; y++) {
						if (str[y] == ',') {
							ObjCode[l] += FromDecToHex(atoi(s.c_str()));
							s = "";
						} else
							s += str[y];

					}
					ObjCode[l] += FromDecToHex(atoi(s.c_str()));
				}
			}
		}
		//False Flags
		n = false;
		i = n, b = n, x = n, p = n, e = n;
		Disp_OR_Address = "", INSTR = "", str = "", Fnumber = 0;
	}
}

void printtab(int n, ofstream &out) {
	for (int i = 0; i < n; i++)
		out << "\t";
}

string printme(int n, char c) {
	string str;
	for (int i = 0; i < n; i++)
		str += c;
	return str;
}

void Printing_data() {
	map<int, pair<string, pair<string, string> > >::const_iterator it1;
	ofstream LOC, Sym, Literal, out;
	// locations and lines
	if (pro_cou == 1) {
		LOC.open("LOC.txt");
		Sym.open("Symbol_Table.txt");
		Literal.open("Literal_Table.txt");
		out.open("output.txt");
	} else {
		LOC.open("LOC.txt", std::fstream::app);
		LOC << endl;
		Sym.open("Symbol_Table.txt", std::fstream::app);
		Sym << endl;
		Literal.open("Literal_Table.txt", std::fstream::app);
		Literal << endl;
		out.open("output.txt", std::fstream::app);
		out << endl;
	}

	LOC << "LINE_ID\t\tLINE_ADDRESS\tINSTRUCTION\tOPERANDS\n";
	for (it1 = line.begin(); it1 != line.end(); it1++) {
		LOC << it1->first << "\t\t\t"
				<< printme(6 - (int) it1->second.first.size(), '0')
				<< it1->second.first << "\t\t\t" << it1->second.second.first;
		4 > (int) it1->second.second.first.size() ?
				printtab(3, LOC) : printtab(2, LOC);
		LOC << it1->second.second.second << "\n";
	}
	LOC.close();
	// symbol table
	Sym << "Symbol\tAddress\n";
	map<string, string>::iterator it;
	for (it = Symbol_Table.begin(); it != Symbol_Table.end(); it++)
		Sym << it->first << "\t" << printme(6 - (int) it->second.size(), '0')
				<< it->second << "\n";
	Sym.close();
	// literal table
	Literal << "Literal\t\t\tValue\t\tAddress\t\tLength\t\tPool\n";
	//
	for (auto it = Literal_Table.begin(); it != Literal_Table.end(); it++) {
		pair<string, pair<string, pair<string, pair<int, int> > > > cur =
				it->second;
		Literal << cur.first << "\t\t\t" << cur.second.first << "\t\t"
				<< cur.second.second.first << "\t\t"
				<< cur.second.second.second.first << "\t\t\t"
				<< cur.second.second.second.second << "\n";
	}
	Literal.close();
	// external for every program
	for (int i = 0; i < (int) EXTDEF.size(); i++)
		EXTSYMT[EXTDEF[i]].first = FromDecToHex(
				FromHexToDec(Symbol_Table[EXTDEF[i]]) + last_prog_len);
	//output filE
	out << "H." << line[0].second.first
			<< printme((6 - (int) line[0].second.first.size()), ' ') << "."
			<< printme((6 - (int) line[0].first.size()), '0') << line[0].first
			<< "." << printme((6 - (int) line[id].first.size()), '0')
			<< line[id].first << endl;
	last_prog_len = FromHexToDec(line[id].first);

	for (int i = 0; i < (int) EXTREF.size(); i++) {
		if (i == 0)
			out << "R.";
		else
			out << ".";
		out << EXTREF[i] << printme((6 - (int) EXTREF[i].size()), ' ');
	}
	out << endl;
	for (int i = 0; i < (int) EXTDEF.size(); i++) {
		if (i == 0)
			out << "D.";
		else
			out << ".";
		out << EXTDEF[i] << printme((6 - (int) EXTDEF[i].size()), ' ') << ".";
		out << printme((6 - (int) Symbol_Table[EXTDEF[i]].size()), '0')
				<< Symbol_Table[EXTDEF[i]];
	}
	out << '\n';
	int i = 0;
	string myline;
	while (i < id) {
		out << "T." << printme((6 - (int) line[i].first.size()), '0')
				<< line[i].first << ".";
		while (i < id
				&& (myline.size() + (int) ObjCode[i].size() <= 60
						|| (myline.size() == 0 && (int) ObjCode[i].size() > 60))) {
			if (myline.size() == 0 && (int) ObjCode[i].size() > 60) {
				myline = ObjCode[i].substr(0, 59);
				ObjCode[i].erase(0, 59);
				i--;
				break;
			} else
				myline += ObjCode[i];
			i++;
		}

		string hexme = FromDecToHex_nozero((int) myline.size() / 2);
		out << printme(2 - (int) hexme.size(), '0') << hexme << "." << myline
				<< endl;
		myline = "";
	}
	for (int i = 0; i < (int) Modification.size(); i++) {
		char a = '+';
		size_t pos1 = 0, pos2 = 0;
		string token;
		while (Modification[i].second.second.find('+') != string::npos
				|| Modification[i].second.second.find('-') != string::npos) {
			pos1 = Modification[i].second.second.find('+');
			pos2 = Modification[i].second.second.find('-');
			if (pos1 < pos2
					&& Modification[i].second.second.find('+')
							!= string::npos) {
				token = Modification[i].second.second.substr(0, pos1);
				Modification[i].second.second.erase(0, pos1 + 1);
				if (Symbol_Table.find(token) != Symbol_Table.end())
					out << "M."
							<< printme(6 - (int) Modification[i].first.size(),
									'0') << Modification[i].first << "."
							<< Modification[i].second.first << a
							<< line[0].second.first << endl;
				else
					out << "M."
							<< printme(6 - (int) Modification[i].first.size(),
									'0') << Modification[i].first << "."
							<< Modification[i].second.first << a << token
							<< endl;
				a = '+';
			}

			if (pos1 > pos2
					&& Modification[i].second.second.find('-')
							!= string::npos) {
				token = Modification[i].second.second.substr(0, pos2);
				Modification[i].second.second.erase(0, pos2 + 1);

				if (Symbol_Table.find(token) != Symbol_Table.end()) {
					out << "M."
							<< printme(6 - (int) Modification[i].first.size(),
									'0') << Modification[i].first << "."
							<< Modification[i].second.first << a
							<< line[0].second.first << endl;
				} else {
					out << "M."
							<< printme(6 - (int) Modification[i].first.size(),
									'0') << Modification[i].first << "."
							<< Modification[i].second.first << a << token
							<< endl;
				}
				a = '-';
			}
		}
		token = Modification[i].second.second;
		if (Symbol_Table.find(token) != Symbol_Table.end()) {
			out << "M." << printme(6 - (int) Modification[i].first.size(), '0')
					<< Modification[i].first << "."
					<< Modification[i].second.first << a << line[0].second.first
					<< endl;

		} else {
			out << "M." << printme(6 - (int) Modification[i].first.size(), '0')
					<< Modification[i].first << "."
					<< Modification[i].second.first << a << token << endl;
		}
	}
	out << "E." << line[0].second.first
			<< printme((6 - (int) line[0].second.first.size()), ' ') << "."
			<< printme((6 - (int) line[0].first.size()), '0') << line[0].first;
	out.close();
	cout << ObjCode[2] << endl;
}

bool ParseLine(string s) {
	string str, START_LABEL;
	bool INSTR_flg = false, START_flg = false;
	stringstream ssin(s);

	while (ssin.good()) {
		ssin >> str;
		if (str == "") {
			line[id + 1].first = line[id].first;
			break;
		}
		if (START_flg) {
			line[id].first = str;
			line[id + 1].first = str;
			pro_cou++;
			Symbol_Table[START_LABEL] = line[id].first;
			line[id].second.first = START_LABEL;
			START_flg = false;
		} else if (INSTR_flg) {
			line[id].second.second = str;
			INSTR_flg = false;
			if (str[0] == '=') {
				saving_literal(str);
			}
			if (str[0] != '#' && FormatNo(INSTR) == 4) {
				Modification.push_back(
						make_pair(
								FromDecToHex_nozero(
										1 + FromHexToDec(line[id].first)),
								make_pair("05", str)));
			}
		} else if (Instruction.find(str) != Instruction.end()) {
			line[id].second.first = str;
			INSTR_flg = true;
			line[id + 1].first = FromDecToHex_nozero(
					FormatNo(str) + FromHexToDec(line[id].first));
			INSTR = str;
		} else if (Dir.find(str) != Dir.end()) {
			line[id].second.first = str;
			// directives handling
			Dir_Handle(ssin, str);

		} else {
			if (str == "START") {
				START_flg = true;
			} else if (str == "BASE") {
				ssin >> BASE;
				id--;
			} else if (str == "END") {
				cout << "DONE\n";
				string dummy;
				ssin >> dummy;
				return true;
			} else if (str == "LTORG") {
				int b;
				LTFlag = false;
				if (lastliter_count == Literal_counter) {
					id--;
					literal_pool++;
					continue;
				}
				Literal_Table[lastliter_count].second.second.first =
						line[id].first;
				line[id].second.first = str;
				for (int i = lastliter_count; i < Literal_counter - 1; i++) {
					b = FromHexToDec(Literal_Table[i].second.second.first)
							+ Literal_Table[i].second.second.second.first;
					Literal_Table[i + 1].second.second.first =
							FromDecToHex_nozero(b);
				}
				b =
						FromHexToDec(
								Literal_Table[Literal_counter - 1].second.second.first)
								+ Literal_Table[Literal_counter - 1].second.second.second.first;
				line[id + 1].first = FromDecToHex_nozero(b);
				literal_pool++;
				lastliter_count = Literal_counter;
			} else if (str == "EXTREF") {
				id--;
				string sx;
				ssin >> sx;
				string delimiter = ",";
				size_t pos = 0;
				string token;
				while ((pos = sx.find(delimiter)) != string::npos) {
					token = sx.substr(0, pos);
					EXTREF.push_back(token);
					sx.erase(0, pos + delimiter.length());
				}
				EXTREF.push_back(sx);
			} else if (str == "EXTDEF") {
				id--;
				string sx;
				ssin >> sx;
				string delimiter = ",";
				size_t pos = 0;
				string token;
				while ((pos = sx.find(delimiter)) != string::npos) {
					token = sx.substr(0, pos);
					EXTDEF.push_back(token);
					EXTSYMT[token].second = pro_cou;
					sx.erase(0, pos + delimiter.length());
				}
				EXTDEF.push_back(sx);
				EXTSYMT[sx].second = pro_cou;
			} else {
				if (id == 0)
					START_LABEL = str;
				Symbol_Table[str] = line[id].first;
			}
		}
	}
	return false;
}

void reading_AssemblyCode(string str) {
//Read File Of Assembly code
	ifstream AssemblyCode("code2.txt");
	string s, Rline;
	bool new_cont_sec = false;
	line[0].first = "0000";

	if (AssemblyCode.is_open()) {
		while (getline(AssemblyCode, Rline)) {
			transform(Rline.begin(), Rline.end(), Rline.begin(), ::toupper);
			new_cont_sec = ParseLine(Rline);
			if (new_cont_sec == true) {
				SetFlags(); // set the flags and generate the object code
				// clear every thing and count from the first execpt linker table
				Printing_data();
				id = -1;
				literal_pool = 1;
				Literal_counter = 0;
				lastliter_count = 0;
				LTFlag = false;
				n = i = b = p = x = e = false;
				Disp_OR_Address = INSTR = BASE = "";
				Symbol_Table.clear();
				line.clear();
				Literal_Table.clear(); // counter of literal , lcount , value, address, length, pool
				Modification.clear();
				ObjCode.clear();
				EXTDEF.clear();
				EXTREF.clear();
				line[0].first = "0000";
				new_cont_sec = false;
			}
			id++;
		}
		AssemblyCode.close();
	} else
		cout << "Error file is closed (not open)" << endl;

	if (LTFlag) {
		Literal_Table[lastliter_count].second.second.first = line[id - 1].first;
		for (int i = lastliter_count; i < Literal_counter - 1; i++) {
			int b = FromHexToDec(Literal_Table[i].second.second.first)
					+ Literal_Table[i].second.second.second.first;
			Literal_Table[i + 1].second.second.first = FromDecToHex_nozero(b);
		}
	}
}

int main() {
	string str = "code2.txt"; // = "code.txt"   //  file name
//Initial definitions for some data
	reading_Reg();
	reading_InstructionSet();
	reading_directives();
// Reading and Parsing the code
	reading_AssemblyCode(str);
	cout << "external for every program\nProg NO\tSymbol\tLocation\n";
	for (auto it = EXTSYMT.begin(); it != EXTSYMT.end(); it++)
		cout << (*it).second.second << "\t" << (*it).first << "\t"
				<< (*it).second.first << "\n";
	cout << "finished";
	cout << endl;
}
