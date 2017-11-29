#include<iostream>
#include<unordered_set>
#include<map>
#include<tuple>
#include<cstdlib>
#include<string>
#include<bitset>

#include"nlohmann/json.hpp"
using json = nlohmann::json;

const long long DEFAULT_MAX_STEPS = 100000;
const long long MAX_MAIN_SET_SIZE = 1000;

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
		using InternalSet = std::bitset<MAX_MAIN_SET_SIZE>;
		using Set = FiniteSet<T>;
		using InternalFamily = FiniteSet<InternalSet>;
		using Family = FiniteSet<Set>;

		bool step() {
			bool anyNew = false;
			InternalFamily result;
			result.insert(family.begin(), family.end());
			for (auto s1 = family.begin(); s1 != family.end(); s1++) {
				const InternalSet & set1 = *s1;
				bool inserted;
				std::tie(std::ignore, inserted) = result.insert((~set1) & mainSet);
				anyNew |= inserted;
				for (auto s2 = std::next(s1); s2 != family.end(); s2++) {
					const InternalSet & set2 = *s2;
					if ((set1 & set2).none()) {
						std::tie(std::ignore, inserted) = result.insert(set1 | set2);
						anyNew |= inserted;
					}
				}
			}
			family = result;
			return anyNew;
		}

		void setMainSet(Set const & ms) {
			mainSet.flip();
			mainSetSize = ms.size();
			mainSet <<= mainSetSize;
			mainSet.flip();
			int idx = 0;
			for (T const & e : ms) {
				toRepresentation[e] = idx;
				idx++;
			}
			assert(idx==mainSetSize);
			for (auto const & tr : toRepresentation) {
				toElement[tr.second] = tr.first;
			}
		}

		void setFamily(Family const & f) {
			family = encodeFamily(f);
		}

		Set getMainSet() const {
			return decodeSet(mainSet);
		}

		Family getFamily() const {
			return decodeFamily(family);
		}
	private:
		InternalSet encodeSet(Set const & s) const {
			InternalSet result;
			for (T const & e : s) {
				if (toRepresentation.count(e) == 0) {
					std::cerr << "Element " << e << " in a set in the family, but not in the main set!" << std::endl;
				}
				result.set(toRepresentation.at(e));
			}
			return result;
		}

		Set decodeSet(InternalSet const & is) const {
			Set result;
			for (int i = 0; i < mainSetSize; i++) {
				if (is.test(i)) {
					result.insert(toElement.at(i));
				}
			}
			return result;
		}

		InternalFamily encodeFamily(Family const & f) const {
			InternalFamily result;
			for (Set const & s : f) {
				result.insert(encodeSet(s));
			}
			return result;
		}

		Family decodeFamily(InternalFamily const & inf) const {
			Family result;
			for (InternalSet const & is : inf) {
				result.insert(decodeSet(is));
			}
			return result;
		}

		std::map<int, T> toElement;
		std::map<T, int> toRepresentation;
		InternalSet mainSet;
		int mainSetSize;
		InternalFamily family;
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
		auto fam = rho.getFamily();
		std::cout << "Family size: " << fam.size() << std::endl;
		std::cout << json(fam) << std::endl;
		steps++;
		if (steps > maxSteps) {
			std::cout << "Maximum number of steps exceeded, stopping." << std::endl;
			return 0;
		}
	} while (rho.step());
	std::cout << "=== Step " << steps << " changes nothing. ===" << std::endl;
	return 0;
}
