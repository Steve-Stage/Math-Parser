/*
* Math expression parser
* Supports brackets, math operations: +, -, * or x, / or :, %, ^
* Credit: SteveState
* @steve_stage
*/

#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <conio.h>

enum
{
	DT_DOUBLE = 0, DT_CHAR, DT_BUFFER
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
	void setBuffer()
	{
		dt1 = 0.0;
		dt2 = 0;
		dataType = DT_BUFFER;
	}
	double getDouble()
	{
		return dt1;
	}
	char getChar()
	{
		return dt2;
	}
};

class brackets_container
{
	std::vector<int> pwrs, mltpls, addtns;
	int pos_start, pos_end;
	bool isFnshd = false, isRpt = false;
public:
	brackets_container() : pos_start(0), pos_end(0), isFnshd(false), isRpt(false) {}
	brackets_container(const int& i) : pos_start(i), pos_end(0), isFnshd(false), isRpt(false) {}
	brackets_container(const int& i1, const int& i2) : pos_start(i1), pos_end(i2), isFnshd(true), isRpt(false) {}
	const int& getPosStart()
	{
		return pos_start;
	}
	const int& getPosEnd()
	{
		return pos_end;
	}
	void setPosStart(const int& i)
	{
		pos_start = i;
	}
	void setPosEnd(const int& i)
	{
		pos_end = i;
		isFnshd = true;
	}
	std::vector<int>& powers()
	{
		return pwrs;
	}
	std::vector<int>& multiplies()
	{
		return mltpls;
	}
	std::vector<int>& additions()
	{
		return addtns;
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

bool math_prior(std::string&, double&);

bool math_calculate(std::vector<unknown_type>&, double&);

int main()
{
	std::string to_parse;
	std::cout << "Enter expression: ";
	std::cin >> to_parse;
	double result;
	if (!math_prior(to_parse, result))
		std::cout << "NaN" << std::endl;
	else
		std::cout << result << std::endl;
	_getch();
	return 0;
}

bool math_prior(std::string& expr, double& res)
{
	int num = 0, exp = 0;
	bool isFloating = false;
	bool isEndBracket = false;;
	int16_t exp_dgr = 0;
	std::vector<unknown_type> v = { unknown_type('(')};
	for (int i = 0; i < expr.size(); i++)
	{
		char s = expr[i];
		if (s >= '0' && s <= '9')
		{
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
					v.push_back(unknown_type(double(num)));
				}
				else
				{
					double tp = double(num) + (double(exp) / std::pow(10, exp_dgr));
					exp_dgr = 0;
					v.push_back(unknown_type(tp));
				}
			}
		}
		else if (s == '.')
		{
			isFloating = true;
		}
		else if (s == ')')
		{
			if (!isFloating)
			{
				v.push_back(unknown_type(double(num)));
				num = 0;
			}
			else
			{
				double tp = double(num) + (double(exp) / std::pow(10, exp_dgr));
				exp_dgr = 0;
				v.push_back(unknown_type(tp));
				num = 0;
				exp = 0;
				exp_dgr = 0;
			}
			isFloating = false;
			v.push_back(unknown_type(s));
			isEndBracket = true;
		}
		else if (s == '(')
		{
			v.push_back(unknown_type(s));
		}
		else if (s == '+' || s == '-' || s == '*' || s == '/' || s == 'x' || s == ':' || s == '%' || s == '^')
		{
			if (!isEndBracket)
			{
				if (!isFloating)
				{
					v.push_back(unknown_type(double(num)));
					num = 0;
				}
				else
				{
					double tp = double(num) + (double(exp) / std::pow(10, exp_dgr));
					exp_dgr = 0;
					v.push_back(unknown_type(tp));
					num = 0;
					exp = 0;
					exp_dgr = 0;
				}
				isFloating = false;
			}
			else
			{
				isEndBracket = false;
			}
			v.push_back(unknown_type(s));
		}
	}
	v.push_back(unknown_type(')'));
	if (!math_calculate(v, res))
		return false;
	return true;
}

// x + (x - x) - (x * x)
// x / (x + (x - x) - (x * x) - (x % x) - x) + x

bool math_calculate(std::vector<unknown_type>& v, double& r)
{
	std::vector<brackets_container> exprs = { brackets_container(0, v.size()-1) }; // first - powers, second - multiplies
	int expr_cntr = 0, br_cntr = 0, expr_offset = 0;
	bool frmr_end_bracket = 0;
	for (int i = 1; i < v.size()-1; i++)
	{
		unknown_type& u = v[i];
		if (u.getType() == DT_CHAR)
		{
			if (u.getChar() == '(')
			{
				frmr_end_bracket = false;
				if ((*exprs.rbegin()).isFinished())
				{
					expr_cntr += 1 + expr_offset;
					exprs.push_back(brackets_container(i));
					exprs[expr_cntr].setRepeat(true);
				}
				else
				{
					expr_cntr++;
					exprs.push_back(brackets_container(i));
				}
			}
			else if (u.getChar() == ')')
			{
				if (frmr_end_bracket)
				{
					exprs[expr_cntr].setPosEnd(i);
					expr_cntr--;
				}
				else
				{
					exprs[expr_cntr].setPosEnd(i);
					expr_cntr -= 1 + expr_offset;
					frmr_end_bracket = true;
				}
				expr_offset++;
			}
			else if (u.getChar() == '^')
			{
				exprs[expr_cntr].powers().push_back(i);
			}
			else if (u.getChar() == '*' || u.getChar() == 'x' || u.getChar() == '/' || u.getChar() == ':'\
				|| u.getChar() == '%')
			{
				exprs[expr_cntr].multiplies().push_back(i);
			}
			else if (u.getChar() == '+' || u.getChar() == '-')
			{
				exprs[expr_cntr].additions().push_back(i);
			}
		}
	}
	if (expr_cntr != 0)
		return false;
	//std::vector<int> to_remove; // deleting elements is critically bad for indexation
	int last_element = 0;
	for (auto it = exprs.rbegin(); it != exprs.rend(); it++) // brackets iteration
	{
		auto& e = *it;
		auto& vl = v[e.getPosStart()];
		for (auto itp = e.powers().rbegin(); itp != e.powers().rend(); itp++)
		{
			last_element = (*itp);
			int prev_idx = last_element - 1;
			int next_idx = last_element + 1;
			unknown_type *p1 = &v[prev_idx], *p2 = &v[next_idx];
			while ((*p1).getType() != DT_DOUBLE)
			{
				prev_idx--;
				p1 = &v[prev_idx];
			}
			while ((*p2).getType() != DT_DOUBLE)
			{
				next_idx++;
				p2 = &v[next_idx];
			}
			auto &pl1 = *p1, &pl2 = *p2;
			double pwr = std::pow(pl1.getDouble(), pl2.getDouble());
			std::cout << pl1.getDouble() << ' ' << v[last_element].getChar() << ' ' << pl2.getDouble() << " = " << pwr << std::endl;
			v[last_element].setDouble(pwr);
			pl1.setBuffer();
			pl2.setBuffer();
		}
		for (auto itm = e.multiplies().begin(); itm != e.multiplies().end(); itm++)
		{
			last_element = (*itm);
			int prev_idx = last_element - 1;
			int next_idx = last_element + 1;
			unknown_type* p1 = &v[prev_idx], * p2 = &v[next_idx];
			while ((*p1).getType() != DT_DOUBLE)
			{
				prev_idx--;
				p1 = &v[prev_idx];
			}
			while ((*p2).getType() != DT_DOUBLE)
			{
				next_idx++;
				p2 = &v[next_idx];
			}
			auto  &pl1 = *p1, &pl2 = *p2;
			double mpl = 0.0;
			switch (v[last_element].getChar())
			{
				case '*':
				{
					mpl = pl1.getDouble() * pl2.getDouble();
					break;
				}
				case 'x':
				{
					mpl = pl1.getDouble() * pl2.getDouble();
					break;
				}
				case '/':
				{
					if (std::nearbyint(pl2.getDouble()) == 0.0)
						return false;
					else
						mpl = pl1.getDouble() / pl2.getDouble();
					break;
				}
				case ':':
				{
					if (std::nearbyint(pl2.getDouble()) == 0.0)
						return false;
					else
						mpl = pl1.getDouble() / pl2.getDouble();
					break;
				}
				case '%':
				{
					if (std::nearbyint(pl2.getDouble()) == 0.0)
						return false;
					else
						mpl = std::fmod(pl1.getDouble(), pl2.getDouble());
					break;
				}
				default:
				{
					break;
				}
			}
			std::cout << pl1.getDouble() << ' ' << v[last_element].getChar() << ' ' << pl2.getDouble() << " = " << mpl << std::endl;
			v[last_element].setDouble(mpl);
			pl1.setBuffer();
			pl2.setBuffer();
		}
		for (auto ita = e.additions().begin(); ita != e.additions().end(); ita++)
		{
			last_element = (*ita);
			int prev_idx = last_element - 1;
			int next_idx = last_element + 1;
			unknown_type *p1 = &v[prev_idx], *p2 = &v[next_idx];
			while ((*p1).getType() != DT_DOUBLE)
			{
				prev_idx--;
				p1 = &v[prev_idx];
			}
			while ((*p2).getType() != DT_DOUBLE)
			{
				next_idx++;
				p2 = &v[next_idx];
			}
			auto &pl1 = *p1, &pl2 = *p2;
			double add = 0.0;
			switch (v[last_element].getChar())
			{
				case '+':
				{
					add = pl1.getDouble() + pl2.getDouble();
					break;
				}
				case '-':
				{
					add = pl1.getDouble() - pl2.getDouble();
					break;
				}
				default:
				{
					break;
				}
			}
			std::cout << pl1.getDouble() << ' ' << v[last_element].getChar() << ' ' << pl2.getDouble() << " = " << add << std::endl;
			v[last_element].setDouble(add);
			pl1.setBuffer();
			pl2.setBuffer();
		}
		v[e.getPosStart()].setBuffer();
		v[e.getPosEnd()].setBuffer();
	}
	r = v[last_element].getDouble();
	return true;
}