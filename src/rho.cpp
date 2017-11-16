#include<iostream>
#include<unordered_set>
#include<tuple>
#include<cstdlib>
#include<string>

#include"nlohmann/json.hpp"
using json = nlohmann::json;

const long long DEFAULT_MAX_STEPS = 100000;

template<typename T>
class FiniteSet : public std::unordered_set<T> {
	public:
		FiniteSet<T> operator|(FiniteSet<T> const & other) const {
			FiniteSet<T> result;
			result.insert(this->cbegin(), this->cend());
			result.insert(other.cbegin(), other.cend());
			return result;
		}

		FiniteSet<T> operator&(FiniteSet<T> const & other) const {
			FiniteSet<T> result;
			for (auto it = this->cbegin(); it != this->cend(); it++) {
				if (other.count(*it) > 0) {
					result.insert(*it);
				}
			}
			return result;
		}

		FiniteSet<T> operator-(FiniteSet<T> const & other) const {
			FiniteSet<T> result;
			for (auto it = this->cbegin(); it != this->cend(); it++) {
				if (other.count(*it) == 0) {
					result.insert(*it);
				}
			}
			return result;
		}
};

namespace std {
	template<typename T> struct hash<std::unordered_set<T>> {
		std::size_t operator()(std::unordered_set<T> const & us) const noexcept {
			std::size_t result = 0;
			for (T const & e : us) {
				result = std::hash<T>{}(e) + result;
			}
			return result;
		}
	};
	template<typename T> struct hash<FiniteSet<T>> {
		std::size_t operator()(FiniteSet<T> const & fs) const noexcept {
			return std::hash<std::unordered_set<T>>{}(static_cast<std::unordered_set<T>>(fs));
		}
	};
}

template<typename T>
class SigmaClass {
	public:
		using Set = FiniteSet<T>;
		using Family = FiniteSet<Set>;

		bool step() {
			bool anyNew = false;
			Family result;
			result.insert(family.begin(), family.end());
			for (auto s1 = family.begin(); s1 != family.end(); s1++) {
				const Set & set1 = *s1;
				bool inserted;
				std::tie(std::ignore, inserted) = result.insert(complement(set1));
				anyNew |= inserted;
				for (auto s2 = std::next(s1); s2 != family.end(); s2++) {
					const Set & set2 = *s2;
					if ((set1 & set2).empty()) {
						std::tie(std::ignore, inserted) = result.insert(set1 | set2);
						anyNew |= inserted;
					}
				}
			}
			family = result;
			return anyNew;
		}

		void setMainSet(Set ms) {
			mainSet = ms;
		}

		void setFamily(Family f) {
			family = f;
		}

		Set const & getMainSet() const {
			return mainSet;
		}

		Family const & getFamily() const {
			return family;
		}
	private:
		Set complement(Set const & s) {
			return mainSet-s;
		}

		Set mainSet;
		Family family;
};

template<typename T>
void to_json(json & j, SigmaClass<T> const & sc) {
	j = {{"mainSet", sc.getMainSet()}, {"family", sc.getFamily()}};
}

template<typename T>
void from_json(json const & j, SigmaClass<T> & sc) {
	sc.setMainSet(j.at("mainSet"));
	sc.setFamily(j.at("family"));
}

int main(int argc, char * argv[]) {
	long long maxSteps = DEFAULT_MAX_STEPS;
	if (argc > 1) {
		maxSteps = std::stoll(argv[1]);
	}
	json input;
	std::cin >> input;
	SigmaClass<int> rho = input;
	long long steps = 0;
	do {
		std::cout << "=== Step " << steps << " ===" << std::endl;
		std::cout << json(rho.getFamily()) << std::endl;
		steps++;
		if (steps > maxSteps) {
			std::cout << "Maximum number of steps exceeded, stopping." << std::endl;
			return 0;
		}
	} while (rho.step());
	std::cout << "=== Step " << steps << " changes nothing. ===" << std::endl;
	return 0;
}
