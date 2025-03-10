// 64 bits is maximum you can get in c++ so it's implemented to do so
// you can edit constexpr var in getRandom64() to get lower amount of bits
#include <iostream>
#include <cstdint>
#include <vector>
#include <random>
#include <bitset>
using namespace std;

class keyGenerate
{	
	public:
	int mulmod(int a, int b, int m)
	{
		int res = 0;
		while (a != 0)
		{
			if (a & 1)
			{
				res = (res + b) % m;
			}
			
			a >>= 1;
			b = (b << 1) % m;
		}
		return res;
	}

	int powMod(int a, int b, int n)
	{
		int x = 1;
	
		a %= n;

		while (b > 0)
		{
			if (b % 2 == 1)
			{
				x = mulmod(x, a, n); // multiplying with base
			}
			a = mulmod(a, a, n); // squaring the base
			b >>= 1;
		}
		return x % n;
	}	

	vector<int> first_primes = { 	2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
					31, 37, 41, 43, 47, 53, 59, 61, 67,
					71, 73, 79, 83, 89, 97, 101, 103,
					107, 109, 113, 127, 131, 137, 139,
					149, 151, 157, 163, 167, 173, 179,
					181, 191, 193, 197, 199, 211, 223,
					227, 229, 233, 239, 241, 251, 257,
					263, 269, 271, 277, 281, 283, 293,
					307, 311, 313, 317, 331, 337, 347, 
					349, 353, 359, 367, 373, 379, 383,
					389, 397, 401, 409, 419, 421, 431,
					433, 439, 443, 449, 457, 461, 463,
					467, 479, 487, 491, 499, 503, 309,
					521, 523, 541 };

// going through all 64 bits and placing randomly 0s and 1s
// setting first and last bit to 1 to get 64 odd number 

	int getRandom64()
	{
	// the value need to be 63 bits because you can not using 64 bit values do a^2 which is needed
		constexpr int bits = 8; 
		std::bitset<bits> a;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<short> distr(0, 1);

		for (int i = 0; i < bits; i++) 
		{
			a[i] = distr(gen);
		}

		a[0] = 1;
		a[bits - 1] = 1;

		return a.to_ullong();
	}

	int getLowLevelPrime()
	{
		while (true)
		{
			int candidate = getRandom64();
			bool is_prime = true;
			for (int i = 0; i < first_primes.size(); i++)
			{
				if (candidate == first_primes[i])
				{
					return candidate;
				}
				
				if (candidate % first_primes[i] == 0)
				{
					is_prime = false;
					break;
				}
			}
			
			if (is_prime)
			{
				return candidate;
			}
		}
	}


	bool trialComposite(int a, int evenC, int to_test, int max_div_2)
	{
		if (powMod(a, evenC, to_test) == 1)
        	return false;

    		for (int i = 0; i < max_div_2; i++)
    		{
        		int temp = (static_cast<int>(1) << i) % to_test; // Use a temporary variable
        		if (powMod(a, temp * evenC, to_test) == to_test - 1)
        		{
           			return false;
           		}
    		}

    		return true;
	}
	
	bool MillerRabinTest(int to_test) {
		constexpr int accuracy = 20;

		int max_div_2 = 0;
		uint64_t evenC = to_test - 1;
		
		while (evenC % 2 == 0)
		{
			evenC >>= 1;
			max_div_2++;
		}

		// random numbers init
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> distr(2, to_test);

		for (int i = 0; i < accuracy; i++)
		{
			int a = distr(gen);

			if (trialComposite(a, evenC, to_test, max_div_2))
			{
				return false;
			}
		}
		return true;
	}
	
	
	int getBigPrime()
	{
		while (true)
		{	
			int candidate = getLowLevelPrime();
			if (MillerRabinTest(candidate))
			{
				return candidate;
			}
		}
	}

	void printBox(const string& text)
	{
    		int width = text.length() + 4; // 2 spaces on each side of the text
   		string horizontalLine(width, '-');

    		// Print the top border
   	 	cout << "+" << horizontalLine << "+" << endl;

   	 	// Print the sides with the text
    		cout << "|  " << text << "  |" << endl;

    		// Print the bottom border
    		cout << "+" << horizontalLine << "+" << endl;
	}
	
	void result(string inputData, string outputData)
	{
		if (inputData == outputData)
    		{
    			printBox("Verification: Decrypted message matches the Original message");
    		}
    		
    		else
   		{
    			printBox("Verification: Decrypted message matches the Original message");
    		}
	}
	
	void About()
	{
		printBox("About");
		cout<<" RSA (Rivest-Shamir-Adleman) is a widely used public-key cryptosystem for secure data transmission. It was introduced in 1977, and it remains one of the most widely used asymmetric encryption algorithms"<<endl<<endl;
	
	printBox("Formula");
	cout<<"Encryption =>  C = P^e mod n"<<endl;
	cout<<"Decryption =>  P = C^d mod n"<<endl<<endl;
	
	
	printBox("Step");
	cout<<"Consider two large prime number p , q "<<endl;
	cout<<"calculate n = p * q" <<endl;
	cout<<"Calculate Euler's Toient Function 'Q(n)': (p-1) * (q-1)"<<endl;
	cout<<"Choose a small number e, co-prime to Euler Totient with GCD[Q(n),e] = 1 and 1<e<Q(n)"<<endl;
	cout<<"find d, such that [d*e mod Q(n)] = 1" <<endl <<endl;

	
	printBox("Rabin Miler");
	cout<<"The Rabin-Miller primality test is a probabilistic algorithm used in the context of RSA (Rivest-Shamir-Adleman) to generate large prime numbers efficiently. In RSA key generation, the security of the system relies on the difficulty of factoring the product of two large prime numbers. Therefore, it is crucial to efficiently find large prime numbers."<<endl<<endl;

	
	printBox("Approach");
	cout<<"Reducing Time by using data decomposition technique.";
	cout<<"As huge data require much time for decryption and encryption, Moreover the RSA algorithm inherently involves modular exponentiation and other mathematical operations that can potentially benefit from parallelization."<<endl<<endl;
	
	
	printBox("Result");
	cout<<"It's essential to note that parallelizing RSA is not straightforward due to the sequential nature of some of its operations, particularly in key generation."<<endl;
	cout<<"But after parallelizing Function such as Encodes and Decoder we have certainly drop time, thus increase the transmission time"<<endl<<endl; 
	
	}

};
