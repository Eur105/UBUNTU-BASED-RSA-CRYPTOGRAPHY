#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <omp.h>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "keyGenerate.h"
set<int> prime;

int publicKey;
int privateKey;
int n;
int P, Q;

	keyGenerate key;
const int CHUNK_SIZE = 1024; // Adjust the chunk size as needed

unsigned int gcd(unsigned int a, unsigned int b)
{
    unsigned int x;
    while (b)
    {
        x = a % b;
        a = b;
        b = x;
    }
    return a;
}

void keyGeneration()
{
    n = P * Q;
    int eulerTotient = (P - 1) * (Q - 1);

    int e = 2;
    while (1)
    {
        if (gcd(e, eulerTotient) == 1)
        {
            break;
        }
        e++;
    }

    publicKey = e;

    int d = 2;
    while (1)
    {
        if ((d * e) % eulerTotient == 1)
        {
            break;
        }
        d++;
    }

    privateKey = d;

    cout << "P: " << P << endl;
    cout << "Q: " << Q << endl
         << endl<<endl;

	key.printBox("Step 02: Generate Public and Private Key");
	
    cout << "Public Key = " << publicKey << endl;
    cout << "Private Key = " << privateKey << endl
         << endl<<endl;
}

long long int modularExponentiation(int base, int exponent, int mod)
{
    long long result = 1;
    base = base % mod;
    if (base < 0)
    {
        base += mod;
    }
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exponent /= 2;
    }
    return result;
}

long long int encryption(int inputData)
{
    return modularExponentiation(inputData, publicKey, n);
}

int decryption(int encryptedText)
{
    return modularExponentiation(encryptedText, privateKey, n);
}


vector<int> encoder(const string &inputData)
{
    vector<int> form(inputData.size());

for (int i = 0; i < inputData.size(); ++i)
    {
        form[i] = encryption((int)inputData[i]);
    }

    return form;
}


string decoder(const vector<int> &encoded)
{
    string s(encoded.size(), 0);

    for (int i = 0; i < encoded.size(); ++i)
    {
        int decryptedValue = decryption(encoded[i]);
        s[i] = static_cast<char>(modularExponentiation(decryptedValue, 1, 256));
    }

    return s;
}



int main()
{
int flag = 0;
int option;
string fileName;
double timeSpent = 0;
key.printBox("Select Option");

cout<<"\t\t1. About \n\t\t2. Run RSA"<<endl;
cout<<"Enter Your Choice: ";cin>>option;

switch(option)
{
	case 1:
		key.About();
		return 0;
		break;
	case 2:
		flag = 1;
		break;
	default:
		cout<<"\n\nInvalid Input\n";
		break;
		return 0;
}

if(flag == 1)
{
	string chunk;
    key.printBox("Step 01: Getting Random P and Q by Rabin Miller");
	 P = key.getBigPrime();
	 Q = key.getBigPrime();
	 if(P == Q)
	 {
	 	Q = key.getBigPrime();
	 	
	 }
	
	
point:	
	key.printBox("Select File Size");
cout<<"\t\t1. 1 MB \n\t\t2. 5MB	\n\t\t3. 10MB \n\t\t4. 20MB"<<endl;
cout<<"Enter Your Choice: ";cin>>option;

switch(option)
{
	case 1:
		fileName = "sample.txt";
		break;
	case 2:
		fileName = "sample02.txt";
		break;
	case 3:
		fileName = "sample03.txt";
		break;
	case 4:
		fileName = "sample04.txt";
		break;		
	default:
		cout<<"\n\nInvalid Input \n\n\n\n\n\n";
		goto point;
		break;
}
    ifstream fInput(fileName);
    stringstream buffer;
    buffer << fInput.rdbuf();

    fInput.close();

    string inputData = buffer.str();


    fInput.open(fileName);

    if (!fInput.is_open())
    {
        cout << "Error opening the input file." << endl;
        return -1;
    }

    keyGeneration();


    key.printBox("Program Vitals");
    long totalSize = inputData.size();
    
    cout<<"File Size =  "<<totalSize <<endl;
    	
    
    clock_t begin = clock();
    
    vector<int> coded = encoder(inputData);
    
    string outputData = decoder(coded);
    
    clock_t end = clock();
    
    fInput.close();
    
    cout<<endl;
    key.printBox("Encrypted and Decrypted Data Save Successfully");
    
    ofstream encryptFile("EncryptedData.txt", ios::out | ios::trunc);
    ofstream decodeFile("DecodedData.txt", ios::out | ios::trunc);

    if (encryptFile.is_open() || decodeFile.is_open())
    {
    decodeFile << outputData;
        decodeFile.close();
        
    for (auto &p : coded)
    {
        
        encryptFile << p;
    }
        encryptFile.close();
        cout<< "Decoded Data has been written to 'DecodedData.txt'" << endl;
             cout << "Encrypted Data has been written to 'EncryptedData.txt'" << endl <<endl<<endl;
   
    }
    else
    {
        cout <<endl<<endl<< "Error opening the output file for writing." << endl<<endl;
        return -1;
    }
   
    

    timeSpent += (double)(end - begin) / CLOCKS_PER_SEC;

    key.printBox("Time Complexity");
    cout<<"Time (Seconds) = "<<timeSpent <<endl <<endl<<endl;
    
    key.result(inputData, outputData);
  
 }   
}
       

