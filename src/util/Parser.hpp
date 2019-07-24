#ifndef _UTIL_PARSER_HPP_
#define _UTIL_PARSER_HPP_

#include <cmath>
#include <deque>
using std::deque;
#include <stack>
using std::stack;
#include <string>
using std::string;

namespace util {
    class Parser {
    public:
        explicit Parser() {}

        virtual ~Parser() {}

        // Evaluates an integer expression using Shunting-Yard algorithm.
        int eval(const string& expression, const map<string, int>& consts = {}) {
            string expr = expression;
            if (!consts.empty()) {
                expr = replaceConsts(expr, consts);
            }

            tokenize(expr);

            int status = infix2postfix();
            if (status > 0) {
                return compute();
            }

            return status;
        }

    private:
        string replaceConsts(const string& expr, const map<string, int>& consts) {
            string result = expr;
            for (const auto& iter : consts) {
                result = Strings::replace(result, iter.first, to_string(iter.second), true);
            }
            return result;
        }

        void tokenize(const string &expr) {
            _tokens.clear();
            string number;
            for (char chr : expr) {
                if (isdigit(chr)) {
                    number += chr;
                } else {
                    if (!number.empty()) {
                        _tokens.push_back(number);
                        number = "";
                    }
                    _tokens.push_back(string(1, chr));
                }
            }
        }

        int infix2postfix() {
            stack<string> stack;
            stack.push("");
            for (const string& token : _tokens) {
                char first = token[0];
                if (isdigit(first)) {
                    _postfix.push_back(token);
                } else if (first == '(') {
                    stack.push(token);

                } else if (first == ')') {
                    while (!stack.top().empty() && stack.top() != "(") {
                        _postfix.push_back(stack.top());
                        stack.pop();
                    }
                    if (stack.top() == "(") {
                        stack.pop();
                    }
                } else {        //If an operator is scanned
                    while (!stack.top().empty() && precedence(token[0]) <= precedence(stack.top()[0])) {
                        _postfix.push_back(stack.top());
                        stack.pop();
                    }
                    stack.push(token);
                }
            }
            while(!stack.top().empty()) {
                _postfix.push_back(stack.top());
                stack.pop();
            }
            return _postfix.size();
        }

        int precedence(char c) {
            if(c == '^')
                return 3;
            else if(c == '*' || c == '/')
                return 2;
            else if(c == '+' || c == '-')
                return 1;
            else
                return -1;
        }

        int compute() {
            // Compute from postfix stack...
            stack<int> stack;
            for (const string& token : _postfix) {
                if (isdigit(token[0])) {
                    stack.push(stoi(token));
                } else {
                    int val1 = stack.top();
                    stack.pop();
                    int val2 = stack.top();
                    stack.pop();

                    switch (token[0]) {
                        case '+':
                            stack.push(val2 + val1);
                            break;
                        case '-':
                            stack.push(val2 - val1);
                            break;
                        case '*':
                            stack.push(val2 * val1);
                            break;
                        case '/':
                            stack.push(val2 / val1);
                            break;
                        case '^':
                            stack.push((int) pow(val2, val1));
                            break;
                    }
                }
            }
            return stack.top();
        }

        deque<string> _tokens;
        vector<string> _postfix;
    };
}

#endif // _UTIL_PARSER_HPP_
