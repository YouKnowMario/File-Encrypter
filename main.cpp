#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

void printHelp()
{
	std::cout << "  ______                             _" << std::endl;
	std::cout << " |  ____|                           | | " << std::endl;
	std::cout << " | |__   _ __   ___ _ __ _   _ _ __ | |_ ___ _ __" << std::endl;
	std::cout << " |  __| | '_ \\ / __| '__| | | | '_ \\| __/ _ \\ '__|" << std::endl;
	std::cout << " | |____| | | | (__| |  | |_| | |_) | ||  __/ |   " << std::endl;
	std::cout << " |______|_| |_|\\___|_|   \\__, | .__/ \\__\\___|_|   " << std::endl;
	std::cout << "                          __/ | |                 " << std::endl;
	std::cout << "                         |___/|_|                 " << std::endl;
	std::cout << std::endl;
	std::cout << "encrypt files and folders with password" << std::endl;
	std::cout << std::endl;
	std::cout << "usage: encrypter [options] [input file name] [password] [output file name]" << std::endl;
	std::cout << "-h or --help		print this message" << std::endl;
	std::cout << "-e or --encrypt		encrypt the input file to the output file" << std::endl;
	std::cout << "-d or --decrypt		decrypt the input encrypted file if no output file name " << std::endl;
	std::cout << "			the output will be the encrypted file name before the encryption" << std::endl;
	std::cout << "			if there is an output file name the output will be that name" << std::endl;
}

void encryptFile(std::string name)
{
	std::cout << name.length() << name << std::endl;
	std::ifstream rf(name, std::ios::binary);
	if (!rf) {
		std::cout << "Cannot open file!" << std::endl;
		return;
	}
	uintmax_t fileSize = std::filesystem::file_size(name);
	std::cout << fileSize << std::endl;
	std::ofstream wf("encrypted", std::ios::binary);

	unsigned i = name.length();
	wf.write((char*)&i, sizeof(unsigned));
	wf.write(name.c_str(), i * sizeof(char));
	char temp;

	while (rf.read(&temp, sizeof(char)))
	{
		temp--;
		wf.write(&temp, sizeof(char));
	}
	wf.close();
	rf.close();
}

void decryptFile(std::string name)
{
	
	std::ifstream rf(name, std::ios::binary);
	if (!rf) {
		std::cout << "Cannot open file!" << std::endl;
		return;
	}
	
	std::string fileName;
	unsigned nameLength;
	char temp;
	rf.read((char*)&nameLength, sizeof(unsigned)); // read the length of the file name from the file
	std::vector<char> tempNameArray;
	tempNameArray.resize(nameLength);
	rf.read((char*)&tempNameArray[0], nameLength * sizeof(char)); // read the name of the file from the file
	fileName.assign(tempNameArray.begin(), tempNameArray.end());
	std::ofstream wf(fileName, std::ios::binary);
	if (!wf) {
		std::cout << "Cannot open file for writing!" << std::endl;
		return;
	}
	while (rf.read(&temp, 1)) // reuse
	{
		temp++;
		wf.write(&temp, 1);
	}
	wf.close();
	rf.close();

}

int main(int argc, char* argv[])
{
	if (argc == 1)
		printHelp();
	if (argc < 3)
		return -1;
	
	if (strcmp(argv[1], "encrypt") == 0)
		encryptFile(argv[2]);
	else if (strcmp(argv[1], "decrypt") == 0)
		decryptFile(argv[2]);
	else
		std::cout << "wrong parameter" << std::endl;

}