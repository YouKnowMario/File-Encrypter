
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <filesystem>
#include <stdio.h>
#include "RC4Enc.h"

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
	std::cout << "-d or --decrypt		decrypts the input encrypted file or directory" << std::endl;
	std::cout << "-E			encrypts the input file and deletes it" << std::endl;
	std::cout << "-D			decrypts the input file or directory and deletes it" << std::endl;
	std::cout << "-ed			encrypts the input directory to the output file" << std::endl;
	std::cout << "-ED			encrypts the input directory to the output file and deletes it" << std::endl;


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

bool ifExist(const char* p_name)
{
	if (std::filesystem::exists(p_name))
	{
		char answer;
		std::cout << "\'" << p_name << "\'" << " is already exists. Overwrite? [y/n] ";
		std::cin >> answer;
		if (answer == 'y')
		{
			std::filesystem::remove_all(p_name);
			return true;
		}
		else
		{
			std::cout << "Not overwriting - exists" << std::endl;
			return false;
		}
	}
	return true;
}

void encryptFile(std::string input, std::string output, std::string password, const int& mode)
{

	if (output == "")
		output = "e_" + input.substr(input.find_first_of('\\') + 1);
	if (!ifExist(output.c_str()))
		return;
	std::ofstream wf(output, std::ios::binary); // open the output file for write
	if (!wf)
	{
		std::cout << output << std::endl;
		std::cout << "cant open file for writing" << std::endl;
		return;
	}
	std::vector<std::string> files;
	// fill the vector and write file header:
	if (mode == 0 || mode == 1)
	{
		files.push_back(input);
		const char* c_format = RC4Enc::encrypt("file", password.c_str(), sizeof(char) * 4, password.size()); // encrypt the format
		wf.write(c_format, sizeof(char) * 4); // write the encrypted format
		delete[] c_format;
	}
	else if (mode == 2 || mode == 3)
	{
		for (std::filesystem::directory_entry name : std::filesystem::directory_iterator(input))
		{
			files.push_back(name.path().string());
		}
		unsigned int filesNum = files.size();
		const char* c_format = RC4Enc::encrypt("dir ", password.c_str(), sizeof(char) * 4, password.size()); // encrypt the format
		wf.write(c_format, sizeof(char) * 4); // write the encrypted format
		const char* c_filesNum = RC4Enc::encrypt((char*)&filesNum, password.c_str(), sizeof(unsigned), password.size()); // encrypt the file number
		wf.write(c_filesNum, sizeof(unsigned)); // write the encrypted file number
		filesNum = input.size(); // reuse the variable

		const char* c_direcNameSize = RC4Enc::encrypt((char*)&filesNum/*direcNameSize*/, password.c_str(), sizeof(unsigned), password.size()); // encrypt the direcNameSize
		wf.write(c_direcNameSize, sizeof(unsigned)); // write the encrypted direcNameSize
		const char* c_direcName = RC4Enc::encrypt(input.c_str() , password.c_str(), filesNum/*direcNameSize*/, password.size()); // encrypt the direcName
		wf.write(c_direcName, filesNum/*direcNameSize*/ * sizeof(char)); // write the encrypted direcName
		delete[] c_format;
		delete[] c_filesNum;
		delete[] c_direcNameSize;
		delete[] c_direcName;
	}
	std::string fileName;
	for (std::string filename : files) // write the data
	{
		fileName = filename.substr(filename.find_first_of('\\') + 1);
		std::ifstream rf(filename, std::ios::binary); // open the file to encrypt
		if (!rf) { // check if exist
			std::cout << "Cannot open file!" << std::endl;
			return;
		}
		if (std::filesystem::file_size(filename) > 4294967295) // if file is more then 4 giga bytes
		{
			std::cout << "file is too big (only files up to 4 gb)" << std::endl;
			return;
		}
		unsigned fileSize;
		unsigned filenamelength;
		fileSize = std::filesystem::file_size(filename);
		filenamelength = fileName.length();
		// write the data to the file
		
		const char* c_filenamelength = RC4Enc::encrypt((char*)&filenamelength, password.c_str(), sizeof(unsigned), password.size()); // encrypt the name size
		wf.write(c_filenamelength, sizeof(unsigned)); // write the encrypted name size
		delete[] c_filenamelength;
		const char* c_filename = RC4Enc::encrypt(fileName.c_str(), password.c_str(), filenamelength, password.size()); // encrypt the file name
		wf.write(c_filename, filenamelength); // write the encrypted file name
		delete[] c_filename;
		const char* c_fileSize = RC4Enc::encrypt((char*)&fileSize, password.c_str(), sizeof(unsigned), password.size()); // encrypt the file size
		wf.write(c_fileSize, sizeof(unsigned)); // write the encrypted file size
		delete[] c_fileSize;
		
		unsigned times = fileSize / 500;
		const char* c_bytes;
		char* bytes = new char[500];
		for (unsigned i = 0; i < times; i++)
		{
			rf.read(bytes, sizeof(char) * 500);
			c_bytes = RC4Enc::encrypt(bytes, password.c_str(), sizeof(char) * 500, password.size());
			wf.write(c_bytes, sizeof(char) * 500);
			delete[] c_bytes;
		}

		unsigned remainingBytes = fileSize % 500;
		if (remainingBytes > 0)
		{
			delete[] bytes; // delete the bytes for reuse
			bytes = new char[remainingBytes];
			rf.read(bytes, sizeof(char) * remainingBytes);
			c_bytes = RC4Enc::encrypt(bytes, password.c_str(), sizeof(char) * remainingBytes, password.size());
			wf.write(c_bytes, sizeof(char) * remainingBytes);
			delete[] c_bytes;
		}
		
		// cleanup:
		delete[] bytes;
		rf.close(); // close the file
		
	}
	if (mode == 1 || mode == 3)
		std::filesystem::remove_all(input);
	wf.close(); // close the file
}

void decryptFile(std::string input, std::string output, std::string password, const int& mode)
{
	std::ifstream rf(input, std::ios::binary); // open the input file for read
	if (!rf) {
		std::cout << "Cannot open file!" << std::endl;
		return;
	}

	char* format = new char[4]; // deleted
	rf.read(format, sizeof(char) * 4);  // read the encrypted format
	format = (char*)RC4Enc::decrypt(format, password.c_str(), sizeof(char) * 4, password.size()); // decrypt the format // deleted
	std::string p_name = "";
	unsigned int p_fileNum = 1;
	if (!strcmp(format, "dir "))
	{
		char* fileNum = new char[sizeof(unsigned)]; // deleted
		rf.read(fileNum, sizeof(unsigned));  // read the encrypted name length
		fileNum = (char*)RC4Enc::decrypt(fileNum, password.c_str(), sizeof(unsigned), password.size());
		std::memcpy(&p_fileNum, fileNum, sizeof(unsigned));
		char* NameLength = new char[sizeof(unsigned)];  // deleted
		unsigned int p_NameLength;
		rf.read(NameLength, sizeof(unsigned));  // read the encrypted name length
		NameLength = (char*)RC4Enc::decrypt(NameLength, password.c_str(), sizeof(unsigned), password.size());
		std::memcpy(&p_NameLength, NameLength, sizeof(unsigned));
		char* name = new char[p_NameLength]; // deleted
		rf.read(name, p_NameLength);  // read the name
		p_name = RC4Enc::decrypt(name, password.c_str(), p_NameLength, password.size());
		delete[] fileNum;
		delete[] NameLength;
		delete[] name;
	}
	
	if (output != "")
	{
		p_name = output;
	}
	
	if (!ifExist(p_name.c_str()))
		goto clean;

	if (!strcmp(format, "dir ")) // the encryption is of directory
	{

		std::filesystem::create_directory(p_name); // create the directory
		p_name = p_name + "\\";
	}
	else if (!strcmp(format, "file"));
	else
	{
		std::cout << "wrong encryption" << std::endl;
		goto clean;
	}
	for (unsigned i = 0; i < p_fileNum; i++)
	{
		char* fileNameLength = new char[sizeof(unsigned)]; // deleted
		unsigned int p_fileNameLength;
		rf.read(fileNameLength, sizeof(unsigned));  // read the encrypted name length
		fileNameLength = (char*)RC4Enc::decrypt(fileNameLength, password.c_str(), sizeof(unsigned), password.size());
		std::memcpy(&p_fileNameLength, fileNameLength, sizeof(unsigned));
		char* name = new char[p_fileNameLength]; // deleted
		rf.read(name, p_fileNameLength);  // read the name
		const char* p_fileName = (char*)RC4Enc::decrypt(name, password.c_str(), p_fileNameLength, password.size());
		std::cout << p_name + p_fileName << std::endl;
		std::ofstream wf(p_name + p_fileName, std::ios::binary); // open the output file to write
		if (!wf) {
			std::cout << "Cannot open file for writing!" << std::endl;
			goto clean;
		}

		char* fileSize = new char[sizeof(unsigned)]; // deleted
		unsigned int p_fileSize;
		rf.read(fileSize, sizeof(unsigned));
		fileSize = (char*)RC4Enc::decrypt(fileSize, password.c_str(), sizeof(unsigned), password.size());
		std::memcpy(&p_fileSize, fileSize, sizeof(unsigned));

		unsigned times = p_fileSize / 500;
		char* p_bytes;
		char* bytes = new char[500]; // deleted
		for (unsigned i = 0; i < times; i++)
		{
			rf.read(bytes, sizeof(char) * 500);
			p_bytes = (char*)RC4Enc::decrypt(bytes, password.c_str(), sizeof(char) * 500, password.size());
			wf.write(p_bytes, sizeof(char) * 500);
			delete[] p_bytes;
		}

		unsigned remainingBytes = p_fileSize % 500;
		if (remainingBytes > 0)
		{
			delete[] bytes; // delete the bytes for reuse
			bytes = new char[remainingBytes]; // deleted
			rf.read(bytes, sizeof(char) * remainingBytes);
			p_bytes = (char*)RC4Enc::decrypt(bytes, password.c_str(), sizeof(char) * remainingBytes, password.size());
			wf.write(p_bytes, sizeof(char) * remainingBytes);
			delete[] p_bytes;
		}
		// cleanup:
		wf.close();
		delete[] fileNameLength;
		delete[] name;
		delete[] p_fileName;
		delete[] fileSize;
		delete[] bytes;

	}
	
clean:

	rf.close();
	delete[] format;
	if (mode == 1)
		std::filesystem::remove_all(input);


}
#ifdef _DEBUG // if debug

int main()
{
	// tests:
	//int argc = 5;
	int argc = 4;
	//const char* argv[5] = { "encrypter", "-e", "tests\\hello_world.png", "goodPass4356", "e_hello_world.png" };
	//const char* argv[4] = { "encrypter", "-e", "tests\\hello_world.png", "goodPass4356"};
	//const char* argv[4] = { "encrypter", "-D", "e_hello_world.png", "goodPass4356"};
	const char* argv[4] = { "encrypter", "-E", "hello_world.png", "goodPass4356" };
	//const char* argv[5] = { "encrypter", "-d", "e_hello_world.png", "goodPass4356", "hello_world.png" };
	//const char* argv[5] = { "encrypter", "-ed", "tests", "goodPass4356", "encryptedDir" };
	//const char* argv[4] = { "encrypter", "-ed", "tests", "goodPass4356"};
	//const char* argv[5] = { "encrypter", "-d", "e_tests", "goodPass4356", "tests2" };
	//const char* argv[5] = { "encrypter", "-D", "e_tests", "goodPass4356", "tests2" };
	//const char* argv[4] = { "encrypter", "-d", "e_hello_world.png", "goodPass4356"};
#else 

int main(int argc, char* argv[])
{
#endif

	if (argc == 1)
	{
		printHelp();
		return -1;
	}
		
	const char* output;
	if (argc < 4)
		return -1;
	else if (argc == 4)
		output = "";
	else
		output = argv[4];
	
	if (!strcmp(argv[1], "--encrypt") || !strcmp(argv[1], "-e"))
		encryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "--decrypt") || !strcmp(argv[1], "-d"))
		decryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "-D"))
		decryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-E"))
		encryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-ed"))
		encryptFile(argv[2], output, argv[3], 2);
	else if (!strcmp(argv[1], "-ED"))
		encryptFile(argv[2], output, argv[3], 3);
	else
		std::cout << "wrong parameter" << std::endl;
	

}