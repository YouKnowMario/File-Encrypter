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
	std::cout << std::endl;
	std::cout << "for more info visit the docs - www.exmple.com" << std::endl;
	std::cout << "-h or --help		print this message" << std::endl;
	std::cout << "-e or --encrypt		encrypts the input file to the output file" << std::endl;
	std::cout << "-d or --decrypt		decrypts the input encrypted file" << std::endl;
	std::cout << "-E			encrypts the input file and deletes it" << std::endl;
	std::cout << "-D			decrypts the input file and deletes it" << std::endl;
	std::cout << "-ed			encrypts the input directory to the output file" << std::endl;
	std::cout << "-dd			decrypts the input encrypted directory file" << std::endl;
	std::cout << "-ED			encrypts the input directory to the output file and deletes it" << std::endl;
	std::cout << "-DD			decrypts the input encrypted directory file and deletes it" << std::endl;

}

// encrypted file structure: (everything is encrypted)
// required | 4 bytes | format | "dir " for directory, "file" for single file
// optional (if directory) | 4 bytes | fileNum | number of files | 
// optional (if directory) | 4 bytes | direcNameSize | length of the name of the directory |
// optional (if directory) | direcNameSize | direcName | the name of the directory |
// required | | data | the data
// 
// data structure: if more than one file this is repeated [fileNum] times
// 4 bytes | fileNameSize | length of the file name
// fileNameSize | fileName | file name
// 4 bytes | fileSize | the size of the file (in bytes)
// fileSize | fileData | the data of the file
//


void encryptFile(std::string input, std::string output, std::string password, const int& mode)
{
	std::ofstream wf(output, std::ios::binary); // open the output file for write

	std::vector<std::string> files;
	// fill the vector and write file header:
	if (mode == 0 || mode == 1)
	{
		files.push_back(input);
		wf.write("file", sizeof(char) * 4);  // write the format
	}
	else if (mode == 2 || mode == 3)
	{
		for (std::filesystem::directory_entry name : std::filesystem::directory_iterator(input))
		{
			files.push_back(name.path().string());
		}
		unsigned i = files.size();
		wf.write("dir ", sizeof(char) * 4); // write the format
		wf.write((char*)&i, sizeof(unsigned)); // write the fileNum
		i = input.size();
		wf.write((char*)&i, sizeof(unsigned)); // write the direcNameSize
		wf.write(input.c_str(), i * sizeof(char)); // write the direcName
	}
	//std::cout << input.length() << input << std::endl;
	if (output == "")
		output = "encryptedFile";
	
	for (auto filename : files) // write the data
	{
		std::ifstream rf(filename, std::ios::binary);
		if (!rf) {
			std::cout << "Cannot open file!" << std::endl;
			return;
		}
		uintmax_t fileSize = std::filesystem::file_size(filename);
		std::cout << fileSize << std::endl;
		unsigned i = filename.length();
		wf.write((char*)&i, sizeof(unsigned));
		wf.write(filename.c_str(), i * sizeof(char));
		wf.write((char*)&fileSize, sizeof(uintmax_t));
		char temp;
		while (rf.read(&temp, sizeof(char)))
		{
			temp--;
			wf.write(&temp, sizeof(char));
		}
	}
	
	wf.close();
	
}

void decryptFile(std::string input, std::string output, std::string password, const int& mode)
{
	
	std::ifstream rf(input, std::ios::binary);
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
	const char* output;
	if (argc < 4)
		return -1;
	if (argc == 4)
		output = "";
	else
		output = argv[4];
	
	if (!strcmp(argv[1], "--encrypt") || !strcmp(argv[1], "-e"))
		encryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "--decrypt") || !strcmp(argv[1], "-d"))
		decryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "-D"))
		encryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-E"))
		decryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-ed"))
		encryptFile(argv[2], output, argv[3], 2);
	else if (!strcmp(argv[1], "-dd"))
		decryptFile(argv[2], output, argv[3], 2);
	else if (!strcmp(argv[1], "-ED"))
		encryptFile(argv[2], output, argv[3], 3);
	else if (!strcmp(argv[1], "-DD"))
		decryptFile(argv[2], output, argv[3], 3);
	else
		std::cout << "wrong parameter" << std::endl;

}