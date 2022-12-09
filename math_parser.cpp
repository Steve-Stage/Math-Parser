/*
* Math expression parser
* Supports brackets, math operations: +, -, *, /, %, ^
* Credit: SteveState
* @steve_stage
*/

#include <iostream>
#include <cstring>
#include <vector>
#include <conio.h>

enum
{
	DT_DOUBLE = 0, DT_CHAR, DT_BUFFER
};

class unknown_type
{
	double dt1 = 0;
	char dt2 = 0.0;
	unknown_type* dt3 = this;
	int16_t dataType = DT_DOUBLE;
public:
	unknown_type(const double& i) :dt1(i) {}
	unknown_type(const char& i) : dt2(i), dataType(DT_CHAR) {}
	unknown_type(unknown_type* i) : dt3(i), dataType(DT_BUFFER) {}
	int16_t getType()
	{
		return dataType;
	}
	void setDouble(const double& i)
	{
		dt1 = i;
		dt2 = 0;
		dt3 = this;
		dataType = DT_DOUBLE;
	}
	void setChar(const char& i)
	{
		dt1 = 0.0;
		dt2 = i;
		dt3 = this;
		dataType = DT_CHAR;
	}
	void setBuffer(unknown_type* i)
	{
		dt1 = 0.0;
		dt2 = 0;
		dt3 = i;
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
	unknown_type* getBuffer()
	{
		return dt3;
	}
};

class brackets_container
{
	std::vector<int> pwrs, mltpls, addtns;
	int pos_start, pos_end;
public:
	brackets_container() : pos_start(0), pos_end(0) {}
	brackets_container(const int& i) : pos_start(i), pos_end(0) {}
	brackets_container(const int& i1, const int& i2) : pos_start(i1), pos_end(i2) {}
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
};

int math_prior(std::string&);

bool math_calculate(std::vector<unknown_type>&, double&);

int main()
{
	std::string to_parse;
	std::cout << "Enter expression: ";
	std::cin >> to_parse;
	math_prior(to_parse);
	_getch();
	return 0;
}

int math_prior(std::string& expr)
{
	double result;
	int num = 0, exp = 0;
	bool isFloating = false;
	int16_t exp_dgr = 0;
	std::vector<unknown_type> v;
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
		else if (s == '+' || s == '-' || s == '*' || s == '/' || s == 'x' || s == ':' || s == '%' || s == '^' || s == ')')
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
		}
		else if (s == '(')
		{
			v.push_back(unknown_type(s));
		}
	}
	double res = 0.0;
	if(math_calculate(v, res))
		std::cout << res << std::endl;
	else
		std::cout << "NaN" << std::endl;
}

bool math_calculate(std::vector<unknown_type>& v, double& r)
{
	std::vector<brackets_container> exprs = { brackets_container(0, v.size()-1)}; // first - powers, second - multiplies
	int expr_cntr = 0;
	for (int i = 0; i < v.size(); i++)
	{
		unknown_type& u = v[i];
		if (u.getType() == DT_CHAR)
		{
			if (u.getChar() == '(')
			{
				exprs.push_back(brackets_container(i));
				expr_cntr++;
			}
			else if (u.getChar() == ')')
			{
				exprs[expr_cntr].setPosEnd(i);
				expr_cntr--;
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
			int prev_idx = last_element-1;
			int next_idx = last_element+1;
			unknown_type *p1 = &v[prev_idx], *p2 = &v[next_idx];
			while ((*p1).getType() == DT_BUFFER)
			{
				p1 = (*p1).getBuffer();
			}
			while ((*p2).getType() == DT_BUFFER)
			{
				p2 = (*p2).getBuffer();
			}
			auto& pl1 = *p1, & pl2 = *p2;
			double pwr = std::pow(pl1.getDouble(), pl2.getDouble());
			v[last_element].setDouble(pwr);
			pl1.setBuffer(&v[last_element]);
			pl2.setBuffer(&v[last_element]);
		}
		for (auto itm = e.multiplies().begin(); itm != e.multiplies().end(); itm++)
		{
			last_element = (*itm);
			int prev_idx = last_element - 1;
			int next_idx = last_element + 1;
			unknown_type* p1 = &v[prev_idx], * p2 = &v[next_idx];
			while ((*p1).getType() == DT_BUFFER)
			{
				p1 = (*p1).getBuffer();
			}
			while ((*p2).getType() == DT_BUFFER)
			{
				p2 = (*p2).getBuffer();
			}
			auto &pl1 = *p1, &pl2 = *p2;
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
					if (int(pl2.getDouble()) == 0)
						return false;
					else
						mpl = pl1.getDouble() / pl2.getDouble();
					break;
				}
				case ':':
				{
					if (int(pl2.getDouble()) == 0)
						return false;
					else
						mpl = pl1.getDouble() / pl2.getDouble();
					break;
				}
				case '%':
				{
					if (int(pl2.getDouble()) == 0)
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
			v[last_element].setDouble(mpl);
			pl1.setBuffer(&v[last_element]);
			pl2.setBuffer(&v[last_element]);
		}
		for (auto ita = e.additions().begin(); ita != e.additions().end(); ita++)
		{
			last_element = (*ita);
			int prev_idx = last_element - 1;
			int next_idx = last_element + 1;
			unknown_type* p1 = &v[prev_idx], * p2 = &v[next_idx];
			while ((*p1).getType() == DT_BUFFER)
			{
				p1 = (*p1).getBuffer();
			}
			while ((*p2).getType() == DT_BUFFER)
			{
				p2 = (*p2).getBuffer();
			}
			auto& pl1 = *p1, & pl2 = *p2;
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
			v[last_element].setDouble(add);
			pl1.setBuffer(&v[last_element]);
			pl2.setBuffer(&v[last_element]);
		}
		v[e.getPosStart()].setDouble(v[last_element].getDouble());
		v[e.getPosEnd()].setDouble(v[last_element].getDouble());
	}
	r = v[last_element].getDouble();
	return true;
}