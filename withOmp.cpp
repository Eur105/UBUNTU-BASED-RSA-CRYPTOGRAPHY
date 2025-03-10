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
int flag;

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

	
#pragma omp parallel
	{
		#pragma omp for
   	 	for (int i = 0; i < inputData.size(); ++i)
    		{
        		form[i] = encryption((int)inputData[i]);
    		}
	}
    return form;
}


string decoder(const vector<int> &encoded)
{
    string s(encoded.size(), 0);

#pragma omp parallel
	{
		#pragma omp for
		    for (int i = 0; i < encoded.size(); ++i)
    		{
       		 int decryptedValue = decryption(encoded[i]);
        		s[i] = static_cast<char>(modularExponentiation(decryptedValue, 1, 256));
    		}
	}
	
    return s;
}


void processChunk(int chunkNumber, const string &chunk, vector<int> &result)
{
    vector<int> codedChunk = encoder(chunk);
   
    // Print thread ID and chunk information
   #pragma omp critical
    {
    	if(flag == 1)
    	{
    	cout << "\n\t\t\t\t\t\tThread "<<omp_get_thread_num()<<" , Processing Chunk ("<<chunkNumber <<")";
	cout << "\n\t\t\t\t\t=============================================\n";
	
		cout<<chunk<<endl<<endl;
	}
 	int index = chunkNumber * CHUNK_SIZE;
	if (index + codedChunk.size() > result.size())
    {
        result.resize(index + codedChunk.size());
    }
    // Insert the values at the specific index
    
    for (int i = 0; i <=codedChunk.size(); i++)
    {
        result[index + i] = codedChunk[i];
    }
    }
}


int main()
{

int flag2 = 0;
int option;
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
		flag2 = 1;
		break;
	default:
		cout<<"\n\nInvalid Input\n";
		break;
		return 0;
}

if(flag2 == 1)
{
string chunk;
    key.printBox("Step 01: Getting Random P and Q by Rabin Miller");
	 P = key.getBigPrime();
	 Q = key.getBigPrime();
	 if(P == Q)
	 {
	 	Q = key.getBigPrime();
	 	
	 }
	double timeSpent = 0;
	
    ifstream fInput("sample02.txt");
    stringstream buffer;
    buffer << fInput.rdbuf();

    fInput.close();

    string inputData = buffer.str();


    fInput.open("sample02.txt");

    if (!fInput.is_open())
    {
        cout << "Error opening the input file." << endl;
        return -1;
    }

    keyGeneration();


    vector<int> coded;
    
    key.printBox("Program Vitals");
    long totalSize = inputData.size();
    
    cout<<"File Size =  "<<totalSize <<endl;
    
    // Calculate the number of chunks based on CHUNK_SIZE
    int numChunks = (totalSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
    cout<<"Chunk Size = "<<CHUNK_SIZE<<endl;
    
	cout<<"Number of Chunks = " <<numChunks <<endl << endl<<endl;
	int threadCount = omp_get_max_threads();
	
	omp_set_num_threads(threadCount);	
	
	
    // Use OpenMP to parallelize the loop
    int option;
    key.printBox("Select Option");
    cout<<"\t\t 1. Data Decomposition \n\t\t 2. Skip to Result" <<endl;
    cout<<"Enter Your Option: ";cin>>option;
    cout<<endl<<endl<<endl;
    
    switch(option)
    {
    	case 1:
    		key.printBox("Show Data Decomposition on Threads");
    		flag = 1;
    		break;
    	case 2:
    		flag = 0;
    }
    
    clock_t begin = clock();

    #pragma omp parallel for private(chunk)
    for (int i = 0; i < numChunks; ++i)
    {
        int startPos = i * CHUNK_SIZE;
        int endPos = (i + 1) * CHUNK_SIZE;
        if (endPos > totalSize)
            endPos = totalSize;

        chunk = inputData.substr(startPos, endPos - startPos);

        // Process the chunk in parallel
        
        processChunk(i,chunk, coded);

        #pragma omp critical
        {
//            // Print additional information if needed
            //cout << "Thread " << omp_get_thread_num() << " processed chunk " << i << endl;
        }
    }
    
    
    fInput.close();
    
    
      clock_t end = clock();
      
    string outputData = decoder(coded);
    

    
        
	
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
       

