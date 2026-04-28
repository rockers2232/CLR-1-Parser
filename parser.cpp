#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <queue>
#include <cctype>

using namespace std;

struct Rule { int id; string lhs; vector<string> rhs; };
struct Item { 
    int r; 
    int d; 
    string la; 
    bool operator<(const Item& o) const { 
        if(r != o.r) return r < o.r;
        if(d != o.d) return d < o.d;
        return la < o.la;
    } 
};
typedef set<Item> State;

vector<Rule> rules = {
    {0, "S'", {"PROG"}},
    {1, "PROG", {"PREAMBLE", "FUNC"}},
    {2, "PROG", {"FUNC"}},
    {3, "PREAMBLE", {"#include<iostream>", "using", "namespace", "std", ";"}},
    {4, "FUNC", {"type", "main", "(", ")", "{", "STMTS", "}"}},
    {5, "STMTS", {"STMT", "STMTS"}},
    {6, "STMTS", {"STMT"}},
    {7, "STMT", {"type", "DECLS", ";"}},
    {8, "DECLS", {"id", "=", "EXPR", ",", "DECLS"}},
    {9, "DECLS", {"id", "=", "EXPR"}},
    {10, "DECLS", {"id", ",", "DECLS"}},
    {11, "DECLS", {"id"}},
    {12, "STMT", {"cout", "<<", "EXPR", ";"}},
    {13, "STMT", {"id", "=", "EXPR", ";"}},
    {14, "STMT", {"id", "incdec", ";"}},
    {15, "STMT", {"while", "(", "COND", ")", "{", "STMTS", "}"}},
    {16, "STMT", {"if", "(", "COND", ")", "{", "STMTS", "}", "else", "{", "STMTS", "}"}},
    {17, "STMT", {"if", "(", "COND", ")", "{", "STMTS", "}"}},
    {18, "STMT", {"for", "(", "FOR_INIT", "COND", ";", "FOR_UPD", ")", "{", "STMTS", "}"}},
    {19, "FOR_INIT", {"type", "id", "=", "EXPR", ";"}},
    {20, "FOR_INIT", {"id", "=", "EXPR", ";"}},
    {21, "FOR_UPD", {"id", "=", "EXPR"}},
    {22, "FOR_UPD", {"id", "incdec"}},
    {23, "COND", {"EXPR", "relop", "EXPR"}},
    {24, "EXPR", {"EXPR", "op", "TERM"}},
    {25, "EXPR", {"TERM"}},
    {26, "TERM", {"id"}},
    {27, "TERM", {"num"}}
};

bool isTerm(string s) { return !isupper(s[0]); }

map<string, set<string>> firsts, follows;
map<State, int> state_id;
vector<State> states;
map<pair<int, string>, string> actionTable;
map<pair<int, string>, int> gotoTable;

void computeFirsts() {
    for(auto& r : rules) for(auto& s : r.rhs) if(isTerm(s)) firsts[s].insert(s);
    bool changed = true;
    while(changed) {
        changed = false;
        for(auto& r : rules) {
            for(string sym : r.rhs) {
                for(string f : firsts[sym]) {
                    if(firsts[r.lhs].insert(f).second) changed = true;
                }
                break; 
            }
        }
    }
}

void computeFollows() {
    follows[rules[0].lhs].insert("$");
    bool changed = true;
    while(changed) {
        changed = false;
        for(auto& r : rules) {
            for(size_t i = 0; i < r.rhs.size(); i++) {
                string B = r.rhs[i];
                if(isTerm(B)) continue;
                bool nextHasEps = true;
                for(size_t j = i + 1; j < r.rhs.size(); j++) {
                    string next = r.rhs[j];
                    for(string f : firsts[next]) {
                        if(follows[B].insert(f).second) changed = true;
                    }
                    nextHasEps = false; break; 
                }
                if(nextHasEps) {
                    for(string f : follows[r.lhs]) {
                        if(follows[B].insert(f).second) changed = true;
                    }
                }
            }
        }
    }
}

set<string> getFirstOfSequence(vector<string> seq, string lookahead) {
    set<string> res;
    bool allEps = true;
    for(string s : seq) {
        bool hasEps = false;
        for(string f : firsts[s]) {
            if(f != "eps") res.insert(f);
            else hasEps = true;
        }
        if(!hasEps) { allEps = false; break; }
    }
    if(allEps) res.insert(lookahead);
    return res;
}

State closure(State I) {
    State J = I;
    bool changed = true;
    while(changed) {
        changed = false;
        State to_add;
        for(Item it : J) {
            if(it.d < (int)rules[it.r].rhs.size()) {
                string B = rules[it.r].rhs[it.d];
                if(!isTerm(B)) {
                    vector<string> beta;
                    for(size_t k = it.d + 1; k < rules[it.r].rhs.size(); k++) beta.push_back(rules[it.r].rhs[k]);
                    set<string> la_set = getFirstOfSequence(beta, it.la);
                    
                    for(auto& r : rules) {
                        if(r.lhs == B) {
                            for(string b : la_set) {
                                Item nit = {r.id, 0, b};
                                if(J.find(nit) == J.end() && to_add.find(nit) == to_add.end()) {
                                    to_add.insert(nit); changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        for(Item nit : to_add) J.insert(nit);
    }
    return J;
}

State goTo(State I, string X) {
    State J;
    for(Item it : I) {
        if(it.d < (int)rules[it.r].rhs.size() && rules[it.r].rhs[it.d] == X) {
            J.insert({it.r, it.d + 1, it.la});
        }
    }
    return closure(J);
}

void buildTable() {
    State I0 = closure({{0, 0, "$"}});
    states.push_back(I0);
    state_id[I0] = 0;
    queue<State> q; q.push(I0);

    set<string> symbols;
    for(auto& r: rules) { symbols.insert(r.lhs); for(auto& s: r.rhs) symbols.insert(s); }

    while(!q.empty()) {
        State I = q.front(); q.pop();
        int i = state_id[I];

        for(string X : symbols) {
            State J = goTo(I, X);
            if(J.empty()) continue;
            if(state_id.find(J) == state_id.end()) {
                state_id[J] = states.size();
                states.push_back(J);
                q.push(J);
            }
            int j = state_id[J];
            if(isTerm(X)) actionTable[{i, X}] = "S" + to_string(j);
            else gotoTable[{i, X}] = j;
        }

        for(Item it : I) {
            if(it.d == (int)rules[it.r].rhs.size()) {
                if(it.r == 0 && it.la == "$") actionTable[{i, "$"}] = "ACC";
                else if (it.r != 0) {
                    if (actionTable.find({i, it.la}) == actionTable.end() || actionTable[{i, it.la}][0] == 'R')
                        actionTable[{i, it.la}] = "R" + to_string(it.r);
                }
            }
        }
    }
}

struct Token { string type; string val; string sym; int line; };

vector<Token> tokenize(string raw) {
    vector<Token> tks;
    int line = 1;
    string w = "";
    
    for(size_t i = 0; i < raw.length(); ++i) {
        if(raw[i] == '\n') { line++; continue; }
        if(isspace(raw[i])) continue;
        
        if(raw.substr(i, 18) == "#include<iostream>") {
            tks.push_back({"INC", "#include<iostream>", "#include<iostream>", line});
            i += 17; continue;
        }
        
        if(isalpha(raw[i]) || raw[i] == '_') {
            w = "";
            while(i < raw.length() && (isalnum(raw[i]) || raw[i] == '_')) {
                w += raw[i]; i++;
            }
            i--;
            if(w == "if" || w == "else" || w == "while" || w == "for" || w == "cout" || w == "using" || w == "namespace" || w == "std" || w == "main") {
                tks.push_back({"KEY", w, w, line});
            } else if (w == "int" || w == "float" || w == "char") {
                tks.push_back({"TYPE", w, "type", line});
            } else {
                tks.push_back({"ID", w, "id", line});
            }
        } else if(isdigit(raw[i])) {
            w = "";
            while(i < raw.length() && isdigit(raw[i])) {
                w += raw[i]; i++;
            }
            i--;
            tks.push_back({"NUM", w, "num", line});
        } else {
            string op = string(1, raw[i]);
            if(i + 1 < raw.length()) {
                string two = raw.substr(i, 2);
                if(two == "<<" || two == ">>" || two == "==" || two == "!=" || two == "<=" || two == ">=" || two == "++" || two == "--") {
                    if(two == "<<") tks.push_back({"OP", two, "<<", line});
                    else if(two == "==" || two == "!=" || two == "<=" || two == ">=") tks.push_back({"OP", two, "relop", line});
                    else if(two == "++" || two == "--") tks.push_back({"OP", two, "incdec", line});
                    else tks.push_back({"OP", two, two, line});
                    i++; continue;
                }
            }
            if(op == "+" || op == "-" || op == "*" || op == "/") tks.push_back({"OP", op, "op", line});
            else if(op == "<" || op == ">") tks.push_back({"OP", op, "relop", line});
            else tks.push_back({"SYM", op, op, line});
        }
    }
    tks.push_back({"EOF", "$", "$", line});
    return tks;
}

int main() {
    computeFirsts();
    computeFollows();
    buildTable();

    string source_code, line;
    while (getline(cin, line)) source_code += line + "\n";

    cout << "@GRAMMAR\n";
    for(size_t i=1; i<rules.size(); i++) {
        string rhs = "";
        for(auto& s: rules[i].rhs) rhs += s + " ";
        cout << rules[i].id << "|" << rules[i].lhs << "|" << rhs << "\n";
    }

    set<string> nts;
    for(auto& r: rules) if(r.id > 0) nts.insert(r.lhs);
    cout << "@FIRST_FOLLOW\n";
    for(string nt : nts) {
        string fst = "", fol = "";
        for(string f : firsts[nt]) fst += f + " ";
        for(string f : follows[nt]) fol += f + " ";
        cout << nt << "|" << fst << "|" << fol << "\n";
    }

    cout << "@ACTION_TABLE\n";
    for(auto const& [key, val] : actionTable) cout << key.first << "|" << key.second << "|" << val << "\n";
    cout << "@GOTO_TABLE\n";
    for(auto const& [key, val] : gotoTable) cout << key.first << "|" << key.second << "|" << val << "\n";

    cout << "@TRACE\n";
    if (source_code.empty()) { cout << "@ERROR\nNo input stream detected.\n"; return 1; }

    vector<Token> tokens = tokenize(source_code);
    stack<int> stateStack; stack<string> symbolStack;
    stateStack.push(0);
    size_t index = 0;
    string finalStatus = "Parsing Incomplete";

    while (index < tokens.size()) {
        vector<int> pStates; stack<int> tStates = stateStack;
        while (!tStates.empty()) { pStates.push_back(tStates.top()); tStates.pop(); }
        vector<string> pSyms; stack<string> tSyms = symbolStack;
        while (!tSyms.empty()) { pSyms.push_back(tSyms.top()); tSyms.pop(); }

        stringstream ssStack, ssInput;
        ssStack << "$0 ";
        int sId = pStates.size() - 2, symId = pSyms.size() - 1;
        while (symId >= 0 && sId >= 0) {
            ssStack << pSyms[symId] << " " << pStates[sId] << " ";
            symId--; sId--;
        }
        for (size_t i = index; i < tokens.size(); ++i) ssInput << tokens[i].val << " ";

        int cState = stateStack.top(); 
        string cTokenSym = tokens[index].sym;
        string cTokenVal = tokens[index].val;
        int cTokenLine = tokens[index].line;

        if (actionTable.find({cState, cTokenSym}) == actionTable.end()) {
            cout << ssStack.str() << "|" << ssInput.str() << "|Error\n";
            finalStatus = "REJECTED: Syntax Error near '" + cTokenVal + "' at Line " + to_string(cTokenLine); 
            break;
        }

        string action = actionTable[{cState, cTokenSym}];
        if (action[0] == 'S') {
            cout << ssStack.str() << "|" << ssInput.str() << "|" << action << "\n";
            stateStack.push(stoi(action.substr(1))); symbolStack.push(cTokenVal); index++;
        } else if (action[0] == 'R') {
            int rNum = stoi(action.substr(1)); string lhs = rules[rNum].lhs;
            string rhsStr = ""; for(auto& s: rules[rNum].rhs) rhsStr += s + " ";
            
            cout << ssStack.str() << "|" << ssInput.str() << "|" << action + " (" + lhs + "->" + rhsStr + ")\n";
            for (size_t i = 0; i < rules[rNum].rhs.size(); ++i) { stateStack.pop(); symbolStack.pop(); }
            int topState = stateStack.top();
            stateStack.push(gotoTable[{topState, lhs}]); symbolStack.push(lhs);
        } else if (action == "ACC") {
            cout << ssStack.str() << "|" << ssInput.str() << "|Accept\n";
            finalStatus = "ACCEPTED: Syntax is Correct!"; break;
        }
    }
    cout << "@STATUS\n" << finalStatus << "\n";
    return 0;
}