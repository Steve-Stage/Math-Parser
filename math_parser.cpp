/*
* Math expression parser
* Supports brackets, math operations: +, -, * or x, / or :, %, ^
* Credit: SteveState
* @steve_stage
*/

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <conio.h>

enum
{
	DT_DOUBLE = 0, DT_CHAR
};

class unknown_type
{
	double dt1 = 0;
	char dt2 = 0.0;
	int16_t dataType = DT_DOUBLE;
public:
	unknown_type(const double& i) :dt1(i) {}
	unknown_type(const char& i) : dt2(i), dataType(DT_CHAR) {}
	int16_t getType()
	{
		return dataType;
	}
	void setDouble(const double& i)
	{
		dt1 = i;
		dt2 = 0;
		dataType = DT_DOUBLE;
	}
	void setChar(const char& i)
	{
		dt1 = 0.0;
		dt2 = i;
		dataType = DT_CHAR;
	}
	const double& getDouble()
	{
		return dt1;
	}
	const char& getChar()
	{
		return dt2;
	}
};

class brackets_container
{
	std::vector<std::list<unknown_type>::iterator> pwrs, mltpls, addtns;
	bool isFnshd = false, isRpt = false;
public:
	brackets_container() : isFnshd(false), isRpt(false) {}
	std::vector<std::list<unknown_type>::iterator>& powers()
	{
		return pwrs;
	}
	std::vector<std::list<unknown_type>::iterator>& multiplies()
	{
		return mltpls;
	}
	std::vector<std::list<unknown_type>::iterator>& additions()
	{
		return addtns;
	}
	void setEnd()
	{
		isFnshd = true;
	}
	void setRepeat(const bool& i)
	{
		isRpt = i;
	}
	const bool& isFinished()
	{
		return isFnshd;
	}
	const bool& isRepeat()
	{
		return isRpt;
	}
};

bool math_prior(const std::string&, double&);

bool math_calculate(std::list<unknown_type>&, double&);

int main()
{
	std::string to_parse;
	std::cout << "Enter expression: ";
	std::getline(std::cin, to_parse);
	double result;
	if (!math_prior(to_parse, result))
		std::cout << "NaN" << std::endl;
	else
		std::cout << result << std::endl;
	_getch();
	return 0;
}

bool math_prior(const std::string& expr, double& res)
{
	int num = 0, exp = 0;
	bool isFloating = false;
	bool isPositive = true;
	int16_t exp_dgr = 0;
	std::list<unknown_type> l; // we are removing much elements and don't want them to move, so list is our choice
	char before = expr[0];
	auto write_num = [](int& n, int& e, int16_t& e_d, bool& isfloat, bool& ispositive, std::list<unknown_type>& lt) -> void
	{
		if (!isfloat)
		{
			if (!ispositive)
			{
				n = -n;
				ispositive = true;
			}
			lt.push_back(unknown_type(double(n)));
			n = 0;
		}
		else
		{
			double tp = double(n) + (double(e) / std::pow(10, e_d));
			if (!ispositive)
			{
				tp = -tp;
				ispositive = true;
			}
			e_d = 0;
			lt.push_back(unknown_type(tp));
			n = 0;
			e = 0;
			e_d = 0;
		}
		isfloat = false;
	};
	for (int i = 0; i < expr.size(); i++)
	{
		char s = expr[i];
		if (s >= '0' && s <= '9')
		{
			if (before == ')' && !isFloating)
			{
				l.push_back(unknown_type('*'));
			}
			if (!isFloating)
			{
				num *= 10;
				num += int(expr[i]) - 48;
			}
			else
			{
				exp *= 10;
				exp += int(expr[i]) - 48;
				exp_dgr++;
			}
			if (i == (expr.size() - 1))
			{
				if (!isFloating)
				{
					l.push_back(unknown_type(double(num)));
				}
				else
				{
					double tp = double(num) + (double(exp) / std::pow(10, exp_dgr));
					l.push_back(unknown_type(tp));
				}
			}
		}
		else if (s == '.')
		{
			if (isFloating)
				return false;
			else
			{
				if (before == ')')
				{
					l.push_back(unknown_type('*'));
				}
				isFloating = true;
			}
		}
		else if (s == ')')
		{
			if (before == '+' || before == '-')
			{
				l.erase(std::next(l.begin(), l.size()-1));
			}
			else if (before == '*' || before == '/' || before == 'x' || before == ':' || before == '%' || before == '^')
			{
				return false;
			}
			else
			{
				write_num(num, exp, exp_dgr, isFloating, isPositive, l);
			}
			l.push_back(unknown_type(s));
		}
		else if (s == '(')
		{
			if ((before >= '0' && before <= '9') || before == '.')
			{
				write_num(num, exp, exp_dgr, isFloating, isPositive, l);
				l.push_back(unknown_type('*'));
			}
			else if (before == ')')
				l.push_back(unknown_type('*'));
			l.push_back(unknown_type(s));
		}
		else if (s == '+')
		{
			if ((before >= '0' && before <= '9') || before == '.')
			{
				write_num(num, exp, exp_dgr, isFloating, isPositive, l);
			}
			else if (before != ')')
				continue;
			l.push_back(unknown_type(s));
		}
		else if (s == '-')
		{
			if ((before >= '0' && before <= '9') || before == '.')
			{
				write_num(num, exp, exp_dgr, isFloating, isPositive, l);
			}
			else if (before == '-' && i != 0)
				return false;
			else if ((before != ')' && i != 0) || i == 0)
			{
				isPositive = false;
				continue;
			}
			l.push_back(unknown_type(s));
		}
		else if (s == '*' || s == '/' || s == 'x' || s == ':' || s == '%' || s == '^')
		{
			if ((before >= '0' && before <= '9') || before == '.')
			{
				write_num(num, exp, exp_dgr, isFloating, isPositive, l);
			}
			else if (before != ')')
				return false;
			l.push_back(unknown_type(s));
		}
		else if (s != ' ')
			return false;
		before = s;
	}
	return math_calculate(l, res);
}

bool math_calculate(std::list<unknown_type>& l, double& r)
{
	std::vector<brackets_container> exprs = { brackets_container() };
	int expr_cntr = 0, br_cntr = 0, expr_offset = 0;
	bool frmr_end_bracket = 0;
	for (int i = 0; i < l.size(); i++)
	{
		auto p = std::next(l.begin(), i);
		auto& u = *p;
		if (u.getType() == DT_CHAR)
		{
			auto& uc = u.getChar();
			if (u.getChar() == '(')
			{
				frmr_end_bracket = false;
				if ((*exprs.rbegin()).isFinished())
				{
					expr_cntr += 1 + expr_offset;
					exprs.push_back(brackets_container());
					exprs[expr_cntr].setRepeat(true);
				}
				else
				{
					expr_cntr++;
					exprs.push_back(brackets_container());
				}
				p = l.erase(p);
				i--;
			}
			else if (u.getChar() == ')')
			{
				if (frmr_end_bracket)
				{
					exprs[expr_cntr].setEnd();
					expr_cntr--;
				}
				else
				{
					exprs[expr_cntr].setEnd();
					expr_cntr -= 1 + expr_offset;
					frmr_end_bracket = true;
				}
				expr_offset++;
				p = l.erase(p);
				i--;
			}
			else if (u.getChar() == '^')
			{
				exprs[expr_cntr].powers().push_back(p);
			}
			else if (u.getChar() == '*' || u.getChar() == 'x' || u.getChar() == '/' || u.getChar() == ':'\
				|| u.getChar() == '%')
			{
				exprs[expr_cntr].multiplies().push_back(p);
			}
			else if (u.getChar() == '+' || u.getChar() == '-')
			{
				exprs[expr_cntr].additions().push_back(p);
			}
		}
	}
	if (expr_cntr != 0)
		return false;
	std::list<unknown_type>::iterator last_e;
	for (auto it = exprs.rbegin(); it != exprs.rend(); it++) // brackets iteration
	{
		auto& e = *it;
		for (auto itp = e.powers().rbegin(); itp != e.powers().rend(); itp++)
		{
			last_e = (*itp);
			auto prev_e = std::prev(last_e);
			auto next_e = std::next(last_e);
			double pwr = std::pow((*prev_e).getDouble(), (*next_e).getDouble());
			(*last_e).setDouble(pwr);
			l.erase(prev_e);
			l.erase(next_e);
		}
		for (auto itm = e.multiplies().begin(); itm != e.multiplies().end(); itm++)
		{
			last_e = (*itm);
			auto prev_e = std::prev(last_e);
			auto next_e = std::next(last_e);
			double mpl = 0.0;
			switch ((*last_e).getChar())
			{
				case '*':
				{
					mpl = (*prev_e).getDouble() * (*next_e).getDouble();
					break;
				}
				case 'x':
				{
					mpl = (*prev_e).getDouble() * (*next_e).getDouble();
					break;
				}
				case '/':
				{
					if ((*next_e).getDouble() == 0.0)
						return false;
					else
						mpl = (*prev_e).getDouble() / (*next_e).getDouble();
					break;
				}
				case ':':
				{
					if ((*next_e).getDouble() == 0.0)
						return false;
					else
						mpl = (*prev_e).getDouble() / (*next_e).getDouble();
					break;
				}
				case '%':
				{
					if ((*next_e).getDouble() == 0.0)
						return false;
					else
						mpl = std::fmod((*prev_e).getDouble(), (*next_e).getDouble());
					break;
				}
				default:
				{
					break;
				}
			}
			(*last_e).setDouble(mpl);
			l.erase(prev_e);
			l.erase(next_e);
		}
		for (auto ita = e.additions().begin(); ita != e.additions().end(); ita++)
		{
			last_e = (*ita);
			auto prev_e = std::prev(last_e);
			auto next_e = std::next(last_e);
			double add = 0.0;
			switch ((*last_e).getChar())
			{
				case '+':
				{
					add = (*prev_e).getDouble() + (*next_e).getDouble();
					break;
				}
				case '-':
				{
					add = (*prev_e).getDouble() - (*next_e).getDouble();
					break;
				}
				default:
				{
					break;
				}
			}
			(*last_e).setDouble(add);
			l.erase(prev_e);
			l.erase(next_e);
		}
	}
	r = (*last_e).getDouble();
	return true;
}