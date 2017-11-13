#include<iostream>
#include<set>
#include<tuple>
#include<cstdlib>
#include<string>

#include"nlohmann/json.hpp"
using json = nlohmann::json;

const long long DEFAULT_MAX_STEPS = 100000;

template<typename T>
class Rho {
	public:
		using Set = std::set<T>;
		using Family = std::set<Set>;

		Rho(json mainSetJson) {
			mainSet = toSet(mainSetJson);
			if (mainSet.size() == 0) {
				std::cout << "Empty main set is not allowed, sorry." << std::endl;
				std::exit(0);
			}
		}

		Set toSet(const json & j) {
			Set result;
			for (const T & e : j) {
				result.insert(e);
			}
			return result;
		}

		Family toFamily(const json & j) {
			Family result;
			for (const json & ij : j) {
				result.insert(toSet(ij));
			}
			return result;
		}

		std::tuple<bool, Family> step(const Family & family) {
			Family result;
			for (auto s1 = family.begin(); s1 != family.end(); s1++) {
				const Set & set1 = *s1;
				result.insert(complement(set1));
				for (auto s2 = std::next(s1); s2 != family.end(); s2++) {
					const Set & set2 = *s2;
					if (disjoint(set1, set2)) {
						result.insert(sum(set1, set2));
					}
				}
			}
			return std::make_tuple(equal(family, result), result);
		}
	private:
		Set complement(const Set & s) {
			Set result;
			for (const T & e : mainSet) {
				if (s.count(e) == 0) {
					result.insert(e);
				}
			}
			return result;
		}

		Set sum(const Set & s1, const Set & s2) {
			Set result;
			result.insert(s1.begin(), s1.end());
			result.insert(s2.begin(), s2.end());
			return result;
		}

		bool disjoint(const Set & s1, const Set & s2) {
			for (const T & e : s1) {
				if (s2.count(e) > 0) {
					return false;
				}
			}
			return true;
		}

		bool equal(const Set & s1, const Set & s2) {
			if (s1.size() != s2.size()) {
				return false;
			}

			for (const T & e : s1) {
				if (s2.count(e) == 0) {
					return false;
				}
			}
			return true;
		}

		bool equal(const Family & f1, const Family & f2) {
			if (f1.size() != f2.size()) {
				return false;
			}

			for (const Set & s1 : f1) {
				bool found = false;
				for (const Set & s2 : f2) {
					if (equal(s1,s2)) {
						found = true;
						break;
					}
				}
				if (!found) {
					return false;
				}
			}
			return true;
		}

		Set mainSet;
};

int main(int argc, char * argv[]) {
	long long maxSteps = DEFAULT_MAX_STEPS;
	if (argc > 1) {
		maxSteps = std::stoll(argv[1]);
	}
	json input;
	std::cin >> input;
	Rho<int> rho(input["mainSet"]);
 bool done = false;
	auto family = rho.toFamily(input["family"]);
	long long steps = 0;
	while (!done) {
		std::cout << "=== Step " << steps << " ===" << std::endl;
		std::cout << json(family) << std::endl;
		std::tie(done, family) = rho.step(family);
		steps++;
		if (steps > maxSteps) {
			std::cout << "Maximum number of steps exceeded, stopping." << std::endl;
			return 0;
		}
	}
	std::cout << "=== Step " << steps << " changes nothing. ===" << std::endl;
	return 0;
}
