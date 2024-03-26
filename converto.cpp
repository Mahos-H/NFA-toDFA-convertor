#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

using namespace std;

class transition {
private:
    set<int> init;
    char sym;
    set<int> fin;
public:
    transition(int i, char s, int f) : sym(s) {
        init.insert(i);
        fin.insert(f);
        cout << "Added transition from " << i << " with " << sym << " to " << f << "\n";
    }
    transition(set<int> i, char s, set<int> f) : init(i), sym(s), fin(f){}
    set<int>& initial()  {
        return init;
    }
    char symbol()  {
        return sym;
    }
    set<int>& final()  { 
        return fin;
    }
};

class nfa {
private:
    int start;
    set<int> states;
    vector<transition> table;
    set<int> final_states;
public:
    nfa(int x) : start(x) {
        cout << "NFA created with start state " << start << "\n";
        states.insert(x);
    }
    void insert(int i) {
        states.insert(i);
    }
    void final(int f) {
        final_states.insert(f);
    }
    void add(int i, char s, int f) {
        table.push_back(transition(i, s, f));
    }
    int get_start()  {
        return start;
    }
    set<int>& get_states()  { 
        return states;
    }
    vector<transition>& get_table()  { 
        return table;
    }
    set<int>& get_final_states()  { 
        return final_states;
    }
};

class dfa {
private:
    int start;
    set<set<int>> states;
    vector<transition> table;
    set<set<int>> final_states;
public:
    dfa(int x) : start(x) {
        cout << "DFA created with start state " << start << "\n";
        states.insert({x});
    }
    void adder(vector<pair<set<int>, unordered_map<char, set<int>>>>& a, nfa& n) {
        for(const auto& i : a){
            states.insert(i.first);
            for(const auto& j : i.second){
                table.push_back(transition(i.first,j.first, j.second)); 
            }
            for(int k : n.get_final_states()){
                if(i.first.find(k) != i.first.end()){
                    final_states.insert(i.first);
                }
            }
        }

    }
    void final(int f) {
        final_states.insert({f});
    }
    void add(int i, char s, int f) {
        table.push_back(transition(i, s, f));
    }
    int get_start()  {
        return start;
    }
    set<set<int>>& get_states()  { // Return reference to the states set
        return states;
    }
    vector<transition>& get_table()  { // Return reference to the transition table
        return table;
    }
    set<set<int>>& get_final_states()  { 
        return final_states;
    }
};
dfa convert(nfa& n) {
    dfa d(n.get_start());
    unordered_map<char, set<int>> x;
    vector<set<int>> nvy; // not visited yet
    set<set<int>> visited;
    for (auto& t : n.get_table()) {
        if (t.initial().find(n.get_start()) != t.initial().end()) {
            char symbol = t.symbol();
            if (x.find(symbol) == x.end()) {
                x[symbol] = {};
            }
            x[symbol].insert(t.final().begin(), t.final().end());
        }
    }
    visited.insert({n.get_start()});
    for(auto& i: x){
        nvy.push_back(i.second);
    }
    pair<set<int>, unordered_map<char, set<int>>> k;
    k.first.insert(n.get_start());
    k.second = x;
    vector<pair<set<int>, unordered_map<char, set<int>>>> v;
    v.push_back(k);
    while(!nvy.empty()){
        auto a = nvy.back();
        nvy.pop_back();
        if (visited.find(a) != visited.end()) continue; 
        visited.insert(a);
        x.clear();
        for(int j: a){
            for (auto& t : n.get_table()) {
                if (t.initial().find(j) != t.initial().end()) {
                    char symbol = t.symbol();
                    if (x.find(symbol) == x.end()) {
                        x[symbol] = {};
                    }
                    x[symbol].insert(t.final().begin(), t.final().end());
                }
            }
        }
        k.first = a;
        k.second = x;
        v.push_back(k);
        for(auto& i: x){
            for(auto& j : v){
                if (i.second == j.first) continue; 
                if (!includes(j.first.begin(), j.first.end(), i.second.begin(), i.second.end())) {
                    nvy.push_back(i.second);
                    break;
                }
            }  
        }
    }
    d.adder(v,n);
    return d;
}

int main() {
    int start, end, state;
    int state_num, symbol_num, transition_num, fin_state_num;
    char sym;
    cout<<"-----NFA to DFA convertor (We are not consdering epsilon transtitions)-----"<<endl;
    cout<<"-----(Please ensure that the states are numbered as integers, that is state 1,2,etc and the symbols are alphabets)-----"<<endl;
    cout<<"Enter the number of states in NFA: ";
    cin>>state_num;
    cout<<"Enter the number of symbols in NFA: ";
    cin>>symbol_num;
    cout<<"Enter the number of transitions in NFA: ";
    cin>>transition_num;
    cout<<"Enter the number of final states in NFA: ";
    cin>>fin_state_num;
    cout<<"Enter the start state of the NFA: ";
    cin>>start;
    nfa N(start);
    cout<<"Enter the states of the NFA(excluding the start state): \n";
    for(int i=1;i<state_num;i++){
        cout<<"Enter the state: ";
        cin>>start;
        N.insert(start);
    }
    cout<<"Enter the final states of the NFA: \n";
    for(int i=0;i<fin_state_num;i++){
        cout<<"Enter the state: ";
        cin>>end;
        N.final(end);
    }
    cout<<"Enter the transitions of the NFA in the format <initial state number><space><symbol><space><final state number>: \n";
    for(int i=0;i<transition_num;i++){
        cout<<"Enter the transition: ";
        cin>>state>>sym>>end;
        N.add(state,sym,end);
    }
    dfa myDFA = convert(N);
    cout << "NFA States: \n{ ";
    for (int state : N.get_states()) {
        cout << state << " ";
    }
    cout<<"}";
    cout << "\nNFA Final States: \n{ ";
    for (int final_state : N.get_final_states()) {
        cout << final_state << " ";
    }
    cout<<"}";
    cout << "\nDFA Transition Table: \n";
    for( auto& transition : N.get_table()) {
        cout << "Initial State: { ";
        for(int state : transition.initial()) {
            cout << state << " ";
        }
        cout << "} Symbol: " << transition.symbol() << " Final States: { ";
        for(int state : transition.final()) {
            cout << state << " ";
        }
        cout << "}\n";
    }
    cout << "\nDFA States: ";
    for (set<int> state : myDFA.get_states()) {
        cout<<"{ ";
        for(auto j = state.begin(); j != state.end(); ++j) {
            int ce = *j;
            cout<<ce<<" ";
        }
        cout<<"}";
        
    }
    cout << "\nDFA Final States: ";
    for (auto final_state : myDFA.get_final_states()) {
        cout<<"{ ";
        for( int f : final_state){
            cout << f << " ";
        }
        cout<<"}";
    }
    cout << "\nDFA Transition Table: \n";
    for( auto& transition : myDFA.get_table()) {
        cout << "Initial State: { ";
        for(int state : transition.initial()) {
            cout << state << " ";
        }
        cout << "} Symbol: " << transition.symbol() << " Final States: { ";
        for(int state : transition.final()) {
            cout << state << " ";
        }
        cout << "}\n";
    }
    cout<<"-----(Note if the DFA transition table is missing a transition, it means that the transition goes to the dead/null state)-----";
    return 0;
}
